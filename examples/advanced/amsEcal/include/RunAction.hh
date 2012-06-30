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
// $Id: RunAction.hh,v 1.5 2009-08-29 08:48:30 maire Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef RunAction_h
#define RunAction_h 1

#include "DetectorConstruction.hh"

#include "G4UserRunAction.hh"
#include "globals.hh"

#include <vector>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PrimaryGeneratorAction;
class HistoManager;

class G4Run;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class RunAction : public G4UserRunAction
{
public:

  RunAction(DetectorConstruction*, PrimaryGeneratorAction*, HistoManager*);
  ~RunAction();

  void BeginOfRunAction(const G4Run*);
  void   EndOfRunAction(const G4Run*);

  void fillPerEvent_1(G4int,G4double,G4double);
  void fillPerEvent_2(G4int,G4double,G4double);  
  void fillPerEvent_3(G4double,G4double,G4double);
  void fillDetailedLeakage(G4int,G4double);
  void fillNbRadLen(G4double);
      
  void SetWriteFile(G4bool val)    {writeFile = val;};
  void CreateFilePixels();
      
private:
  
  DetectorConstruction*   detector;
  PrimaryGeneratorAction* primary;    
  HistoManager*           histoManager;
  
  std::vector<G4double> visibleEnergy, visibleEnergy2;
  std::vector<G4double>   totalEnergy,   totalEnergy2;
  
  std::vector<G4double> layerEvis, layerEvis2;
  std::vector<G4double> layerEtot, layerEtot2;
  
  G4int    nbEvents;  
  G4double calorEvis, calorEvis2;
  G4double calorEtot, calorEtot2;
  G4double Eleak,     Eleak2;
  G4double EdLeak[3];
  G4double nbRadLen, nbRadLen2;
      
  G4bool   writeFile;
    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

