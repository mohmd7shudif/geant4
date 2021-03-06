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
// $Id: DetectorConstruction.hh,v 1.1 2006-11-22 14:51:27 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
// --------------------------------------------------------------
//      GEANT 4 - DetectorConstruction class
// --------------------------------------------------------------
//
// Author: Witold POKORSKI (Witold.Pokorski@cern.ch)
// Date  : 2006-02-28
//
// --------------------------------------------------------------
#ifndef DetectorConstruction_H
#define DetectorConstruction_H 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"       

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:

  DetectorConstruction();
  ~DetectorConstruction();
  
  G4VPhysicalVolume* Construct();

private:

  void PrintParameters();
    // Print the name of absorber and active materials.

  G4Material* Iron;
  G4Material* Copper;
  G4Material* Tungsten;
  G4Material* Lead;
  G4Material* Uranium;
  G4Material* PbWO4;
  G4Material* Polystyrene;
  G4Material* LiquidArgon;
  G4Material* theAbsorberMaterial;
  
  G4LogicalVolume* experimentalHall_log;
  G4VPhysicalVolume* experimentalHall_phys;
    // World envelope. 
  
  G4LogicalVolume*  logicAbsorber;
  G4VPhysicalVolume* physiAbsorber;
    // Absorber layer of the "calorimeter".
  
};

#endif
