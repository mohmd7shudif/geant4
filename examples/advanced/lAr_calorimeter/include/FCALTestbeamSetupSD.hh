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
// $Id: FCALTestbeamSetupSD.hh,v 1.7 2006-06-29 16:02:32 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef FCALTestbeamSetupSD_h
#define FCALTestbeamSetupSD_h 1

#include "G4VSensitiveDetector.hh"
#include "globals.hh"
#include "FCALCalorHit.hh"
class FCALTestbeamSetup;
class G4HCofThisEvent;
class G4Step;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class FCALTestbeamSetupSD : public G4VSensitiveDetector
{
  public:
  
  //      FCALTestbeamSetupSD(G4String, FCALTestbeamSetup* );
      FCALTestbeamSetupSD(G4String); 
     ~FCALTestbeamSetupSD();

      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*,G4TouchableHistory*);
      void EndOfEvent(G4HCofThisEvent*);
      void clear();
      void DrawAll();
      void PrintAll();

  private:
  
  //  FCALCalorHitsCollection*  CalCollection;      
  //      FCALTestbeamSetup* Detector;
  //   G4int*                   HitID;
  
  G4int InitBeam;

public:
  
  G4double EBeamS1, EBeamS2, EBeamS3;
  G4double EHoleScint, EBeamHole;
  G4double EBeamDead;
  G4int TailCatcherID;
  G4double ETailVis[8], ETailDep[7];
  

};

#endif
