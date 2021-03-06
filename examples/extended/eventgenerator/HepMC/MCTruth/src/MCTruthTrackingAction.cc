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
//
// $Id: MCTruthTrackingAction.cc,v 1.1 2006-11-22 14:51:30 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
// --------------------------------------------------------------
//      GEANT 4 - MCTruthTrackingAction class
// --------------------------------------------------------------
//
// Author: Witold POKORSKI (Witold.Pokorski@cern.ch)
//
// --------------------------------------------------------------

#include <iostream>

#include "G4Track.hh"
#include "G4TrackVector.hh"
#include "G4TrackingManager.hh"
#include "MCTruthTrackingAction.hh"
#include "MCTruthTrackInformation.hh"

MCTruthTrackingAction::MCTruthTrackingAction() 
{}

MCTruthTrackingAction::~MCTruthTrackingAction() 
{} 

void MCTruthTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  fmom = G4LorentzVector(track->GetMomentum(), track->GetTotalEnergy());
  
  if(!track->GetUserInformation()) 
  {
    G4VUserTrackInformation* mcinf = new MCTruthTrackInformation;
    fpTrackingManager->SetUserTrackInformation(mcinf);
  }
}

void MCTruthTrackingAction::PostUserTrackingAction(const G4Track* track)
{

  G4LorentzVector prodpos(track->GetGlobalTime() - track->GetLocalTime(),
                             track->GetVertexPosition());
  G4LorentzVector endpos(track->GetGlobalTime(), track->GetPosition());

  // here (?) make all different checks to decide whether to store the particle
  //
  if (trackToBeStored(track))
  {
    MCTruthTrackInformation* mcinf =
      (MCTruthTrackInformation*) track->GetUserInformation();

    MCTruthManager::GetInstance()->
      AddParticle(fmom, prodpos, endpos,
                  track->GetDefinition()->GetPDGEncoding(),
                  track->GetTrackID(),
                  track->GetParentID(), mcinf->GetDirectParent());
  }
  else
  {
    // If track is not to be stored, propagate it's parent ID (stored)
    // to its secondaries
    //
    G4TrackVector* childrens = fpTrackingManager->GimmeSecondaries() ;

    for( unsigned int index = 0 ; index < childrens->size() ; ++index )
    {
      G4Track* tr = (*childrens)[index] ;
      tr->SetParentID( track->GetParentID() );

      // set the flag saying that the direct mother is not stored
      //
      MCTruthTrackInformation* mcinf =
        (MCTruthTrackInformation*) tr->GetUserInformation();
      if(!mcinf)
        tr->SetUserInformation(mcinf = new MCTruthTrackInformation);

      mcinf->SetDirectParent(false); 
    }     
  }
}

G4bool MCTruthTrackingAction::trackToBeStored(const G4Track* track)
{
  MCTruthConfig* config = MCTruthManager::GetInstance()->GetConfig();
  
  // check energy
  if (fmom.e() > config->GetMinE()) return true;
  
  // particle type
  std::vector<G4int> types = config->GetParticleTypes();
  
  if(std::find( types.begin(), types.end(),
                track->GetDefinition()->GetPDGEncoding())
     != types.end()) return true;

  // creator process

  // etc...
  
  return false;
}
