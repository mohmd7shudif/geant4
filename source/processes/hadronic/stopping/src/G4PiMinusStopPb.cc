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
//      File name:     G4PiMinusStopPb
//
//      Author:        Maria Grazia Pia (pia@genova.infn.it)
// 
//      Creation date: 8 May 1998
//
// -------------------------------------------------------------------

#include "G4ios.hh"

#include "G4PiMinusStopPb.hh"

#include <vector>

#include "globals.hh"
#include "Randomize.hh"
#include "G4Proton.hh"
#include "G4Neutron.hh"
#include "G4ParticleTypes.hh"
#include "G4ReactionKinematics.hh"
#include "G4DynamicParticleVector.hh"
#include "G4LorentzVector.hh"
#include "G4PiMinusStopMaterial.hh"
#include "G4DistributionGenerator.hh"

// np/pp production ratio
// Experimental values: 
// R(np/pp) (E. Gadioli et al., Phys Rev C36 (1987) 741) 
G4double G4PiMinusStopPb::npRatio = 6;
 
// Average numbers of final nucleons detected, for N-pair absorption
// (R. Madey  et al., Phys Rev C25 (1982) 3050
G4double G4PiMinusStopPb::nFinalNucleons = 1.67;

// Kinetic energy (MeV) distributions measured for coincident nucleon 
// emission
// (R. Madey  et al., Phys Rev C25 (1982) 3050

G4int G4PiMinusStopPb::eKinEntries = 10;

G4double G4PiMinusStopPb::eKinData[10] = { 0.24, 0.5, 1.03, 1.28,
                                     1.44, 
				     1.28, 1.03, 0.82, 0.50, 0.22};

G4double G4PiMinusStopPb::eKin[11] = { 5.2, 15., 27., 41.5,
                                  49.6,
				  57.7, 79.3, 94.4, 114., 125., 140.};


// Opening angle distributions measured for coincident nucleon emission
// (P.Heusi et al., Nucl. Phys. A407 (1983) 429

G4int G4PiMinusStopPb::angleEntries = 7;

G4double G4PiMinusStopPb::angleData[7] = 
{ 1.43, 1.67, 2.62, 4.29, 7.62, 11.90, 14.76 };

G4double G4PiMinusStopPb::angle[8] = { 1.308997, 1.570796, 1.832596, 2.094395, 
				  2.356194, 2.617994, 2.967060, 3.1415927 };



// Constructor

G4PiMinusStopPb::G4PiMinusStopPb()
  
{
  // Cluster size: nucleon pair, alpha, triton etc.
  // First implementation: interaction with nucleon pair only
  _clusterSize = 2;

  // R ratio
  theR = 1. / (1. + npRatio);

  _definitions = new std::vector<G4ParticleDefinition*>();
  _momenta = new std::vector<G4LorentzVector*>();

  std::vector<double> eKinVector;
  std::vector<double> eKinDataVector;
  int i;
  for (i=0; i<eKinEntries; i++)
    {
      eKinVector.push_back(eKin[i]);
      eKinDataVector.push_back(eKinData[i]);
    }
  eKinVector.push_back(eKin[eKinEntries]);
  _distributionE = new G4DistributionGenerator(eKinVector,eKinDataVector);

  std::vector<double> angleVector;
  std::vector<double> angleDataVector;
  for (i=0; i<angleEntries; i++)
    {
      angleVector.push_back(angle[i]);
      angleDataVector.push_back(angleData[i]);
    }
  angleVector.push_back(angle[angleEntries]);
  _distributionAngle = new G4DistributionGenerator(angleVector,angleDataVector);
}


// Destructor

G4PiMinusStopPb::~G4PiMinusStopPb()
{}

G4double G4PiMinusStopPb::FinalNucleons()
{
  return nFinalNucleons;
}

