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
// $Id: G4EmStandardPhysics.cc,v 1.24 2010-10-10 15:18:34 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4EmStandardPhysics
//
// Author:      V.Ivanchenko 09.11.2005
//
// Modified:
// 05.12.2005 V.Ivanchenko add controlled verbosity
// 13.11.2006 V.Ivanchenko use G4hMultipleScattering
// 23.11.2006 V.Ivanchenko remove mscStepLimit option and improve cout
// 13.02.2007 V.Ivanchenko use G4hMultipleScattering for muons
// 13.02.2007 V.Ivanchenko set skin=0.0
// 21.04.2008 V.Ivanchenko add long-lived D and B mesons
//
//----------------------------------------------------------------------------
//

#include "G4EmStandardPhysics.hh"
#include "G4ParticleDefinition.hh"
#include "G4LossTableManager.hh"
#include "G4EmProcessOptions.hh"

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"

#include "G4eMultipleScattering.hh"
#include "G4MuMultipleScattering.hh"
#include "G4hMultipleScattering.hh"
#include "G4CoulombScattering.hh"
#include "G4WentzelVIModel.hh"

#include "G4MuBremsstrahlungModel.hh"
#include "G4MuPairProductionModel.hh"
#include "G4hBremsstrahlungModel.hh"
#include "G4hPairProductionModel.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"
#include "G4UAtomicDeexcitation.hh"

#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#include "G4hBremsstrahlung.hh"
#include "G4hPairProduction.hh"

#include "G4hIonisation.hh"
#include "G4ionIonisation.hh"
#include "G4alphaIonisation.hh"

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4KaonPlus.hh"
#include "G4KaonMinus.hh"
#include "G4Proton.hh"
#include "G4AntiProton.hh"
#include "G4Deuteron.hh"
#include "G4Triton.hh"
#include "G4He3.hh"
#include "G4Alpha.hh"
#include "G4GenericIon.hh"

