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

#include "ListProcsDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"

ListProcsDetectorConstruction::ListProcsDetectorConstruction()
	:  experimentalHall_log(0)
{
}

ListProcsDetectorConstruction::~ListProcsDetectorConstruction()
{
}

G4VPhysicalVolume* ListProcsDetectorConstruction::Construct()
{
	G4Material* Ar =
	    new G4Material("ArgonGas",
		/* atomic number */ 18.,
		/* atomic mass   */ 39.95 *  g/mole,
		/* density       */ 1.782 * mg/cm3);

	G4double expHall_x = 1.0 * m;
	G4double expHall_y = 1.0 * m;
	G4double expHall_z = 1.0 * m;

	G4Box* experimentalHall_box
	    = new G4Box("expHall_box", expHall_x, expHall_y, expHall_z);

	experimentalHall_log = new G4LogicalVolume(experimentalHall_box,
	    Ar, "expHall_log", 0, 0, 0);

	experimentalHall_phys = new G4PVPlacement(0, G4ThreeVector(),
	    experimentalHall_log, "expHall", 0, false, 0);
	
	return experimentalHall_phys;
}
