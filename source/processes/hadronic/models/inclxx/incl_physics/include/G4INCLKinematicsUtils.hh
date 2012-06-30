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

#ifndef KinematicsUtils_hh
#define KinematicsUtils_hh 1

#include "G4INCLThreeVector.hh"
#include "G4INCLParticle.hh"
#include "G4INCLNucleus.hh"

namespace G4INCL {

  class KinematicsUtils {
  public:
    static void transformToLocalEnergyFrame(Nucleus const * const n, Particle * const p);
    static G4double getLocalEnergy(Nucleus const * const n, Particle * const p);

    static ThreeVector makeBoostVector(Particle const * const p1, Particle const * const p2);
    static G4double totalEnergyInCM(Particle const * const p1, Particle const * const p2);
    static G4double squareTotalEnergyInCM(Particle const * const p1, Particle const * const p2);

    /** \brief gives the momentum in the CM frame of two particles.
     *
     * The formula is the following:
     * \f[ p_{CM}^2 = \frac{z^2 - m_1^2 m_2^2}{2 z + m_1^2 + m_2^2} \f]
     * where \f$z\f$ is the scalar product of the momentum four-vectors:
     * \f[ z = E_1 E_2 - \vec{p}_1\cdot\vec{p}_2 \f]
     *
     * \param p1 poG4inter to particle 1
     * \param p2 poG4inter to particle 2
     * \return the absolute value of the momentum of any of the two particles in
     * the CM frame, in MeV/c.
     */
    static G4double momentumInCM(Particle const * const p1, Particle const * const p2);

    static G4double momentumInCM(const G4double E, const G4double M1, const G4double M2);

    /** \brief gives the momentum in the lab frame of two particles.
     *
     * Assumes particle 1 carries all the momentum and particle 2 is at rest.
     *
     * The formula is the following:
     * \f[ p_{lab}^2 = \frac{s^2 - 2 s (m_1^2 + m_2^2) + {(m_1^2 - m_2^2)}^2}{4 m_2^2} \f]
     *
     * \param p1 poG4inter to particle 1
     * \param p2 poG4inter to particle 2
     * \return the absolute value of the momentum of particle 1 in the lab frame,
     * in MeV/c
     */
    static G4double momentumInLab(Particle const * const p1, Particle const * const p2);
    static G4double momentumInLab(const G4double s, const G4double m1, const G4double m2);
    static G4double sumTotalEnergies(const ParticleList &);
    static ThreeVector sumMomenta(const ParticleList &);
    static G4double energy(const ThreeVector &p, const G4double m);
    static G4double invariantMass(const G4double E, const ThreeVector & p);
  };
}

#endif
