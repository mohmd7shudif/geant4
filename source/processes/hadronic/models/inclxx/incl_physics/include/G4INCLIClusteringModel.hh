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

#ifndef G4INCLIClusteringModel_hh
#define G4INCLIClusteringModel_hh 1

#include "G4INCLParticle.hh"
#include "G4INCLCluster.hh"
#include "G4INCLNucleus.hh"

namespace G4INCL {

  /**
   * An abstract G4interface to cluster formation model(s).
   */
  class IClusteringModel {
  public:
    IClusteringModel() {};
    virtual ~IClusteringModel() {};

    /**
     * Choose a cluster candidate to be produced. At this poG4int we
     * don't yet decide if it can pass through the Coulomb barrier or
     * not.
     */
    virtual Cluster* getCluster(Nucleus*, Particle*) = 0;

    /**
     * Determine whether cluster can escape or not.
     */
    virtual G4bool clusterCanEscape(Cluster const * const) = 0;

    static G4int maxClusterAlgorithmMass;
  };

}

#endif
