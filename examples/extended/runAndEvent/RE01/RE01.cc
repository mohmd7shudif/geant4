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
// $Id: RE01.cc,v 1.4 2010-11-08 17:42:28 allison Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// --------------------------------------------------------------
//      GEANT4 - RE01 exsample code
//
// --------------------------------------------------------------
// Comments
//
// 
// --------------------------------------------------------------


#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "RE01DetectorConstruction.hh"
#include "RE01PhysicsList.hh"
#include "RE01PrimaryGeneratorAction.hh"
#include "RE01RunAction.hh"
#include "RE01EventAction.hh"
#include "RE01StackingAction.hh"
#include "RE01TrackingAction.hh"
#include "RE01SteppingAction.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

int main(int argc,char** argv)
{
  G4RunManager* runManager = new G4RunManager;

#ifdef G4VIS_USE
  // Visualization manager construction
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
#endif

  runManager->SetUserInitialization(new RE01DetectorConstruction);
  runManager->SetUserInitialization(new RE01PhysicsList);
  
  runManager->Initialize();

  runManager->SetUserAction(new RE01PrimaryGeneratorAction);
  runManager->SetUserAction(new RE01RunAction);  
  runManager->SetUserAction(new RE01EventAction);
  runManager->SetUserAction(new RE01StackingAction);
  runManager->SetUserAction(new RE01TrackingAction);
  runManager->SetUserAction(new RE01SteppingAction);
  
  if(argc==1)
  {
#ifdef G4UI_USE
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);
    ui->SessionStart();
    delete ui;
#endif
  }
  else
  {
    G4UImanager* UImanager = G4UImanager::GetUIpointer();  
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }

#ifdef G4VIS_USE
  delete visManager;
#endif

  delete runManager;

  return 0;
}

