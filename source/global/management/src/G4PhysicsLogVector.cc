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
// $Id: G4PhysicsLogVector.cc,v 1.25 2010-05-28 05:13:43 kurasige Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// --------------------------------------------------------------
//      GEANT 4 class implementation file
//
//  G4PhysicsLogVector.cc
//
//  History:
//    02 Dec. 1995, G.Cosmo : Structure created based on object model
//    15 Feb. 1996, K.Amako : Implemented the 1st version
//    01 Jul. 1996, K.Amako : Hidden bin from the user introduced
//    26 Sep. 1996, K.Amako : Constructor with only 'bin size' added
//    11 Nov. 2000, H.Kurashige : use STL vector for dataVector and binVector
//    9  Mar. 2001, H.Kurashige : added PhysicsVector type and Retrieve
//    05 Sep. 2008, V.Ivanchenko : added protections for zero-length vector
//    19 Jun. 2009, V.Ivanchenko : removed hidden bin 
//
// --------------------------------------------------------------

#include "G4PhysicsLogVector.hh"

G4PhysicsLogVector::G4PhysicsLogVector()
  : G4PhysicsVector()
{ 
  type = T_G4PhysicsLogVector;
}

G4PhysicsLogVector::G4PhysicsLogVector(size_t theNbin)
  : G4PhysicsVector()
{
  type = T_G4PhysicsLogVector;

  numberOfNodes = theNbin + 1;
  dataVector.reserve(numberOfNodes);
  binVector.reserve(numberOfNodes);      

  for (size_t i=0; i<numberOfNodes; i++)
  {
     binVector.push_back(0.0);
     dataVector.push_back(0.0);
  }
}  

G4PhysicsLogVector::G4PhysicsLogVector(G4double theEmin, 
                                       G4double theEmax, size_t theNbin)
  : G4PhysicsVector()
{
  type = T_G4PhysicsLogVector;

  dBin     =  std::log10(theEmax/theEmin)/theNbin;
  baseBin  =  std::log10(theEmin)/dBin;

  numberOfNodes = theNbin + 1;
  dataVector.reserve(numberOfNodes);
  binVector.reserve(numberOfNodes);
  static const G4double g4log10 = std::log(10.); 

  binVector.push_back(theEmin);
  dataVector.push_back(0.0);

  for (size_t i=1; i<numberOfNodes-1; i++)
    {
      binVector.push_back(std::exp(g4log10*(baseBin+i)*dBin));
      dataVector.push_back(0.0);
    }
  binVector.push_back(theEmax);
  dataVector.push_back(0.0);

  edgeMin = binVector[0];
  edgeMax = binVector[numberOfNodes-1];
}  

G4PhysicsLogVector::~G4PhysicsLogVector()
{
}

G4bool G4PhysicsLogVector::Retrieve(std::ifstream& fIn, G4bool ascii)
{
  G4bool success = G4PhysicsVector::Retrieve(fIn, ascii);
  if (success)
  {
    G4double theEmin = binVector[0];
    dBin = std::log10(binVector[1]/theEmin);
    baseBin = std::log10(theEmin)/dBin;
  }
  return success;
}

void G4PhysicsLogVector::ScaleVector(G4double factorE, G4double factorV)
{
  G4PhysicsVector::ScaleVector(factorE, factorV);
  G4double theEmin = binVector[0];
  dBin = std::log10(binVector[1]/theEmin);
  baseBin = std::log10(theEmin)/dBin;
}

size_t G4PhysicsLogVector::FindBinLocation(G4double theEnergy) const
{
  // For G4PhysicsLogVector, FindBinLocation is implemented using
  // a simple arithmetic calculation.
  //
  // Because this is a virtual function, it is accessed through a
  // pointer to the G4PhyiscsVector object for most usages. In this
  // case, 'inline' will not be invoked. However, there is a possibility 
  // that the user access to the G4PhysicsLogVector object directly and 
  // not through pointers or references. In this case, the 'inline' will
  // be invoked. (See R.B.Murray, "C++ Strategies and Tactics", Chap.6.6)
  //G4cout << "G4PhysicsLogVector::FindBinLocation: e= " << theEnergy

  return size_t( std::log10(theEnergy)/dBin - baseBin );
}

