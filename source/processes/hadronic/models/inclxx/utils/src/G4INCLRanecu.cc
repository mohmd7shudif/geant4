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
// INCL++ intra-nuclear cascade model
// Pekka Kaitaniemi, CEA and Helsinki Institute of Physics
// Davide Mancusi, CEA
// Alain Boudard, CEA
// Sylvie Leray, CEA
// Joseph Cugnon, University of Liege
//
// INCL++ revision: v5.0_rc3
//
#define INCLXX_IN_GEANT4_MODE 1

#include "globals.hh"

/*
 * G4INCLRanecu.cc
 *
 *  Created on: 7 juin 2009
 *      Author: Pekka Kaitaniemi
 */

#include "G4INCLRanecu.hh"

namespace G4INCL {

  Ranecu::Ranecu() {
    iseed1 = 666;
    iseed2 = 777;
  }

  Ranecu::Ranecu(const SeedVector &sv) {
    setSeeds(sv);
  }

  Ranecu::~Ranecu() {
    // TODO Auto-generated destructor stub
  }

  G4double Ranecu::flat()
  {
    // This is an adapted version of subroutine ranecu:
    // A. Padal, J. Sempau Computer Physics Cummunications 175 (2006) 440-450
    G4double uscale=1.0/2.147483563e9;

    long i1=iseed1/53668;
    iseed1=40014*(iseed1-i1*53668)-i1*12211;

    if(iseed1 < 0) iseed1 = iseed1 + 2147483563;

    long i2=iseed2/52774;
    iseed2=40692*(iseed2-i2*52774)-i2*3791;
    if(iseed2 < 0) iseed2=iseed2+2147483399;

    long iz=iseed1-iseed2;
    if(iz < 1) iz=iz+2147483562;

    return iz*uscale;
  }

}
