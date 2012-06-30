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

#ifndef G4INCLGlobals_hh
#define G4INCLGlobals_hh 1

#include <cmath>
#include "G4INCLParticleType.hh"

namespace G4INCL {
  class Particle;

  const G4double hc = 197.328;
  //  const G4double Pf = 1.36828*hc;
  const G4double Pf = 1.37*hc;
  const G4double PfSquared = Pf*Pf;

  namespace Math {
    const G4double pi = 3.14159265358979323846264338328;
    const G4double twoPi = 2.0 * pi;
    const G4double tenPi = 10.0 * pi;
    const G4double piOverTwo = 0.5 * pi;

    inline G4double toDegrees(G4double radians) {
      return radians * (180.0 / pi);
    }

    inline G4int heaviside(G4int n) {
      if(n < 0) return 0;
      else return 1;
    }

    inline G4double pow13(G4double x) {
      return std::pow(x, 1.0/3.0);
    }

    inline G4double pow23(G4double x) {
      return std::pow(x, 2.0/3.0);
    }

    /**
     * A simple sign function that allows us to port fortran code to c++ more easily.
     */
    template <typename T> inline G4int sign(T t) {
      return t > 0 ? 1: t < 0 ? -1 : 0;
    }
  }

  namespace ParticleConfig {
    G4bool isPair(Particle const * const p1, Particle const * const p2, ParticleType t1, ParticleType t2);
  }
}
#endif
