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
// $Id: field02.cc,v 1.10 2010-05-12 16:36:58 allison Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// --------------------------------------------------------------
//      GEANT 4 - TestF02 
//
// --------------------------------------------------------------
// Comments
//     
//   
// --------------------------------------------------------------

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "Randomize.hh"

#include "F02DetectorConstruction.hh"
#include "F02ElectricFieldSetup.hh"
#include "F02PhysicsList.hh"
#include "F02PrimaryGeneratorAction.hh"
#include "F02RunAction.hh"
#include "F02EventAction.hh"
#include "F02SteppingAction.hh"
#include "F02SteppingVerbose.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

int main(int argc,char** argv) 
{

  //choose the Random engine

  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  
  //my Verbose output class

  G4VSteppingVerbose::SetInstance(new F02SteppingVerbose);
  
  // Construct the default run manager

  G4RunManager * runManager = new G4RunManager;

  // Construct the helper class to manage the electric field & 
  //  the parameters for the propagation of particles in it.

  F02ElectricFieldSetup* field = new F02ElectricFieldSetup() ;
    
  // Set mandatory initialization classes

  F02DetectorConstruction* detector;
  detector = new F02DetectorConstruction;
  runManager->SetUserInitialization(detector);
  runManager->SetUserInitialization(new F02PhysicsList(detector));
  
  // Set user action classes

  runManager->SetUserAction(new F02PrimaryGeneratorAction(detector));

  F02RunAction* runAction = new F02RunAction;

  runManager->SetUserAction(runAction);

  F02EventAction* eventAction = new F02EventAction(runAction);

  runManager->SetUserAction(eventAction);

  F02SteppingAction* steppingAction = new F02SteppingAction();
  runManager->SetUserAction(steppingAction);
  
  // Initialize G4 kernel, physics tables ...

  runManager->Initialize();
    
#ifdef G4VIS_USE

  // visualization manager

  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

#endif 
 
  // get the pointer to the User Interface manager 

  G4UImanager* UImanager = G4UImanager::GetUIpointer();  
 
  if (argc!=1)   // batch mode
    {
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UImanager->ApplyCommand(command+fileName);    
    }
  else
    {  // interactive mode : define UI session
#ifdef G4UI_USE
      G4UIExecutive* ui = new G4UIExecutive(argc, argv);
      ui->SessionStart();
      delete ui;
#endif
    }
    
  // job termination

#ifdef G4VIS_USE
  delete visManager;
#endif
  delete field;
  delete runManager;

  return 0;
}

