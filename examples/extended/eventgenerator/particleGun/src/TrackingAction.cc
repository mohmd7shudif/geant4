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
// $Id: TrackingAction.cc,v 1.3 2010-07-16 07:37:48 maire Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 

#include "TrackingAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorAction2.hh"
#include "PrimaryGeneratorAction3.hh"
#include "PrimaryGeneratorAction4.hh"
#include "RunAction.hh"
#include "HistoManager.hh"

#include "G4Track.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingAction::TrackingAction(PrimaryGeneratorAction* prim,HistoManager* histo)
:primary(prim),histoManager(histo)
{ 
 // parameters for generator action #3
  newUz = primary->GetAction3()->GetNewUz();

 // parameters for generator action #4
  deltaR3 = 
    (primary->GetAction4()->GetRmax3() - primary->GetAction4()->GetRmin3())/3.;
  cosAlphaMin = primary->GetAction4()->GetCosAlphaMin();
  cosAlphaMax = primary->GetAction4()->GetCosAlphaMax();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PreUserTrackingAction(const G4Track* track)
{
 G4int selectedGeneratorAction = primary->GetSelectedAction();
 G4int id = 0;

 if(selectedGeneratorAction==2)
 {
  //energy spectrum
  //
  id = 1;   
  histoManager->FillHisto(id, track->GetKineticEnergy());
 }
 else if(selectedGeneratorAction==3)
 {
  //momentum direction : angular distr dN/dOmega = f(alpha)
  //
  id = 2;
  G4ThreeVector um = track->GetMomentumDirection();
  G4double alpha  = std::acos(newUz*um);
  G4double dalfa  = histoManager->GetBinWidth(id);
  G4double dOmega = twopi*std::sin(alpha)*dalfa;     
  if (dOmega > 0.) histoManager->FillHisto(id, alpha, 1./dOmega);
    
  //momentum direction : angular distr dN/dOmega = f(psi)
  //
  id = 3;
  // complete local frame
  G4ThreeVector u1(1.,0.,0.);  u1.rotateUz(newUz);
  G4ThreeVector u2(0.,1.,0.);  u2.rotateUz(newUz);  
  //  
  G4double psi = std::atan2(u2*um, u1*um);
  if (psi < 0.) psi += twopi;    
  G4double dpsi  = histoManager->GetBinWidth(id);
  G4double alphaMax = primary->GetAction3()->GetAlphaMax();    
  dOmega = (1. - std::cos(alphaMax))*dpsi;     
  if (dOmega > 0.) histoManager->FillHisto(id, psi, 1./dOmega);  
 }
 else if(selectedGeneratorAction==4)
 {
  G4ThreeVector vertex   = track->GetVertexPosition();
  G4double r = vertex.mag();
  if (r <= 0.0) return;
  //local frame : new uz = ur
  G4ThreeVector ur   = vertex/r;

  //vertex position: radial distribution : dN/dv = f(r)
  //
  id = 4;
  G4double dr = histoManager->GetBinWidth(id);
  G4double dv = 2*twopi*r*r*dr;
  if (dv > 0.) histoManager->FillHisto(id, r, 1./dv);

  //vertex position: angular distribution : dN/dv = f(theta)
  //
  id = 5;
  G4double theta  = std::acos(ur.z());
  G4double dteta  = histoManager->GetBinWidth(id);
  dv = deltaR3*twopi*std::sin(theta)*dteta;
  if (dv > 0.) histoManager->FillHisto(id, theta, 1./dv);

  //vertex position: angular distribution : dN/dv = f(phi)
  //
  id = 6;
  G4double phi  = std::atan2(ur.y(), ur.x());
  if (phi < 0.) phi += twopi;
  G4double dphi  = histoManager->GetBinWidth(id);
  dv = 2*deltaR3*dphi;
  if (dv > 0.) histoManager->FillHisto(id, phi, 1./dv);

  //momentum direction : angular distr dN/dOmega = f(alpha)
  //
  id = 7;
  G4ThreeVector um = track->GetMomentumDirection();
  G4double alpha  = std::acos(ur*um);
  G4double dalfa  = histoManager->GetBinWidth(id);
  G4double dOmega = twopi*std::sin(alpha)*dalfa;
  if (dOmega > 0.) histoManager->FillHisto(id, alpha, 1./dOmega);

  //momentum direction : angular distr dN/dOmega = f(psi)
  //
  id = 8;
  // complete local frame
  G4ThreeVector u1(1.,0.,0.);  u1.rotateUz(ur);
  G4ThreeVector u2(0.,1.,0.);  u2.rotateUz(ur);
  //
  G4double psi = std::atan2(u2*um, u1*um);
  if (psi < 0.) psi += twopi;
  G4double dpsi  = histoManager->GetBinWidth(id);
  dOmega = (cosAlphaMin - cosAlphaMax)*dpsi;
  if (dOmega > 0.) histoManager->FillHisto(id, psi, 1./dOmega);
 }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PostUserTrackingAction(const G4Track*)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