#include "G4PhysicsListHelper.hh"
#include "G4BuilderType.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4EmStandardPhysics::G4EmStandardPhysics(G4int ver)
  : G4VPhysicsConstructor("G4EmStandard"), verbose(ver)
{
  G4LossTableManager::Instance();
  SetPhysicsType(bElectromagnetic);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4EmStandardPhysics::G4EmStandardPhysics(G4int ver, const G4String&)
  : G4VPhysicsConstructor("G4EmStandard"), verbose(ver)
{
  G4LossTableManager::Instance();
  SetPhysicsType(bElectromagnetic);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4EmStandardPhysics::~G4EmStandardPhysics()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4EmStandardPhysics::ConstructParticle()
{
// gamma
  G4Gamma::Gamma();

// leptons
  G4Electron::Electron();
  G4Positron::Positron();
  G4MuonPlus::MuonPlus();
  G4MuonMinus::MuonMinus();

// mesons
  G4PionPlus::PionPlusDefinition();
  G4PionMinus::PionMinusDefinition();
  G4KaonPlus::KaonPlusDefinition();
  G4KaonMinus::KaonMinusDefinition();

// barions
  G4Proton::Proton();
  G4AntiProton::AntiProton();

// ions
  G4Deuteron::Deuteron();
  G4Triton::Triton();
  G4He3::He3();
  G4Alpha::Alpha();
  G4GenericIon::GenericIonDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4EmStandardPhysics::ConstructProcess()
{
  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

  // Add standard EM Processes
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4String particleName = particle->GetParticleName();
    if(verbose > 1)
      G4cout << "### " << GetPhysicsName() << " instantiates for " 
	     << particleName << G4endl;

    if (particleName == "gamma") {

      ph->RegisterProcess(new G4PhotoElectricEffect(), particle);
      ph->RegisterProcess(new G4ComptonScattering(), particle);
      ph->RegisterProcess(new G4GammaConversion(), particle);

    } else if (particleName == "e-") {

      ph->RegisterProcess(new G4eMultipleScattering(), particle);
      ph->RegisterProcess(new G4eIonisation(), particle);
      ph->RegisterProcess(new G4eBremsstrahlung(), particle);

    } else if (particleName == "e+") {

      ph->RegisterProcess(new G4eMultipleScattering(), particle);
      ph->RegisterProcess(new G4eIonisation(), particle);
      ph->RegisterProcess(new G4eBremsstrahlung(), particle);
      ph->RegisterProcess(new G4eplusAnnihilation(), particle);

    } else if (particleName == "mu+" ||
               particleName == "mu-"    ) {

      G4MuMultipleScattering* msc = new G4MuMultipleScattering();
      msc->AddEmModel(0, new G4WentzelVIModel());

      G4MuBremsstrahlung* mub = new G4MuBremsstrahlung();
      G4MuPairProduction* mup = new G4MuPairProduction();

      ph->RegisterProcess(msc, particle);
      ph->RegisterProcess(new G4MuIonisation(), particle);
      ph->RegisterProcess(mub, particle);
      ph->RegisterProcess(mup, particle);
      ph->RegisterProcess(new G4CoulombScattering(), particle);

    } else if (particleName == "alpha" ||
               particleName == "He3") {

      ph->RegisterProcess(new G4hMultipleScattering(), particle);
      ph->RegisterProcess(new G4ionIonisation(), particle);

    } else if (particleName == "GenericIon") {

      ph->RegisterProcess(new G4hMultipleScattering(), particle);
      ph->RegisterProcess(new G4ionIonisation(), particle);

    } else if (particleName == "pi+" ||
               particleName == "pi-" ) {

      G4hBremsstrahlung* pib = new G4hBremsstrahlung();
      G4hPairProduction* pip = new G4hPairProduction();

      ph->RegisterProcess(new G4hMultipleScattering(), particle);
      ph->RegisterProcess(new G4hIonisation(), particle);
      ph->RegisterProcess(pib, particle);
      ph->RegisterProcess(pip, particle);

    } else if (particleName == "kaon+" ||
               particleName == "kaon-" ) {

      G4hBremsstrahlung* kb = new G4hBremsstrahlung();
      G4hPairProduction* kp = new G4hPairProduction();

      ph->RegisterProcess(new G4hMultipleScattering(), particle);
      ph->RegisterProcess(new G4hIonisation(), particle);
      ph->RegisterProcess(kb, particle);
      ph->RegisterProcess(kp, particle);

    } else if (particleName == "proton" ) {

      ph->RegisterProcess(new G4hMultipleScattering(), particle);
      ph->RegisterProcess(new G4hIonisation(), particle);
      ph->RegisterProcess(new G4hBremsstrahlung(), particle);
      ph->RegisterProcess(new G4hPairProduction(), particle);

    } else if (particleName == "B+" ||
	       particleName == "B-" ||
	       particleName == "D+" ||
	       particleName == "D-" ||
	       particleName == "Ds+" ||
	       particleName == "Ds-" ||
               particleName == "anti_He3" ||
               particleName == "anti_alpha" ||
               particleName == "anti_deuteron" ||
               particleName == "anti_lambda_c+" ||
               particleName == "anti_omega-" ||
               particleName == "anti_proton" ||
               particleName == "anti_sigma_c+" ||
               particleName == "anti_sigma_c++" ||
               particleName == "anti_sigma+" ||
               particleName == "anti_sigma-" ||
               particleName == "anti_triton" ||
               particleName == "anti_xi_c+" ||
               particleName == "anti_xi-" ||
               particleName == "deuteron" ||
	       particleName == "lambda_c+" ||
               particleName == "omega-" ||
               particleName == "sigma_c+" ||
               particleName == "sigma_c++" ||
               particleName == "sigma+" ||
               particleName == "sigma-" ||
               particleName == "tau+" ||
               particleName == "tau-" ||
               particleName == "triton" ||
               particleName == "xi_c+" ||
               particleName == "xi-" ) {

      ph->RegisterProcess(new G4hMultipleScattering(), particle);
      ph->RegisterProcess(new G4hIonisation(), particle);
    }
  }
  G4EmProcessOptions opt;
  opt.SetVerbose(verbose);
  opt.SetPolarAngleLimit(CLHEP::pi);

  // Deexcitation
  //
  G4VAtomDeexcitation* de = new G4UAtomicDeexcitation();
  G4LossTableManager::Instance()->SetAtomDeexcitation(de);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
