//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: G4AdjointStackingAction.cc,v 1.2 2009-11-18 17:57:59 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
/////////////////////////////////////////////////////////////////////////////
//      Class Name:	G4AdjointCrossSurfChecker
//	Author:       	L. Desorgher
// 	Organisation: 	SpaceIT GmbH
//	Contract:	ESA contract 21435/08/NL/AT
// 	Customer:     	ESA/ESTEC
/////////////////////////////////////////////////////////////////////////////

#include "G4AdjointStackingAction.hh"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4ios.hh"

G4AdjointStackingAction::G4AdjointStackingAction()
  : kill_tracks(true), adjoint_mode(true)
{
  theFwdStackingAction =0;
  theUserAdjointStackingAction =0;
}
////////////////////////////////////////////////////////////////////////////////
//
G4AdjointStackingAction::~G4AdjointStackingAction()
{;}
////////////////////////////////////////////////////////////////////////////////
//
G4ClassificationOfNewTrack  G4AdjointStackingAction::ClassifyNewTrack(const G4Track * aTrack)
{  
   G4ClassificationOfNewTrack classification = fUrgent;
   if ( kill_tracks) classification=fKill;
   else if (!adjoint_mode && theFwdStackingAction)   classification =  theFwdStackingAction->ClassifyNewTrack(aTrack); 
   else if (adjoint_mode && theUserAdjointStackingAction)   classification =  theUserAdjointStackingAction->ClassifyNewTrack(aTrack);
   return classification;
}
////////////////////////////////////////////////////////////////////////////////
//
void G4AdjointStackingAction::NewStage()
{
  if ( !adjoint_mode && theFwdStackingAction)  theFwdStackingAction->NewStage(); 
  else if (adjoint_mode && theUserAdjointStackingAction)   theUserAdjointStackingAction->NewStage();
}
////////////////////////////////////////////////////////////////////////////////
//    
void G4AdjointStackingAction::PrepareNewEvent()
{
  if ( !adjoint_mode && theFwdStackingAction)  theFwdStackingAction->PrepareNewEvent();
  else if (adjoint_mode && theUserAdjointStackingAction)   theUserAdjointStackingAction->PrepareNewEvent();
}

