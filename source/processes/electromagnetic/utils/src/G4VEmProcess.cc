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
// $Id: G4VEmProcess.cc,v 1.88 2010-08-17 17:36:59 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
//
// File name:     G4VEmProcess
//
// Author:        Vladimir Ivanchenko on base of Laszlo Urban code
//
// Creation date: 01.10.2003
//
// Modifications:
// 30-06-04 make it to be pure discrete process (V.Ivanchenko)
// 30-09-08 optimise integral option (V.Ivanchenko)
// 08-11-04 Migration to new interface of Store/Retrieve tables (V.Ivanchenko)
// 11-03-05 Shift verbose level by 1, add applyCuts and killPrimary flags (VI)
// 14-03-05 Update logic PostStepDoIt (V.Ivanchenko)
// 08-04-05 Major optimisation of internal interfaces (V.Ivanchenko)
// 18-04-05 Use G4ParticleChangeForGamma (V.Ivanchenko)
// 25-07-05 Add protection: integral mode only for charged particles (VI)
// 04-09-05 default lambdaFactor 0.8 (V.Ivanchenko)
// 11-01-06 add A to parameters of ComputeCrossSectionPerAtom (VI)
// 12-09-06 add SetModel() (mma)
// 12-04-07 remove double call to Clear model manager (V.Ivanchenko)
// 27-10-07 Virtual functions moved to source (V.Ivanchenko)
// 24-06-09 Removed hidden bin in G4PhysicsVector (V.Ivanchenko)
// 17-02-10 Added pointer currentParticle (VI)
//
// Class Description:
//

