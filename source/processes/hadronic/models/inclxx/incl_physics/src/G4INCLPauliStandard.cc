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

#include "G4INCLPauliStandard.hh"
#include "G4INCLPauliBlocking.hh"
#include "G4INCLGlobals.hh"
#include "G4INCLRandom.hh"

namespace G4INCL {

  PauliStandard::PauliStandard() {}
  PauliStandard::~PauliStandard() {}

  G4bool PauliStandard::isBlocked(ParticleList const pL, Nucleus const * const n) const {
    for(ParticleIter p = pL.begin(); p != pL.end(); ++p) {
      if( !(*p)->isNucleon() ) continue;
      if(getBlockingProbability(*p, n) > Random::shoot()) return true;
    }
    return false;
  }

  G4double PauliStandard::getBlockingProbability(Particle const * const particle, Nucleus const * const nucleus) const {
    const G4double rbl = 3.1848;
    const G4double pbl = 200.0;
    const G4double maxVolR = rbl;
    const G4double maxVolP = pbl;
    G4double vol = std::pow(4.*Math::pi/3.0, 2)
      * std::pow(maxVolR*maxVolP/(Math::twoPi*hc), 3);

    const G4double rdeq = nucleus->getDensity()->getMaximumRadius();
    const G4double rs = particle->getPosition().mag();

    if(rs - maxVolR > rdeq) {
      return 0.0;
    }

    if(rs + maxVolR > rdeq) {
      vol = vol * 0.5 * (rdeq - rs + maxVolR) / maxVolR;
    }

    // Get the list of particles that are currently inside the
    // nucleus.
    ParticleList particles = nucleus->getStore()->getParticles();

    G4int nl = 0;
    for(ParticleIter it = particles.begin(); it != particles.end(); ++it) {
      // Skip comparing with the same particle
      if( (*it)->getID() == particle->getID() ) continue;

      if((*it)->getType() == particle->getType()) {
        const ThreeVector dx2v = particle->getPosition() - (*it)->getPosition();
        const G4double dx2 = dx2v.mag2();
        if(dx2 > maxVolR * maxVolR) continue;

        const ThreeVector dp2v = particle->getMomentum() - (*it)->getMomentum();
        const G4double dp2 = dp2v.mag2();
        if(dp2 > maxVolP * maxVolP) continue;

        nl++;
      }
    }
    const G4double blockingProbability = ((G4double) nl) / vol / 2.0;

    if(blockingProbability > 1.0) return 1.0;
    else if(blockingProbability < 0.0) return 0.0;
    else return blockingProbability;
  }
}
