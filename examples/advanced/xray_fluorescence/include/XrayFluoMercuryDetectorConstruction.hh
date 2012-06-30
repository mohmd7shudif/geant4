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
// $Id: XrayFluoMercuryDetectorConstruction.hh
// GEANT4 tag $Name:XrayFluo-V05-02-06 
//
// Author: Alfonso Mantero (Alfonso.Mantero@ge.infn.it)
//
// History:
// -----------
//
//  08 Sep 2003  Alfonso Mantero created
//
// -------------------------------------------------------------------

#ifndef XrayFluoMercuryDetectorConstruction_hh
#define XrayFluoMercuryDetectorConstruction_hh 1

#include "XrayFluoSiLiDetectorType.hh"
#include "XrayFluoHPGeDetectorType.hh"
#include "XrayFluoSD.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4RotationMatrix.hh"


class G4Box;
class G4Tubs;
class G4Sphere;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class XrayFluoMercuryDetectorMessenger;
class XrayFluoNistMaterials;

//class XrayFluoSD;
//class XrayFluoVDetectorType;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class XrayFluoMercuryDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  

  ~XrayFluoMercuryDetectorConstruction();
  
public:
  
  G4VPhysicalVolume* Construct();
  
  void UpdateGeometry();


  void SetMercuryMaterial(G4String newMaterial);

  void SetDetectorType(G4String type);

  static XrayFluoMercuryDetectorConstruction* GetInstance();

  inline void SetOribitHeight(G4double size)
  {distDe = size;};
  inline void SetLatitude(G4double lat)
  {ThetaHPGe = 180.* deg - lat;};

  void PrintApparateParameters(); 

  XrayFluoVDetectorType* GetDetectorType();


  G4double GetWorldSizeZ()           {return WorldSizeZ;}; 
  G4double GetWorldSizeXY()          {return WorldSizeXY;};
  
  G4double GetDeviceThickness()      {return DeviceThickness;}; 
  G4double GetDeviceSizeX()          {return DeviceSizeX;};
  G4double GetDeviceSizeY()          {return DeviceSizeY;};
  G4double GetPixelSizeXY()          {return PixelSizeXY;};
  G4double GetContactSizeXY()        {return ContactSizeXY;};

  G4int GetNbOfPixels()              {return NbOfPixels;}; //mandatory for XrayFluoSD 
  G4int GetNbOfPixelRows()           {return NbOfPixelRows;}; 
  G4int GetNbOfPixelColumns()        {return NbOfPixelColumns;}; 
  
  G4Material* GetOhmicPosMaterial()  {return OhmicPosMaterial;};
  G4double    GetOhmicPosThickness() {return OhmicPosThickness;};      
  
  G4Material* GetOhmicNegMaterial()  {return OhmicNegMaterial;};
  G4double    GetOhmicNegThickness() {return OhmicNegThickness;};      
  
  const G4VPhysicalVolume* GetphysiWorld() {return physiWorld;};  
  const G4VPhysicalVolume* GetHPGe()        {return physiHPGe;};
  const G4VPhysicalVolume* GetMercury()     {return physiMercury;};

  const G4VPhysicalVolume* GetphysiPixel()  {return physiPixel;};           
  const G4VPhysicalVolume* GetOhmicPos()    {return physiOhmicPos;};
  const G4VPhysicalVolume* GetOhmicNeg()    {return physiOhmicNeg;};

  const G4VPhysicalVolume* GetOptic()    {return physiOptic;};
  