// -------------------------------------------------------------------
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "G4VEmProcess.hh"
#include "G4ProcessManager.hh"
#include "G4LossTableManager.hh"
#include "G4LossTableBuilder.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4VEmModel.hh"
#include "G4DataVector.hh"
#include "G4PhysicsTable.hh"
#include "G4PhysicsLogVector.hh"
#include "G4VParticleChange.hh"
#include "G4ProductionCutsTable.hh"
#include "G4Region.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4PhysicsTableHelper.hh"
#include "G4EmBiasingManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4VEmProcess::G4VEmProcess(const G4String& name, G4ProcessType type):
  G4VDiscreteProcess(name, type),
  secondaryParticle(0),
  buildLambdaTable(true),
  theLambdaTable(0),
  theDensityFactor(0),
  theDensityIdx(0),
  integral(false),
  applyCuts(false),
  startFromNull(false),
  currentModel(0),
  particle(0),
  currentParticle(0),
  currentCouple(0)
{
  SetVerboseLevel(1);

  // Size of tables assuming spline
  minKinEnergy = 0.1*keV;
  maxKinEnergy = 10.0*TeV;
  nLambdaBins  = 77;

  // default lambda factor
  lambdaFactor  = 0.8;

  // default limit on polar angle
  polarAngleLimit = 0.0;
  biasFactor = 1.0;

  // particle types
  theGamma     = G4Gamma::Gamma();
  theElectron  = G4Electron::Electron();
  thePositron  = G4Positron::Positron();

  pParticleChange = &fParticleChange;
  secParticles.reserve(5);

  preStepLambda = 0.0;
  mfpKinEnergy  = DBL_MAX;

  modelManager = new G4EmModelManager();
  biasManager  = 0;
  biasFlag     = false; 
  weightFlag   = false;
  (G4LossTableManager::Instance())->Register(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4VEmProcess::~G4VEmProcess()
{
  if(1 < verboseLevel) {
    G4cout << "G4VEmProcess destruct " << GetProcessName() 
	   << "  " << this << "  " <<  theLambdaTable <<G4endl;
  }
  Clear();
  if(theLambdaTable) {
    theLambdaTable->clearAndDestroy();
    delete theLambdaTable;
  }
  delete modelManager;
  delete biasManager;
  (G4LossTableManager::Instance())->DeRegister(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4VEmProcess::Clear()
{
  currentCouple = 0;
  preStepLambda = 0.0;
  mfpKinEnergy  = DBL_MAX;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double G4VEmProcess::MinPrimaryEnergy(const G4ParticleDefinition*,
					const G4Material*)
{
  return 0.0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4VEmProcess::AddEmModel(G4int order, G4VEmModel* p, 
			      const G4Region* region)
{
  G4VEmFluctuationModel* fm = 0;
  modelManager->AddEmModel(order, p, fm, region);
  if(p) { p->SetParticleChange(pParticleChange); }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4VEmProcess::SetModel(G4VEmModel* p, G4int index)
{
  G4int n = emModels.size();
  if(index >= n) { for(G4int i=n; i<=index; ++i) {emModels.push_back(0);} }
  emModels[index] = p;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4VEmModel* G4VEmProcess::Model(G4int index)
{
  G4VEmModel* p = 0;
  if(index >= 0 && index <  G4int(emModels.size())) { p = emModels[index]; }
  return p;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4VEmProcess::UpdateEmModel(const G4String& nam, 
				 G4double emin, G4double emax)
{
  modelManager->UpdateEmModel(nam, emin, emax);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4VEmModel* G4VEmProcess::GetModelByIndex(G4int idx, G4bool ver)
{
  return modelManager->GetModel(idx, ver);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4VEmProcess::PreparePhysicsTable(const G4ParticleDefinition& part)
{
  if(!particle) { SetParticle(&part); }
  if(1 < verboseLevel) {
    G4cout << "G4VEmProcess::PreparePhysicsTable() for "
           << GetProcessName()
           << " and particle " << part.GetParticleName()
	   << " local particle " << particle->GetParticleName() 
           << G4endl;
  }

  G4LossTableManager* man = G4LossTableManager::Instance();
  G4LossTableBuilder* bld = man->GetTableBuilder();

  man->PreparePhysicsTable(&part, this);

  if(particle == &part) {
    Clear();
    InitialiseProcess(particle);

    const G4ProductionCutsTable* theCoupleTable=
      G4ProductionCutsTable::GetProductionCutsTable();
    size_t n = theCoupleTable->GetTableSize();

    theEnergyOfCrossSectionMax.resize(n, 0.0);
    theCrossSectionMax.resize(n, DBL_MAX);

    // initialisation of models
    G4int nmod = modelManager->NumberOfModels();
    for(G4int i=0; i<nmod; ++i) {
      G4VEmModel* mod = modelManager->GetModel(i);
      mod->SetPolarAngleLimit(polarAngleLimit);
      if(mod->HighEnergyLimit() > maxKinEnergy) {
	mod->SetHighEnergyLimit(maxKinEnergy);
      }
    }

    if(man->AtomDeexcitation()) { modelManager->SetFluoFlag(true); }
    theCuts = modelManager->Initialise(particle,secondaryParticle,
				       2.,verboseLevel);
    theCutsGamma    = theCoupleTable->GetEnergyCutsVector(idxG4GammaCut);
    theCutsElectron = theCoupleTable->GetEnergyCutsVector(idxG4ElectronCut);
    theCutsPositron = theCoupleTable->GetEnergyCutsVector(idxG4PositronCut);

    // prepare tables
    if(buildLambdaTable){
      theLambdaTable = G4PhysicsTableHelper::PreparePhysicsTable(theLambdaTable);
      bld->InitialiseBaseMaterials(theLambdaTable);
    }
    // forced biasing
    if(biasManager) { 
      biasManager->Initialise(part,GetProcessName(),verboseLevel); 
      biasFlag = false; 
    }
  }
  theDensityFactor = bld->GetDensityFactors();
  theDensityIdx = bld->GetCoupleIndexes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4VEmProcess::BuildPhysicsTable(const G4ParticleDefinition& part)
{
  G4String partname = part.GetParticleName();
  if(1 < verboseLevel) {
    G4cout << "G4VEmProcess::BuildPhysicsTable() for "
           << GetProcessName()
           << " and particle " << partname
	   << " buildLambdaTable= " << buildLambdaTable
           << G4endl;
  }

  (G4LossTableManager::Instance())->BuildPhysicsTable(particle);

  if(buildLambdaTable) {
    BuildLambdaTable();
    FindLambdaMax();
  }

  // reduce printout for nuclear stopping
  G4bool gproc = true;
  G4int st = GetProcessSubType();
  if((st == fCoulombScattering || st == fNuclearStopping) && 
     part.GetParticleType() == "nucleus" && 
     partname != "GenericIon" && partname != "alpha") { gproc = false; } 

  if(gproc && 0 < verboseLevel) { PrintInfoDefinition(); }

  if(1 < verboseLevel) {
    G4cout << "G4VEmProcess::BuildPhysicsTable() done for "
           << GetProcessName()
           << " and particle " << partname
           << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4VEmProcess::BuildLambdaTable()
{
  if(1 < verboseLevel) {
    G4cout << "G4EmProcess::BuildLambdaTable() for process "
           << GetProcessName() << " and particle "
           << particle->GetParticleName() << "  " << this
           << G4endl;
  }

  // Access to materials
  const G4ProductionCutsTable* theCoupleTable=
        G4ProductionCutsTable::GetProductionCutsTable();
  size_t numOfCouples = theCoupleTable->GetTableSize();

  G4LossTableBuilder* bld = (G4LossTableManager::Instance())->GetTableBuilder();
  G4bool splineFlag = (G4LossTableManager::Instance())->SplineFlag();

  G4PhysicsLogVector* aVector = 0;
  G4PhysicsLogVector* bVector = 0;

  G4double scale = 0.0;
  if(startFromNull) { scale = std::log(maxKinEnergy/minKinEnergy); }
    
  for(size_t i=0; i<numOfCouples; ++i) {

    if (bld->GetFlag(i)) {

      // create physics vector and fill it
      const G4MaterialCutsCouple* couple = 
	theCoupleTable->GetMaterialCutsCouple(i);
      delete (*theLambdaTable)[i];

      // if start from zero then change the scale
      if(startFromNull) {
	G4double emin = MinPrimaryEnergy(particle,couple->GetMaterial());
	if(0.0 >= emin) { emin = eV; }
	else if(maxKinEnergy <= emin) { emin = 0.5*maxKinEnergy; }
	G4int bin = 
	  G4int(nLambdaBins*std::log(maxKinEnergy/emin)/scale + 0.5);
	if(bin < 3) { bin = 3; }
	aVector = new G4PhysicsLogVector(emin, maxKinEnergy, bin);

	// start not from zero
      } else if(!bVector) {
	aVector = 
	  new G4PhysicsLogVector(minKinEnergy, maxKinEnergy, nLambdaBins);
        bVector = aVector;
      } else {
        aVector = new G4PhysicsLogVector(*bVector);
      }
      aVector->SetSpline(splineFlag);
      modelManager->FillLambdaVector(aVector, couple, startFromNull);
      if(splineFlag) { aVector->FillSecondDerivatives(); }
      G4PhysicsTableHelper::SetPhysicsVector(theLambdaTable, i, aVector);
    }
  }

  if(1 < verboseLevel) {
    G4cout << "Lambda table is built for "
           << particle->GetParticleName()
           << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4VEmProcess::PrintInfoDefinition()
{
  if(verboseLevel > 0) {
    G4cout << G4endl << GetProcessName() << ":   for  "
           << particle->GetParticleName();
    if(integral)  { G4cout << ", integral: 1 "; }
    if(applyCuts) { G4cout << ", applyCuts: 1 "; }
    G4cout << "    SubType= " << GetProcessSubType();;
    if(biasFactor != 1.0) { G4cout << "   BiasingFactor= " << biasFactor; }
    G4cout << G4endl;
    if(buildLambdaTable) {
      G4cout << "      Lambda tables from "
	     << G4BestUnit(minKinEnergy,"Energy") 
	     << " to "
	     << G4BestUnit(maxKinEnergy,"Energy")
	     << " in " << nLambdaBins << " bins, spline: " 
	     << (G4LossTableManager::Instance())->SplineFlag()
	     << G4endl;
    }
    PrintInfo();
    modelManager->DumpModelList(verboseLevel);
  }

  if(verboseLevel > 2 && buildLambdaTable) {
    G4cout << "      LambdaTable address= " << theLambdaTable << G4endl;
    if(theLambdaTable) { G4cout << (*theLambdaTable) << G4endl; }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double G4VEmProcess::PostStepGetPhysicalInteractionLength(
                             const G4Track& track,
                             G4double   previousStepSize,
                             G4ForceCondition* condition)
{
  // condition is set to "Not Forced"
  *condition = NotForced;
  G4double x = DBL_MAX;
  if(previousStepSize <= 0.0) { theNumberOfInteractionLengthLeft = -1.0; }
  InitialiseStep(track);
  if(!currentModel->IsActive(preStepKinEnergy)) { return x; }
 
  // forced biasing only for primary particles
  if(biasManager) {
    if(0 == track.GetParentID()) {
      if(0 == track.GetCurrentStepNumber()) {
        biasFlag = true; 
	biasManager->ResetForcedInteraction(); 
      }
      if(biasFlag && biasManager->ForcedInteractionRegion(currentCoupleIndex)) {
        return biasManager->GetStepLimit(currentCoupleIndex, previousStepSize);
      }
    }
  }

  // compute mean free path
  if(preStepKinEnergy < mfpKinEnergy) {
    if (integral) { ComputeIntegralLambda(preStepKinEnergy); }
    else { preStepLambda = GetCurrentLambda(preStepKinEnergy); }
    if(preStepLambda <= 0.0) { mfpKinEnergy = 0.0; }
  }

  // non-zero cross sect}ion
  if(preStepLambda > 0.0) { 
    if (theNumberOfInteractionLengthLeft < 0.0) {
      // beggining of tracking (or just after DoIt of this process)
      ResetNumberOfInteractionLengthLeft();
    } else if(currentInteractionLength < DBL_MAX) {
      // subtract NumberOfInteractionLengthLeft
      SubtractNumberOfInteractionLengthLeft(previousStepSize);
      if(theNumberOfInteractionLengthLeft < 0.)
	theNumberOfInteractionLengthLeft = perMillion;
    }

    // get mean free path and step limit
    currentInteractionLength = 1.0/preStepLambda;
    x = theNumberOfInteractionLengthLeft * currentInteractionLength;
#ifdef G4VERBOSE
    if (verboseLevel>2){
      G4cout << "G4VEmProcess::PostStepGetPhysicalInteractionLength ";
      G4cout << "[ " << GetProcessName() << "]" << G4endl; 
      G4cout << " for " << currentParticle->GetParticleName() 
	     << " in Material  " <<  currentMaterial->GetName()
	     << " Ekin(MeV)= " << preStepKinEnergy/MeV 
	     <<G4endl;
      G4cout << "MeanFreePath = " << currentInteractionLength/cm << "[cm]" 
	     << "InteractionLength= " << x/cm <<"[cm] " <<G4endl;
    }
#endif

    // zero cross section case
  } else {
    if(theNumberOfInteractionLengthLeft > DBL_MIN && 
       currentInteractionLength < DBL_MAX) {

      // subtract NumberOfInteractionLengthLeft
      SubtractNumberOfInteractionLengthLeft(previousStepSize);
      if(theNumberOfInteractionLengthLeft < 0.)
	theNumberOfInteractionLengthLeft = perMillion;
    }
    currentInteractionLength = DBL_MAX;
  }
  return x;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4VParticleChange* G4VEmProcess::PostStepDoIt(const G4Track& track,
                                              const G4Step&)
{
  fParticleChange.InitializeForPostStep(track);

  // Do not make anything if particle is stopped, the annihilation then
  // should be performed by the AtRestDoIt!
  if (track.GetTrackStatus() == fStopButAlive) { return &fParticleChange; }

  G4double finalT = track.GetKineticEnergy();

  // forced process - should happen only once per track
  if(biasFlag) {
    if(biasManager->ForcedInteractionRegion(currentCoupleIndex)) {
      biasFlag = false;
    }
  }

  // Integral approach
  if (integral) {
    G4double lx = GetLambda(finalT, currentCouple);
    if(preStepLambda<lx && 1 < verboseLevel) {
      G4cout << "WARING: for " << currentParticle->GetParticleName() 
             << " and " << GetProcessName()
             << " E(MeV)= " << finalT/MeV
             << " preLambda= " << preStepLambda << " < " 
	     << lx << " (postLambda) "
	     << G4endl;  
    }

    if(preStepLambda*G4UniformRand() > lx) {
      ClearNumberOfInteractionLengthLeft();
      return &fParticleChange;
    }
  }

  SelectModel(finalT, currentCoupleIndex);
  if(!currentModel->IsActive(finalT)) { return &fParticleChange; }

  // define new weight for primary and secondaries
  if(weightFlag) {
    G4double weight = fParticleChange.GetParentWeight()/biasFactor;
    fParticleChange.ProposeParentWeight(weight);
  }
  /*  
  if(0 < verboseLevel) {
    G4cout << "G4VEmProcess::PostStepDoIt: Sample secondary; E= "
           << finalT/MeV
           << " MeV; model= (" << currentModel->LowEnergyLimit()
           << ", " <<  currentModel->HighEnergyLimit() << ")"
           << G4endl;
  }
  */

  // sample secondaries
  secParticles.clear();
  currentModel->SampleSecondaries(&secParticles, 
				  currentCouple, 
				  track.GetDynamicParticle(),
				  (*theCuts)[currentCoupleIndex]);

  // save secondaries
  G4int num = secParticles.size();
  if(num > 0) {

    fParticleChange.SetNumberOfSecondaries(num);
    G4double edep = fParticleChange.GetLocalEnergyDeposit();
     
    for (G4int i=0; i<num; ++i) {
      G4DynamicParticle* dp = secParticles[i];
      const G4ParticleDefinition* p = dp->GetParticleDefinition();
      G4double e = dp->GetKineticEnergy();
      G4bool good = true;
      if(applyCuts) {
	if (p == theGamma) {
	  if (e < (*theCutsGamma)[currentCoupleIndex]) good = false;

	} else if (p == theElectron) {
	  if (e < (*theCutsElectron)[currentCoupleIndex]) good = false;

	} else if (p == thePositron) {
	  if (electron_mass_c2 < (*theCutsGamma)[currentCoupleIndex] &&
	      e < (*theCutsPositron)[currentCoupleIndex]) {
	    good = false;
	    e += 2.0*electron_mass_c2;
	  }
	}
        if(!good) {
	  delete dp;
	  edep += e;
	}
      }
      if (good) { fParticleChange.AddSecondary(dp); }
    } 
    fParticleChange.ProposeLocalEnergyDeposit(edep);
  }

  if(0.0 == fParticleChange.GetProposedKineticEnergy() &&
     fAlive == fParticleChange.GetTrackStatus()) {
    if(particle->GetProcessManager()->GetAtRestProcessVector()->size() > 0)
         { fParticleChange.ProposeTrackStatus(fStopButAlive); }
    else { fParticleChange.ProposeTrackStatus(fStopAndKill); }
  }

  ClearNumberOfInteractionLengthLeft();
  return &fParticleChange;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4bool G4VEmProcess::StorePhysicsTable(const G4ParticleDefinition* part,
			 	       const G4String& directory,
				             G4bool ascii)
{
  G4bool yes = true;

  if ( theLambdaTable && part == particle) {
    const G4String name = 
      GetPhysicsTableFileName(part,directory,"Lambda",ascii);
    yes = theLambdaTable->StorePhysicsTable(name,ascii);

    if ( yes ) {
      G4cout << "Physics tables are stored for " << particle->GetParticleName()
             << " and process " << GetProcessName()
	     << " in the directory <" << directory
	     << "> " << G4endl;
    } else {
      G4cout << "Fail to store Physics Tables for " 
	     << particle->GetParticleName()
             << " and process " << GetProcessName()
	     << " in the directory <" << directory
	     << "> " << G4endl;
    }
  }
  return yes;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

G4bool G4VEmProcess::RetrievePhysicsTable(const G4ParticleDefinition* part,
			  	          const G4String& directory,
					  G4bool ascii)
{
  if(1 < verboseLevel) {
    G4cout << "G4VEmProcess::RetrievePhysicsTable() for "
           << part->GetParticleName() << " and process "
	   << GetProcessName() << G4endl;
  }
  G4bool yes = true;

  if(!buildLambdaTable || particle != part) return yes;

  const G4String particleName = part->GetParticleName();
  G4String filename;

  filename = GetPhysicsTableFileName(part,directory,"Lambda",ascii);
  yes = G4PhysicsTableHelper::RetrievePhysicsTable(theLambdaTable,
						   filename,ascii);
  if ( yes ) {
    if (0 < verboseLevel) {
      G4cout << "Lambda table for " << particleName 
	     << " is Retrieved from <"
             << filename << ">"
             << G4endl;
    }
    if((G4LossTableManager::Instance())->SplineFlag()) {
      size_t n = theLambdaTable->length();
      for(size_t i=0; i<n; ++i) {
        if((* theLambdaTable)[i]) {
	  (* theLambdaTable)[i]->SetSpline(true);
	}
      }
    }
  } else {
    if (1 < verboseLevel) {
      G4cout << "Lambda table for " << particleName << " in file <"
             << filename << "> is not exist"
             << G4endl;
    }
  }

  return yes;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double 
G4VEmProcess::CrossSectionPerVolume(G4double kineticEnergy,
				    const G4MaterialCutsCouple* couple)
{
  // Cross section per atom is calculated
  DefineMaterial(couple);
  G4double cross = 0.0;
  if(theLambdaTable) {
    cross = (*theDensityFactor)[currentCoupleIndex]*
      (((*theLambdaTable)[basedCoupleIndex])->Value(kineticEnergy));
  } else {
    SelectModel(kineticEnergy, currentCoupleIndex);
    cross = currentModel->CrossSectionPerVolume(currentMaterial,
						currentParticle,kineticEnergy);
  }

  if(cross < 0.0) { cross = 0.0; }
  return cross;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double G4VEmProcess::GetMeanFreePath(const G4Track& track,
				       G4double,
				       G4ForceCondition* condition)
{
  *condition = NotForced;
  return G4VEmProcess::MeanFreePath(track);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double G4VEmProcess::MeanFreePath(const G4Track& track)
{
  DefineMaterial(track.GetMaterialCutsCouple());
  preStepLambda = GetCurrentLambda(track.GetKineticEnergy());
  G4double x = DBL_MAX;
  if(0.0 < preStepLambda) { x = 1.0/preStepLambda; }
  return x;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double 
G4VEmProcess::ComputeCrossSectionPerAtom(G4double kineticEnergy, 
					 G4double Z, G4double A, G4double cut)
{
  SelectModel(kineticEnergy, currentCoupleIndex);
  G4double x = 0.0;
  if(currentModel) {
    x = currentModel->ComputeCrossSectionPerAtom(currentParticle,kineticEnergy,
						 Z,A,cut);
  }
  return x;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4VEmProcess::FindLambdaMax()
{
  if(1 < verboseLevel) {
    G4cout << "### G4VEmProcess::FindLambdaMax: " 
	   << particle->GetParticleName() 
           << " and process " << GetProcessName() << G4endl; 
  }
  size_t n = theLambdaTable->length();
  G4PhysicsVector* pv;
  G4double e, s, emax, smax;

  size_t i;

  // first loop on existing vectors
  for (i=0; i<n; ++i) {
    pv = (*theLambdaTable)[i];
    if(pv) {
      size_t nb = pv->GetVectorLength();
      emax = DBL_MAX;
      smax = 0.0;
      if(nb > 0) {
	for (size_t j=0; j<nb; ++j) {
	  e = pv->Energy(j);
	  s = (*pv)(j);
	  if(s > smax) {
	    smax = s;
	    emax = e;
	  }
	}
      }
      theEnergyOfCrossSectionMax[i] = emax;
      theCrossSectionMax[i] = smax;
      if(1 < verboseLevel) {
	G4cout << "For " << particle->GetParticleName() 
	       << " Max CS at i= " << i << " emax(MeV)= " << emax/MeV
	       << " lambda= " << smax << G4endl;
      }
    }
  }
  // second loop using base materials
  for (size_t i=0; i<n; ++i) {
    pv = (*theLambdaTable)[i];
    if(!pv){
      G4int j = (*theDensityIdx)[i];
      theEnergyOfCrossSectionMax[i] = theEnergyOfCrossSectionMax[j];
      theCrossSectionMax[i] = (*theDensityFactor)[i]*theCrossSectionMax[j];
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4PhysicsVector* 
G4VEmProcess::LambdaPhysicsVector(const G4MaterialCutsCouple*)
{
  G4PhysicsVector* v = 
    new G4PhysicsLogVector(minKinEnergy, maxKinEnergy, nLambdaBins);
  v->SetSpline((G4LossTableManager::Instance())->SplineFlag());
  return v;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

const G4Element* G4VEmProcess::GetCurrentElement() const
{
  const G4Element* elm = 0;
  if(currentModel) {elm = currentModel->GetCurrentElement(); }
  return elm;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4VEmProcess::SetCrossSectionBiasingFactor(G4double f, G4bool flag)
{
  if(f > 0.0) { 
    biasFactor = f; 
    weightFlag = flag;
    if(1 < verboseLevel) {
      G4cout << "### SetCrossSectionBiasingFactor: for " 
	     << particle->GetParticleName() 
	     << " and process " << GetProcessName()
	     << " biasFactor= " << f << " weightFlag= " << flag 
	     << G4endl; 
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void 
G4VEmProcess::ActivateForcedInteraction(G4double length, const G4String& r,
					G4bool flag)
{
  if(!biasManager) { biasManager = new G4EmBiasingManager(); }
  if(1 < verboseLevel) {
    G4cout << "### ActivateForcedInteraction: for " 
	   << particle->GetParticleName() 
	   << " and process " << GetProcessName()
	   << " length(mm)= " << length/mm
	   << " in G4Region <" << r 
	   << "> weightFlag= " << flag 
	   << G4endl; 
  }
  weightFlag = flag;
  biasManager->ActivateForcedInteraction(length, r);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
