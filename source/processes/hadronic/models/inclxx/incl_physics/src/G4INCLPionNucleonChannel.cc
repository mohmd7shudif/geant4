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

#include "G4INCLPionNucleonChannel.hh"
#include "G4INCLKinematicsUtils.hh"
#include "G4INCLBinaryCollisionAvatar.hh"
#include "G4INCLRandom.hh"
#include "G4INCLGlobals.hh"
#include "G4INCLLogger.hh"

namespace G4INCL {

  PionNucleonChannel::PionNucleonChannel(Particle *p1, Particle *p2, Nucleus *nucleus, const G4bool localE)
    : theNucleus(nucleus), particle1(p1), particle2(p2), locE(localE)
  {

  }

  PionNucleonChannel::~PionNucleonChannel(){

  }

  FinalState* PionNucleonChannel::getFinalState() {
    FinalState *fs = new FinalState;

    Particle * nucleon;
    Particle * pion;
    if(particle1->isNucleon()) {
      nucleon = particle1;
      pion = particle2;
    } else {
      nucleon = particle2;
      pion = particle1;
    }

    ParticleType deltaType = DeltaZero;
    if(ParticleConfig::isPair(particle1, particle2, Proton, PiPlus)) {
      deltaType = DeltaPlusPlus;
    } else if(ParticleConfig::isPair(particle1, particle2, Neutron, PiPlus)) {
      deltaType = DeltaPlus;
    } else if(ParticleConfig::isPair(particle1, particle2, Proton, PiZero)) {
      deltaType = DeltaPlus;
    } else if(ParticleConfig::isPair(particle1, particle2, Neutron, PiZero)) {
      deltaType = DeltaZero;
    } else if(ParticleConfig::isPair(particle1, particle2, Proton, PiMinus)) {
      deltaType = DeltaZero;
    } else if(ParticleConfig::isPair(particle1, particle2, Neutron, PiMinus)) {
      deltaType = DeltaMinus;
    } else {
      ERROR("Unknown particle pair in Pi-N collision." << std::endl);
    }

    G4double deltaEnergy = nucleon->getEnergy() - nucleon->getPotentialEnergy()
      + pion->getEnergy() - pion->getPotentialEnergy();

    nucleon->setType(deltaType); // nucleon becomes the delta
    deltaEnergy += theNucleus->getPotential()->computePotentialEnergy(nucleon);
    nucleon->setEnergy(deltaEnergy); // set the energy of the delta

    ThreeVector deltaMomentum = nucleon->getMomentum() + pion->getMomentum();
    nucleon->setMomentum(deltaMomentum);

    const G4double deltaMass = std::sqrt(deltaEnergy*deltaEnergy - deltaMomentum.mag2());
    nucleon->setMass(deltaMass);
    theNucleus->updatePotentialEnergy(nucleon);

    fs->addModifiedParticle(nucleon); // nucleon became a delta
    fs->addDestroyedParticle(pion);  // pion was removed
    return fs;
  }

}
