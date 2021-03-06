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
// $Id: DetectorConstruction.hh,v 1.1 2007-10-15 16:20:23 maire Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4Material;
class DetectorMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  
    DetectorConstruction();
   ~DetectorConstruction();

  public:
  
     void SetWallMaterial    (G4String);
       
     void SetWallThickness   (G4double);       
     void SetCavityThickness (G4double);            
      
     void SetWorldRadius     (G4double);
                 
     G4VPhysicalVolume* Construct();
     void               UpdateGeometry();
     
  public:
    
     G4double     GetWallThickness()   {return wallThickness;};
     G4Material*  GetWallMaterial()    {return wallMaterial;};
     G4VPhysicalVolume* GetWall()      {return pWall;};
                     
     G4double     GetCavityThickness() {return cavityThickness;};
     G4Material*  GetCavityMaterial()  {return cavityMaterial;}; 
     G4VPhysicalVolume* GetCavity()    {return pCavity;};
          
     G4double     GetWorldThickness()  {return worldThickness;};
     G4double     GetWorldRadius()     {return worldRadius;};        
     
     void         PrintParameters();
                       
  private:
   
     G4double            wallThickness;
     G4Material*         wallMaterial;
     G4VPhysicalVolume*  pWall;
     
     G4double            cavityThickness;
     G4Material*         cavityMaterial;
     G4VPhysicalVolume*  pCavity;
     
     G4double            worldThickness;
     G4double            worldRadius;

     DetectorMessenger* detectorMessenger;

  private:
    
     void               DefineMaterials();
     G4VPhysicalVolume* ConstructVolumes();     
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

