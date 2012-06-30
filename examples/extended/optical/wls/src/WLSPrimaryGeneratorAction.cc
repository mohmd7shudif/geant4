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
//

#include "G4ios.hh"
#include "G4Event.hh"

#include "G4GeneralParticleSource.hh"

#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"

#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include "G4PhysicsTable.hh"

#include "Randomize.hh"

#include "WLSPrimaryGeneratorAction.hh"

#include "WLSDetectorConstruction.hh"
#include "WLSPrimaryGeneratorMessenger.hh"

G4bool WLSPrimaryGeneratorAction::first = false;

WLSPrimaryGeneratorAction::
                         WLSPrimaryGeneratorAction(WLSDetectorConstruction* DC)
{
  detector = DC;
  theIntegralTable = NULL;

  particleGun = new G4GeneralParticleSource();
 
  gunMessenger = new WLSPrimaryGeneratorMessenger(this);

  // G4String particleName;
  // G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  timeConstant = 0.;

//  particleGun->SetParticleDefinition(particleTable->
//                               FindParticle(particleName="opticalphoton"));
}

WLSPrimaryGeneratorAction::~WLSPrimaryGeneratorAction()
{
  delete particleGun;
  delete gunMessenger;
  if (theIntegralTable) {
     theIntegralTable->clearAndDestroy();
     delete theIntegralTable;
  }
}

void WLSPrimaryGeneratorAction::SetDecayTimeConstant(G4double time)
{
  timeConstant = time;
}

void WLSPrimaryGeneratorAction::BuildEmissionSpectrum()
{
   if (theIntegralTable) return;

   const G4MaterialTable* theMaterialTable = G4Material::GetMaterialTable();

   G4int numOfMaterials = G4Material::GetNumberOfMaterials();

   if(!theIntegralTable)theIntegralTable = new G4PhysicsTable(numOfMaterials);

   for (G4int i=0 ; i < numOfMaterials; i++) {

       G4PhysicsOrderedFreeVector* aPhysicsOrderedFreeVector =
                                           new G4PhysicsOrderedFreeVector();

       G4Material* aMaterial = (*theMaterialTable)[i];

       G4MaterialPropertiesTable* aMaterialPropertiesTable =
                                    aMaterial->GetMaterialPropertiesTable();

       if (aMaterialPropertiesTable) {
          G4MaterialPropertyVector* theWLSVector =
                      aMaterialPropertiesTable->GetProperty("WLSCOMPONENT");

          if (theWLSVector) {
             G4double currentIN = (*theWLSVector)[0];
             if (currentIN >= 0.0) {
                G4double currentPM = theWLSVector->Energy(0);
                G4double currentCII = 0.0;
                aPhysicsOrderedFreeVector->
                                     InsertValues(currentPM , currentCII);
                G4double prevPM  = currentPM;
                G4double prevCII = currentCII;
                G4double prevIN  = currentIN;

                for (size_t i = 1;
                     i < theWLSVector->GetVectorLength();
                     i++)
                {
                  currentPM = theWLSVector->Energy(i);
                  currentIN = (*theWLSVector)[i];
                  currentCII = 0.5 * (prevIN + currentIN);
                  currentCII = prevCII + (currentPM - prevPM) * currentCII;
                  aPhysicsOrderedFreeVector->
                                     InsertValues(currentPM, currentCII);
                  prevPM  = currentPM;
                  prevCII = currentCII;
                  prevIN  = currentIN;
                }
             }
          }
       }
       theIntegralTable->insertAt(i,aPhysicsOrderedFreeVector);
   }
}

void WLSPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  if (!first) {
     first = true;
     BuildEmissionSpectrum();
  }


#ifdef use_sampledEnergy
  const G4MaterialTable* theMaterialTable = G4Material::GetMaterialTable();

  G4double sampledEnergy = 3*eV;

  for (size_t j=0 ; j<theMaterialTable->size() ; j++) {
      G4Material* fMaterial = (*theMaterialTable)[j];
      if (fMaterial->GetName() == "PMMA" ) {
         G4MaterialPropertiesTable* aMaterialPropertiesTable =
                                      fMaterial->GetMaterialPropertiesTable();
         const G4MaterialPropertyVector* WLSIntensity =
                   aMaterialPropertiesTable->GetProperty("WLSCOMPONENT");

         if (WLSIntensity) {
            G4int MaterialIndex = fMaterial->GetIndex();
            G4PhysicsOrderedFreeVector* WLSIntegral =
              (G4PhysicsOrderedFreeVector*)((*theIntegralTable)(MaterialIndex));

            G4double CIImax = WLSIntegral->GetMaxValue();
            G4double CIIvalue = G4UniformRand()*CIImax;

            sampledEnergy = WLSIntegral->GetEnergy(CIIvalue);
         }
      }
  }

  //particleGun->SetParticleEnergy(sampledEnergy);
#endif

  if(particleGun->GetParticleDefinition()->GetParticleName()=="opticalphoton"){
    SetOptPhotonPolar();
    SetOptPhotonTime();
  }

  particleGun->GeneratePrimaryVertex(anEvent);
}

void WLSPrimaryGeneratorAction::SetOptPhotonPolar()
{
  G4double angle = G4UniformRand() * 360.0*deg;
  SetOptPhotonPolar(angle);
}

void WLSPrimaryGeneratorAction::SetOptPhotonPolar(G4double angle)
{
  if (particleGun->GetParticleDefinition()->GetParticleName()!="opticalphoton")
  {
     G4cout << "-> warning from WLSPrimaryGeneratorAction::SetOptPhotonPolar()"
            << ":  the particleGun is not an opticalphoton" << G4endl;
     return;
  }

  G4ThreeVector normal (1., 0., 0.);
  G4ThreeVector kphoton = particleGun->GetParticleMomentumDirection();
  G4ThreeVector product = normal.cross(kphoton);
  G4double modul2       = product*product;

  G4ThreeVector e_perpend (0., 0., 1.);
  if (modul2 > 0.) e_perpend = (1./std::sqrt(modul2))*product;
  G4ThreeVector e_paralle    = e_perpend.cross(kphoton);

  G4ThreeVector polar = std::cos(angle)*e_paralle + std::sin(angle)*e_perpend;
  particleGun->SetParticlePolarization(polar);

}

void WLSPrimaryGeneratorAction::SetOptPhotonTime()
{
   G4double time = -std::log(G4UniformRand())*timeConstant;
   particleGun->SetParticleTime(time);
}
