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
// $Id: fanoCavity2.cc,v 1.3 2010-11-02 15:56:47 allison Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"

#include "RunAction.hh"
#include "EventAction.hh"
#include "TrackingAction.hh"
#include "SteppingAction.hh"
#include "SteppingVerbose.hh"
#include "HistoManager.hh"

#ifdef G4VIS_USE
 #include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
 #include "G4UIExecutive.hh"
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
int main(int argc,char** argv) {
 
  //choose the Random engine
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  
  //my Verbose output class
  G4VSteppingVerbose::SetInstance(new SteppingVerbose);
    
  //Construct the default run manager
  G4RunManager * runManager = new G4RunManager;

  //set mandatory initialization classes
  DetectorConstruction* det;
  PhysicsList* phys;
  PrimaryGeneratorAction* kin;
  HistoManager* histo   = new HistoManager();
     
  runManager->SetUserInitialization(det  = new DetectorConstruction);
  runManager->SetUserInitialization(phys = new PhysicsList(det));
  runManager->SetUserAction(kin = new PrimaryGeneratorAction(det,histo));
    
  //set user action classes  
  RunAction* run        = new RunAction(det,kin,histo);
  EventAction* event    = new EventAction(run,histo);
  TrackingAction* track = new TrackingAction(run,histo);      
  SteppingAction* step  = new SteppingAction(det,run,event,track,histo);
  
  runManager->SetUserAction(run); 
  runManager->SetUserAction(event);
  runManager->SetUserAction(track);      
  runManager->SetUserAction(step);

  //get the pointer to the User Interface manager 
  G4UImanager* UImanager = G4UImanager::GetUIpointer();  

  if (argc!=1)   // batch mode  
    {
     G4String command = "/control/execute ";
     G4String fileName = argv[1];
     UImanager->ApplyCommand(command+fileName);
    }
    
  else           // interactive mode :define visualization and UI terminal
    { 
#ifdef G4UI_USE
#ifdef G4VIS_USE
      G4VisManager* visManager = new G4VisExecutive;
      visManager->Initialize();
      UImanager->ApplyCommand("/control/execute vis.mac");     
#endif
      G4UIExecutive* ui = new G4UIExecutive(argc, argv);
      ui->SessionStart();
      delete ui;
#ifdef G4VIS_USE
      delete visManager;
#endif     
#endif     
    }


  //job termination
  //
  delete histo;
  delete runManager;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 
