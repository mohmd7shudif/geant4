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
// $Id: G4ExitonConfiguration.hh,v 1.8 2010-12-15 07:39:50 gunter Exp $
//
// 20100909  Add function to reset values to zero
// 20100924  Migrate to integer A and Z
// 20110225  M. Kelsey -- Add equality operator (NOT sorting!)
// 20110922  M. Kelsey -- Replace print() with stream operator<<(), put
//		implementation into new .cc file.

#ifndef G4EXITON_CONFIGURATION_HH
#define G4EXITON_CONFIGURATION_HH

#include "globals.hh"
#include <iosfwd>

class G4ExitonConfiguration {
public:
  G4ExitonConfiguration() 
    : protonQuasiParticles(0), neutronQuasiParticles(0),
      protonHoles(0), neutronHoles(0) {}

  G4ExitonConfiguration(G4int qpp, G4int qnp, G4int qph, G4int qnh) 
    : protonQuasiParticles(qpp), neutronQuasiParticles(qnp), 
      protonHoles(qph), neutronHoles(qnh) {}

  void clear() {
    protonQuasiParticles = neutronQuasiParticles = 0;
    protonHoles = neutronHoles = 0;
  }

  bool operator==(const G4ExitonConfiguration& right) {
    return ( (&right == this) ||
	     (protonQuasiParticles == right.protonQuasiParticles &&
	      neutronQuasiParticles == right.neutronQuasiParticles &&
	      protonHoles == right.protonHoles &&
	      neutronHoles == right.neutronHoles) );
  }

  bool operator!=(const G4ExitonConfiguration& right) {
    return !operator==(right);
  }


  // Modify configuration

  void incrementQP(G4int ip) {
    if (ip == 1) protonQuasiParticles++;
    else if (ip == 2) neutronQuasiParticles++;
  }

  void incrementHoles(G4int ip) {
    if (ip == 1) protonHoles++;
    else if (ip == 2) neutronHoles++;
  }

  G4int protonQuasiParticles;
  G4int neutronQuasiParticles;
  G4int protonHoles;
  G4int neutronHoles;
};        

// Dump information to output

std::ostream& operator<<(std::ostream& os, const G4ExitonConfiguration& ex);

#endif // G4EXITON_CONFIGURATION_HH 
