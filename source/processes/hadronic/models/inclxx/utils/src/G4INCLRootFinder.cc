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

/** \file G4INCLRootFinder.hh
 * \brief Static root-finder algorithm.
 *
 * Provides a stateless root-finder algorithm.
 *
 * Created on: 2nd March 2011
 *     Author: Davide Mancusi
 */

#include "G4INCLRootFinder.hh"
#include "G4INCLGlobals.hh"
#include "G4INCLLogger.hh"
#include <utility>
#include <cmath>

namespace G4INCL {

  std::pair<G4double,G4double> RootFinder::solution;

  const G4double RootFinder::toleranceY = 1.e-4;

  G4bool RootFinder::solve(RootFunctor const * const f, const G4double x0) {
    // If we already have the solution, do nothing
    const G4double y0 = (*f)(x0);
    if( std::abs(y0) < toleranceY ) {
      solution = std::make_pair(x0,y0);
      return true;
    }

    // Bracket the root and set the initial values
    std::pair<G4double,G4double> bracket = bracketRoot(f,x0);
    G4double x1 = bracket.first;
    G4double x2 = bracket.second;
    // If x1>x2, it means that we could not bracket the root. Return false.
    if(x1>x2) {
      WARN("Root-finding algorithm could not bracket the root." << std::endl);
      f->cleanUp(false);
      return false;
    }

    G4double y1 = (*f)(x1);
    G4double y2 = (*f)(x2);
    G4double x = x1;
    G4double y = y1;

    /* ********************************
     * Start of the false-position loop
     * ********************************/

    // Keep track of the last updated G4interval end (-1=left, 1=right)
    G4int lastUpdated = 0;

    for(G4int iterations=0; std::abs(y) > toleranceY; iterations++) {

      if(iterations > maxIterations) {
        WARN("Root-finding algorithm did not converge." << std::endl);
        f->cleanUp(false);
        return false;
      }

      // Estimate the root position by linear G4interpolation
      x = (y1*x2-y2*x1)/(y1-y2);

      // Update the value of the function
      y = (*f)(x);

      // Update the bracketing G4interval
      if(Math::sign(y) == Math::sign(y1)) {
        x1=x;
        y1=y;
        if(lastUpdated==-1) y2 *= 0.5;
        lastUpdated = -1;
      } else {
        x2=x;
        y2=y;
        if(lastUpdated==1) y1 *= 0.5;
        lastUpdated = 1;
      }
    }

    /* ******************************
     * End of the false-position loop
     * ******************************/

    solution = std::make_pair(x,y);
    f->cleanUp(true);
    return true;
  }

  std::pair<G4double,G4double> RootFinder::bracketRoot(RootFunctor const * const f, G4double x0) {
    G4double y0 = (*f)(x0);
    G4double x1 = x0;
    G4double y1 = (*f)(x1);

    const G4double scaleFactor = 1.5;
    const G4double scaleFactorMinus1 = 1./scaleFactor;

    G4double oldx0, oldx1, oldy1;
    G4int iterations=0;
    do {
      if(iterations > maxIterations) {
       DEBUG("Could not bracket the root." << std::endl);
        return std::make_pair((G4double) 1.,(G4double) -1.);
      }

      oldx0=x0;
      oldx1=x1;
      oldy1=y1;

      x0 *= scaleFactorMinus1;
      x1 *= scaleFactor;
      y0 = (*f)(x0);
      y1 = (*f)(x1);
      iterations++;
    } while(Math::sign(y0)==Math::sign(y1));

    if(Math::sign(y1)==Math::sign(oldy1))
      return std::make_pair(x0,oldx0);
    else
      return std::make_pair(oldx1,x1);
  }

}
