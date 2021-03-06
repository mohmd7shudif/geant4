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
// $Id: G4NeutronHPLabAngularEnergy.hh,v 1.11 2007-06-06 12:45:13 ahoward Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
#ifndef G4NeutronHPLabAngularEnergy_h
#define G4NeutronHPLabAngularEnergy_h 1

#include "G4ios.hh"
#include <fstream>
#include "globals.hh"
#include "G4Neutron.hh"
#include "G4NeutronHPInterpolator.hh"
#include "G4NeutronHPVector.hh"
#include "G4VNeutronHPEnergyAngular.hh"
#include "G4ReactionProduct.hh"
#include "G4InterpolationManager.hh"

class G4NeutronHPLabAngularEnergy : public G4VNeutronHPEnergyAngular
{
  public:
  
  G4NeutronHPLabAngularEnergy()
  {
    theEnergies = 0;
    theData = 0;
    nCosTh = 0;
    theSecondManager = 0;
  }
  ~G4NeutronHPLabAngularEnergy()
  {
    if(theEnergies != 0) delete [] theEnergies;
    if(nCosTh != 0) delete [] nCosTh;
    if(theData != 0) 
    {
      for(G4int i=0; i<nEnergies; i++)
        delete [] theData[i];
      delete [] theData;
    }
    if(theSecondManager != 0) delete [] theSecondManager;
  }
  
  public:
  
  void Init(std::ifstream & aDataFile);
     G4ReactionProduct * Sample(G4double anEnergy, G4double massCode, G4double mass);
  G4double MeanEnergyOfThisInteraction()
  {
    return currentMeanEnergy;
  }
  
  
  private:
  
  // number of incoming neutron energies
  G4int nEnergies;
  // Interpol between neutron energies
  G4InterpolationManager theManager; 
  // Incoming neutron energies
  G4double * theEnergies; 
  // number of directioncosines; parallel to theEnergies
  G4int * nCosTh; 
  // knows the interpolation between these stores
  G4InterpolationManager * theSecondManager; 
  // vectors of secondary energy, haufigkeit; parallel to theEnergies
  G4NeutronHPVector ** theData; 

  // utility interpolator
  G4NeutronHPInterpolator theInt;
  
  // cashed value of mean secondary energy in this event.
  G4double currentMeanEnergy;
};
#endif