private:
  
  XrayFluoMercuryDetectorConstruction();

  static XrayFluoMercuryDetectorConstruction* instance;

  XrayFluoVDetectorType* detectorType;

  G4bool mercuryGranularity;

  G4double           DeviceSizeX;
  G4double           DeviceSizeY;
  G4double           DeviceThickness;
  
  G4Box*             solidWorld;    //pointer to the solid    World 
  G4LogicalVolume*   logicWorld;    //pointer to the logical  World
  G4VPhysicalVolume* physiWorld;    //pointer to the physical World
  
  G4Box*             solidHPGe; //pointer to the solid    Sensor
  G4LogicalVolume*   logicHPGe; //pointer to the logical  Sensor
  G4VPhysicalVolume* physiHPGe; //pointer to the physical Sensor
 
  G4Box*             solidScreen; //pointer to the solid    Screen
  G4LogicalVolume*   logicScreen; //pointer to the logical  Screen
  G4VPhysicalVolume* physiScreen; //pointer to the physical Screen
 
  G4Sphere*             solidMercury;    //pointer to the solid    Mercury
  G4LogicalVolume*   logicMercury;    //pointer to the logical  Mercury
  G4VPhysicalVolume* physiMercury;    //pointer to the physical Mercury
  
  //  G4Tubs*             solidDia1; //pointer to the solid   Diaphragm
  //  G4LogicalVolume*   logicDia1; //pointer to the logical  Diaphragm
  //  G4VPhysicalVolume* physiDia1; //pointer to the physical Diaphragm 
  
  //  G4Tubs*             solidDia3; //pointer to the solid   Diaphragm
  //  G4LogicalVolume*   logicDia3; //pointer to the logical  Diaphragm
  //  G4VPhysicalVolume* physiDia3; //pointer to the physical Diaphragm  
  
  G4Box*             solidOhmicPos;
  G4LogicalVolume*   logicOhmicPos; 
  G4VPhysicalVolume* physiOhmicPos; 
  
  G4Box*             solidOhmicNeg;
  G4LogicalVolume*   logicOhmicNeg; 
  G4VPhysicalVolume* physiOhmicNeg;     
  
  G4Box*             solidPixel;   
  G4LogicalVolume*   logicPixel;  
  G4VPhysicalVolume* physiPixel;    
 
  G4Tubs*            solidOptic;
  G4LogicalVolume*   logicOptic;
  G4VPhysicalVolume* physiOptic;

  G4Sphere*          solidGrain;
  G4LogicalVolume*   logicGrain;
  G4VPhysicalVolume* physiGrain;

  //materials management
  XrayFluoNistMaterials* materials;

  G4Material*          screenMaterial;
  G4Material*        OhmicPosMaterial;
  G4Material*        OhmicNegMaterial; 
  G4Material*           pixelMaterial;
  G4Material*         mercuryMaterial;
  G4Material*           opticMaterial;
  //  G4Material*        Dia3Material;
  G4Material*         defaultMaterial;

  //apparate parameters

  G4double           OhmicPosThickness;
  G4double           OhmicNegThickness;
  
  G4double           opticDia;
  G4double           opticThickness;

  G4double           screenSizeXY;
  G4double           screenThickness;

  G4int              PixelCopyNb;
  G4int              grainCopyNb;
  G4int              NbOfPixels;
  G4int              NbOfPixelRows;
  G4int              NbOfPixelColumns;
  G4double           PixelThickness;
  G4double           PixelSizeXY;
  G4double           ContactSizeXY;

  G4double           opticAperture;

  G4double           mercuryDia;
  G4double           sunDia;

  G4double           mercurySunDistance;

  G4double           grainDia;

  G4double           ThetaHPGe;
  
  G4double           distDe;
  G4double           distScreen;
  G4double           distOptic;


  G4double           PhiHPGe;
  
  G4RotationMatrix   zRotPhiHPGe;

  G4double           WorldSizeXY;
  G4double           WorldSizeZ;
  
  
  XrayFluoMercuryDetectorMessenger* detectorMessenger; //pointer to the Messenger

  XrayFluoSD* HPGeSD;  //pointer to the sensitive detector


  
public:

  G4Material* GetMercuryMaterial() {return mercuryMaterial;}; 
  G4Material* GetPixelMaterial()   {return pixelMaterial;}; 
  
  G4double GetMercuryDia()         {return mercuryDia;};
  G4double GetSunDia()             {return sunDia;};

  //Inclinaton of the orbit respect Mercury respect the equator (latitude)

  G4double GetOrbitInclination()   {return 180 * deg - ThetaHPGe;}; 
  G4double GetOrbitDistance()      {return distDe;};
  G4double GetOpticAperture()      {return opticAperture;};  


  
private:
  
  void DefineDefaultMaterials();
  G4VPhysicalVolume* ConstructApparate();

  //calculates some quantities used to construct geometry
  void ComputeApparateParameters();

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline void XrayFluoMercuryDetectorConstruction::ComputeApparateParameters()
{     
  // Compute derived parameters of the apparate
  
  DeviceThickness = PixelThickness+OhmicNegThickness+OhmicPosThickness;

  ///G4cout << "DeviceThickness(cm): "<< DeviceThickness/cm << G4endl;

  DeviceSizeY =(NbOfPixelRows * std::max(ContactSizeXY,PixelSizeXY));
  DeviceSizeX =(NbOfPixelColumns * std::max(ContactSizeXY,PixelSizeXY));

  screenSizeXY = opticDia;

  G4cout << "DeviceSizeX(cm): "<< DeviceSizeX/cm <<G4endl;
  G4cout << "DeviceSizeY(cm): "<< DeviceSizeY/cm << G4endl;

  //*********************************************************************
  //**  Astronomical distances reduce by a factor 10^-7 due to G4 Bug  **
  //*********************************************************************

  WorldSizeZ = 2 * mercurySunDistance ; 
  WorldSizeXY = (2 * distDe) + 2000 * km ;
  //WorldSizeZ = WorldSizeXY;
}

#endif






