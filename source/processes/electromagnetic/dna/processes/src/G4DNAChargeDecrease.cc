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
// $Id: G4DNAChargeDecrease.cc,v 1.4 2010-03-18 16:36:48 sincerti Exp $
// GEANT4 tag $Name: not supported by cvs2svn $

#include "G4DNAChargeDecrease.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using namespace std;

G4DNAChargeDecrease::G4DNAChargeDecrease(const G4String& processName,
  G4ProcessType type):G4VEmProcess (processName, type),
    isInitialised(false)
{
  SetProcessSubType(56);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
G4DNAChargeDecrease::~G4DNAChargeDecrease()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
 
G4bool G4DNAChargeDecrease::IsApplicable(const G4ParticleDefinition& p)
{

  G4DNAGenericIonsManager *instance;
  instance = G4DNAGenericIonsManager::Instance();

  return 
    (
       &p == G4Proton::ProtonDefinition()
    || &p == instance->GetIon("alpha++")
    || &p == instance->GetIon("alpha+")
    );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4DNAChargeDecrease::InitialiseProcess(const G4ParticleDefinition* p)
{
  if(!isInitialised) 
  {
    isInitialised = true;
    SetBuildTableFlag(false);
    
    G4String name = p->GetParticleName();

    if( name == "proton" )
    {
      if(!Model()) SetModel(new G4DNADingfelderChargeDecreaseModel);
      Model()->SetLowEnergyLimit(100*eV);
      Model()->SetHighEnergyLimit(100*MeV);

      AddEmModel(1, Model());   
    }
    
    if( name == "alpha" || name == "alpha+" )
    {
      if(!Model()) SetModel(new G4DNADingfelderChargeDecreaseModel);
      Model()->SetLowEnergyLimit(1*keV);
      Model()->SetHighEnergyLimit(400*MeV);

      AddEmModel(1, Model());   
    }
    
  } 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4DNAChargeDecrease::PrintInfo()
{
  G4cout
    << " Total cross sections computed from " << Model()->GetName() << " model"
    << G4endl;
}         

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
