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
// $Id: RE01SteppingAction.cc,v 1.2 2006-06-29 17:44:23 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//


#include "RE01SteppingAction.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Region.hh"
#include "RE01RegionInformation.hh"

RE01SteppingAction::RE01SteppingAction()
{;}

RE01SteppingAction::~RE01SteppingAction()
{;}

void RE01SteppingAction::UserSteppingAction(const G4Step * theStep)
{
  // Suspend a track if it is entering into the calorimeter

  // check if it is alive
  G4Track * theTrack = theStep->GetTrack();
  if(theTrack->GetTrackStatus()!=fAlive) { return; }

  // get region information
  G4StepPoint * thePrePoint = theStep->GetPreStepPoint();
  G4LogicalVolume * thePreLV = thePrePoint->GetPhysicalVolume()->GetLogicalVolume();
  RE01RegionInformation* thePreRInfo
   = (RE01RegionInformation*)(thePreLV->GetRegion()->GetUserInformation());
  G4StepPoint * thePostPoint = theStep->GetPostStepPoint();
  G4LogicalVolume * thePostLV = thePostPoint->GetPhysicalVolume()->GetLogicalVolume();
  RE01RegionInformation* thePostRInfo
   = (RE01RegionInformation*)(thePostLV->GetRegion()->GetUserInformation());

  // check if it is entering to the calorimeter volume
  if(!(thePreRInfo->IsCalorimeter()) && (thePostRInfo->IsCalorimeter()))
  { theTrack->SetTrackStatus(fSuspend); }
}


