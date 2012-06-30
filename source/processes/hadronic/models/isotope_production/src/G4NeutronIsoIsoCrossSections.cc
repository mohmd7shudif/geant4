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
#include "G4NeutronIsoIsoCrossSections.hh"
#include "G4NeutronHPDataUsed.hh"
#include "G4NeutronInelasticCrossSection.hh"

G4NeutronIsoIsoCrossSections::
G4NeutronIsoIsoCrossSections()
: theCrossSection(), theNames()
{
  theProductionData = NULL;
  hasData = false;
  theNumberOfProducts = 0;
  theZ = 0;
  theA = 0;
}

G4NeutronIsoIsoCrossSections::
~G4NeutronIsoIsoCrossSections()
{
  for(G4int i=0; i<theNumberOfProducts; i++)
  {
    delete theProductionData[i];
  }
  delete [] theProductionData;
}

void G4NeutronIsoIsoCrossSections::
Init(G4int A, G4int Z, G4double frac)
{
  frac = frac/100.;
  // First transmution scattering cross-section
  // in our definition inelastic and fission.
  
  theZ = Z;
  theA = A;
  theNames.SetMaxOffSet(5);
  G4NeutronHPDataUsed dataUsed;
  G4String rest = "/CrossSection/";
  G4String base = getenv("G4NEUTRONHPDATA");
  G4String base1 = base + "/Inelastic/";
  G4bool hasInelasticData = false;
  dataUsed = theNames.GetName(A, Z, base1, rest, hasInelasticData);
  G4String aName = dataUsed.GetName();
  G4NeutronHPVector inelasticData;
  G4double dummy;
  G4int total;
  if(hasInelasticData)
  {
    std::ifstream aDataSet(aName, std::ios::in);
    aDataSet >> dummy >> dummy >> total;
    inelasticData.Init(aDataSet, total, eV);
  }
  base1 = base + "/Capture/";
  G4bool hasCaptureData = false;
  dataUsed = theNames.GetName(A, Z, base1, rest, hasCaptureData);
  aName = dataUsed.GetName();
  G4NeutronHPVector captureData;
  if(hasCaptureData)
  {
    std::ifstream aDataSet(aName, std::ios::in);
    aDataSet >> dummy >> dummy >> total;
    captureData.Init(aDataSet, total, eV);
  }
  base1 = base + "/Elastic/";
  G4bool hasElasticData = false;
  dataUsed = theNames.GetName(A, Z, base1, rest, hasElasticData);
  aName = dataUsed.GetName();
  G4NeutronHPVector elasticData;
  if(hasElasticData)
  {
    std::ifstream aDataSet(aName, std::ios::in);
    aDataSet >> dummy >> dummy >> total;
    elasticData.Init(aDataSet, total, eV);
  }
  base1 = base + "/Fission/";
  G4bool hasFissionData = false;
  if(Z>=91)
  {
    dataUsed = theNames.GetName(A, Z, base1, rest, hasFissionData);
    aName = dataUsed.GetName();
  }
  G4NeutronHPVector fissionData;
  if(hasFissionData)
  {
    std::ifstream aDataSet(aName, std::ios::in);
    aDataSet >> dummy >> dummy >> total;
    fissionData.Init(aDataSet, total, eV);
  }
  hasData = hasFissionData||hasInelasticData||hasElasticData||hasCaptureData;
  G4NeutronHPVector merged, merged1;
  if(hasData)
  {
    if(hasFissionData&&hasInelasticData) 
    {
      merged = fissionData + inelasticData;
    }
    else if(hasFissionData)
    {
      merged = fissionData;
    }
    else if(hasInelasticData)
    {
      merged = inelasticData;
    }
    
    if(hasElasticData&&hasCaptureData)
    {
      merged1=elasticData + captureData;
    }
    else if(hasCaptureData)
    {
      merged1 = captureData;
    }
    else if(hasElasticData)
    {
      merged1 = elasticData;
    }
    
    if((hasElasticData||hasCaptureData)&&(hasFissionData||hasInelasticData))
    {
      theCrossSection = merged + merged1;
    }
    else if(hasElasticData||hasCaptureData)
    {
      theCrossSection = merged1;
    }
    else if(hasFissionData||hasInelasticData)
    {
      theCrossSection = merged;
    }
    theCrossSection.Times(frac);
  }
//  theCrossSection.Dump();
  
  // now isotope-production cross-sections
  theNames.SetMaxOffSet(1);
  rest = "/CrossSection/";
  base1 = base + "/IsotopeProduction/";
  G4bool hasIsotopeProductionData;
  dataUsed = theNames.GetName(A, Z, base1, rest, hasIsotopeProductionData);
  aName = dataUsed.GetName();
  if(hasIsotopeProductionData)
  {
    std::ifstream aDataSet(aName, std::ios::in);
    aDataSet>>theNumberOfProducts;
    theProductionData = new G4IsoProdCrossSections * [theNumberOfProducts];
    for(G4int i=0; i<theNumberOfProducts; i++)
    {
      G4String aName;
      aDataSet >> aName;
      aDataSet >> dummy >> dummy;
      theProductionData[i] = new G4IsoProdCrossSections(aName);
      theProductionData[i]->Init(aDataSet);
    }
  }
  else
  {
    hasData = false;
  }
  G4NeutronInelasticCrossSection theParametrization;
  G4double lastEnergy = theCrossSection.GetX(theCrossSection.GetVectorLength()-1);
  G4double lastValue = theCrossSection.GetY(theCrossSection.GetVectorLength()-1);
  G4double norm = theParametrization.GetCrossSection(lastEnergy, Z, A);
  G4double increment = 1*MeV;
  while(lastEnergy+increment<101*MeV) 
  {
    G4double currentEnergy = lastEnergy+increment;
    G4double value = theParametrization.GetCrossSection(currentEnergy, Z, A)*(lastValue/norm);
    G4int position = theCrossSection.GetVectorLength();
    theCrossSection.SetData(position, currentEnergy, value);
    increment+=1*MeV;
  }
}

G4double G4NeutronIsoIsoCrossSections::
GetCrossSection(G4double anEnergy)
{
  G4double result;
  result = theCrossSection.GetY(anEnergy);
  return result;
}

G4String G4NeutronIsoIsoCrossSections::
GetProductIsotope(G4double anEnergy)
{
  G4String result = "UNCHANGED";
  
  G4double totalXSec = theCrossSection.GetY(anEnergy);
  if(totalXSec==0) return result;
  
  // now do the isotope changing reactions
  G4double * xSec = new G4double[theNumberOfProducts];
  G4double sum = 0;
  {
  for(G4int i=0; i<theNumberOfProducts; i++)
  {
    xSec[i] = theProductionData[i]->GetProductionCrossSection(anEnergy);
    sum += xSec[i];
  }
  }
  G4double isoChangeXsec = sum;
  G4double rand = G4UniformRand();
  if(rand > isoChangeXsec/totalXSec) 
  {
    delete [] xSec;
    return result;
  }
  G4double random = G4UniformRand();
  G4double running = 0;
  G4int index(0);
  {
  for(G4int i=0; i<theNumberOfProducts; i++)
  {
    running += xSec[i];
    index = i;
    if(random<=running/sum) break;
  }
  }
  delete [] xSec;
  result = theProductionData[index]->GetProductIsotope();
  
  return result;
}
