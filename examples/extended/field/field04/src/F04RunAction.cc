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

#include "F04RunAction.hh"
#include "F04RunActionMessenger.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"

#include "Randomize.hh"

#include "F04GlobalField.hh"

#include <ctime>

F04RunAction::F04RunAction()
  : saveRndm(0), autoSeed(false)
{
  runMessenger = new F04RunActionMessenger(this);
}

F04RunAction::~F04RunAction()
{
  delete runMessenger;
}

void F04RunAction::BeginOfRunAction(const G4Run* aRun)
{  
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;

  G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  G4RunManager::GetRunManager()->SetRandomNumberStoreDir("random/");

  if (autoSeed) {
     // automatic (time-based) random seeds for each run
     G4cout << "*******************" << G4endl;
     G4cout << "*** AUTOSEED ON ***" << G4endl;
     G4cout << "*******************" << G4endl;
     long seeds[2];
     time_t systime = time(NULL);
     seeds[0] = (long) systime;
     seeds[1] = (long) (systime*G4UniformRand());
     CLHEP::HepRandom::setTheSeeds(seeds);
     CLHEP::HepRandom::showEngineStatus();
  } else {
     CLHEP::HepRandom::showEngineStatus();
  }
  
  if (saveRndm > 0) CLHEP::HepRandom::saveEngineStatus("BeginOfRun.rndm");

  FieldList* fields = F04GlobalField::getObject()->getFields();

  if (fields) {
     if (fields->size()>0) {
        FieldList::iterator i;
        for (i=fields->begin(); i!=fields->end(); ++i)(*i)->construct();
     }
  }
}

void F04RunAction::EndOfRunAction(const G4Run*)
{
  if (saveRndm == 1) {
     CLHEP::HepRandom::showEngineStatus();
     CLHEP::HepRandom::saveEngineStatus("endOfRun.rndm");
  }     
}
