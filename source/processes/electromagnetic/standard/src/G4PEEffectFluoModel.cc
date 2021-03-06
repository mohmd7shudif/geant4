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
// $Id: G4PEEffectFluoModel.cc,v 1.4 2010-11-21 16:08:37 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
//
// File name:     G4PEEffectFluoModel
//
// Author:        Vladimir Ivanchenko on base of G4PEEffectModel
//
// Creation date: 13.06.2010
//
// Modifications:
//
// Class Description:
// Implementation of the photo-electric effect with deexcitation
//
// -------------------------------------------------------------------
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "G4PEEffectFluoModel.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "Randomize.hh"
#include "G4DataVector.hh"
#include "G4ParticleChangeForGamma.hh"
#include "G4VAtomDeexcitation.hh"
#include "G4LossTableManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

using namespace std;

G4PEEffectFluoModel::G4PEEffectFluoModel(const G4String& nam)
  : G4VEmModel(nam),isInitialized(false)
{
  theGamma    = G4Gamma::Gamma();
  theElectron = G4Electron::Electron();
  fminimalEnergy = 1.0*eV;
  SetDeexcitationFlag(true);
  fParticleChange = 0;
  fAtomDeexcitation = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4PEEffectFluoModel::~G4PEEffectFluoModel()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4PEEffectFluoModel::Initialise(const G4ParticleDefinition*,
				     const G4DataVector&)
{
  fAtomDeexcitation = G4LossTableManager::Instance()->AtomDeexcitation();

  if (isInitialized) { return; }
  fParticleChange = GetParticleChangeForGamma();
  isInitialized = true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

G4double 
G4PEEffectFluoModel::ComputeCrossSectionPerAtom(const G4ParticleDefinition*,
						G4double energy,
						G4double Z, G4double,
						G4double, G4double)
{
  G4double* SandiaCof = G4SandiaTable::GetSandiaCofPerAtom((G4int)Z, energy);

  G4double energy2 = energy*energy;
  G4double energy3 = energy*energy2;
  G4double energy4 = energy2*energy2;

  return SandiaCof[0]/energy  + SandiaCof[1]/energy2 +
    SandiaCof[2]/energy3 + SandiaCof[3]/energy4;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double 
G4PEEffectFluoModel::CrossSectionPerVolume(const G4Material* material,
					   const G4ParticleDefinition*,
					   G4double energy,
					   G4double, G4double)
{
  G4double* SandiaCof = 
    material->GetSandiaTable()->GetSandiaCofForMaterial(energy);
				
  G4double energy2 = energy*energy;
  G4double energy3 = energy*energy2;
  G4double energy4 = energy2*energy2;
	  
  return SandiaCof[0]/energy  + SandiaCof[1]/energy2 +
    SandiaCof[2]/energy3 + SandiaCof[3]/energy4; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void 
G4PEEffectFluoModel::SampleSecondaries(std::vector<G4DynamicParticle*>* fvect,
				       const G4MaterialCutsCouple* couple,
				       const G4DynamicParticle* aDynamicPhoton,
				       G4double,
				       G4double)
{
  const G4Material* aMaterial = couple->GetMaterial();

  G4double energy = aDynamicPhoton->GetKineticEnergy();
  G4ParticleMomentum PhotonDirection = aDynamicPhoton->GetMomentumDirection();

  // select randomly one element constituing the material.
  const G4Element* anElement = SelectRandomAtom(aMaterial,theGamma,energy);
  
  //
  // Photo electron
  //

  // Select atomic shell
  G4int nShells = anElement->GetNbOfAtomicShells();
  G4int i = 0;  
  for(; i<nShells; ++i) {
    /*
    G4cout << "i= " << i << " E(eV)= " << energy/eV 
    	   << " Eb(eV)= " << anElement->GetAtomicShell(i)/eV
           << "  " << anElement->GetName() 
	   << G4endl;
    */
    if(energy >= anElement->GetAtomicShell(i)) { break; }
  }
  // no shell available
  if (i == nShells) { return; }
  
  G4double bindingEnergy  = anElement->GetAtomicShell(i);
  G4double ElecKineEnergy = energy - bindingEnergy;
  G4double edep = bindingEnergy;

  // create photo electron
  //
  if (ElecKineEnergy > fminimalEnergy) {
    G4double cosTeta = ElecCosThetaDistribution(ElecKineEnergy);
    G4double sinTeta = sqrt(1.-cosTeta*cosTeta);
    G4double Phi     = twopi * G4UniformRand();
    G4double dirx = sinTeta*cos(Phi),diry = sinTeta*sin(Phi),dirz = cosTeta;
    G4ThreeVector ElecDirection(dirx,diry,dirz);
    ElecDirection.rotateUz(PhotonDirection);
    //
    G4DynamicParticle* aParticle = new G4DynamicParticle (
                       theElectron,ElecDirection, ElecKineEnergy);
    fvect->push_back(aParticle);
  } else { edep = energy; }

  // sample deexcitation
  //
  if(fAtomDeexcitation) {
    G4int index = couple->GetIndex();
    if(fAtomDeexcitation->CheckDeexcitationActiveRegion(index)) {
      G4int Z = (G4int)anElement->GetZ();
      G4AtomicShellEnumerator as = G4AtomicShellEnumerator(i);
      const G4AtomicShell* shell = fAtomDeexcitation->GetAtomicShell(Z, as);
      size_t nbefore = fvect->size();
      fAtomDeexcitation->GenerateParticles(fvect, shell, Z, index);
      size_t nafter = fvect->size();
      if(nafter > nbefore) {
	for (size_t i=nbefore; i<nafter; ++i) {
	  edep -= ((*fvect)[i])->GetKineticEnergy();
	} 
      }
    }
  }
  if(edep < 0.0) { edep = 0.0; }

  // kill primary photon
  fParticleChange->SetProposedKineticEnergy(0.);
  fParticleChange->ProposeTrackStatus(fStopAndKill);
  fParticleChange->ProposeLocalEnergyDeposit(edep);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4PEEffectFluoModel::ElecCosThetaDistribution(G4double kineEnergy)
{
  // Compute Theta distribution of the emitted electron, with respect to the
  // incident Gamma.
  // The Sauter-Gavrila distribution for the K-shell is used.
  //
  G4double costeta = 1.;
  G4double gamma   = 1. + kineEnergy/electron_mass_c2;
  if (gamma > 5.) { return costeta; }
  G4double beta  = sqrt(gamma*gamma-1.)/gamma;
  G4double b     = 0.5*gamma*(gamma-1.)*(gamma-2);

  G4double rndm,term,greject,grejsup;
  if (gamma < 2.) { grejsup = gamma*gamma*(1.+b-beta*b); }
  else            { grejsup = gamma*gamma*(1.+b+beta*b); }

  do { rndm = 1.-2*G4UniformRand();
       costeta = (rndm+beta)/(rndm*beta+1.);
       term = 1.-beta*costeta;
       greject = (1.-costeta*costeta)*(1.+b*term)/(term*term);
  } while(greject < G4UniformRand()*grejsup);

  return costeta;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
