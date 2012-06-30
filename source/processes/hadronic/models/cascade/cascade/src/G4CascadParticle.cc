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
// $Id: G4CascadParticle.cc,v 1.18 2010-10-20 14:34:26 mkelsey Exp $
// Geant4 tag: $Name: not supported by cvs2svn $
//
// 20100112  M. Kelsey -- Remove G4CascadeMomentum, use G4LorentzVector directly
// 20100114  M. Kelsey -- Replace vector<G4Double> position with G4ThreeVector 
// 20101012  M. Kelsey -- Check for negative d2 in getPathToTheNextZone()
// 20110806  M. Kelsey -- Add fill() function to replicate ctor/op=() action
// 20110922  M. Kelsey -- Follow G4InuclParticle::print(ostream&) migration,
//		Add stream argument to print(), add operator<<().
// 20111017  M. Kelsey -- Add check for zero momentum in path calculation.

#include "G4CascadParticle.hh"
#include "G4ios.hh"
#include <cmath>


// Default constructor produces non-functional object

G4CascadParticle::G4CascadParticle()
  : verboseLevel(0), current_zone(-1), current_path(-1.), movingIn(false),
    reflectionCounter(0), reflected(false), generation(-1) {
  if (verboseLevel > 3) {
    G4cout << " >>> G4CascadParticle::G4CascadParticle" << G4endl;
  }
}

// Analogue to operator=() to support filling vectors w/o temporaries

void G4CascadParticle::fill(const G4InuclElementaryParticle& particle, 
			    const G4ThreeVector& pos, G4int izone,
			    G4double cpath, G4int gen) {
  if (verboseLevel > 3) G4cout << " >>> G4CascadParticle::fill" << G4endl;

  theParticle = particle;
  position = pos;
  current_zone = izone;
  current_path = cpath;
  movingIn = true;
  reflectionCounter = 0;
  reflected = false;
  generation = gen;
}


G4double G4CascadParticle::getPathToTheNextZone(G4double rz_in, 
						G4double rz_out) {
  if (verboseLevel > 3) {
    G4cout << " >>> G4CascadParticle::getPathToTheNextZone rz_in " << rz_in
	   << " rz_out " << rz_out << G4endl;
  }

  const G4LorentzVector& mom = getMomentum();

  G4double path = -1.0;
  G4double rp = mom.vect().dot(position);
  G4double rr = position.mag2();
  G4double pp = mom.vect().mag2();

  if (std::abs(pp) < 1e-9) {	// Cut-off for "at rest" is 1 eV momentum
    if (verboseLevel > 3) G4cout << " at rest; path length is zero" << G4endl;
    return 0.;
  }

  G4double ra = rr - rp * rp / pp;
  pp = std::sqrt(pp);
  G4double ds;
  G4double d2;

  if (verboseLevel > 3) {
    G4cout << " current_zone " << current_zone << " rr " << rr
	   << " rp " << rp << " pp " << pp << " ra " << ra << G4endl;
  }

  if (current_zone == 0 || rp > 0.0) {
    d2 = rz_out * rz_out - ra;
    if (d2 > 0.0) {
      ds = 1.0;
      movingIn = false;
    } else {
      d2 = rz_in * rz_in - ra;
      ds = -1.0;
      movingIn = true;
    }
  } else { 
    d2 = rz_in * rz_in - ra;
    if (d2 > 0.0) {
      ds = -1.0;
      movingIn = true;
    } else {
      d2 = rz_out * rz_out - ra;
      ds = 1.0;
      movingIn = false;
    }
  }

  if (verboseLevel > 3) G4cout << " ds " << ds << " d2 " << d2 << G4endl;

  if (d2 < 0.0 && d2 > -1e-6) d2 = 0.;		// Account for round-off

  if (d2 > 0.0) path = ds * std::sqrt(d2) - rp / pp;	// Avoid FPE failure

  return path;    
}

void G4CascadParticle::propagateAlongThePath(G4double path) {
  if (verboseLevel > 3) {
    G4cout << " >>> G4CascadParticle::propagateAlongThePath" << G4endl;
  }

  position += getMomentum().vect().unit()*path;
}


// Proper stream output (just calls print())

std::ostream& operator<<(std::ostream& os, const G4CascadParticle& part) {
  part.print(os);
  return os;
}

void G4CascadParticle::print(std::ostream& os) const {
  os << theParticle << G4endl
     << " zone " << current_zone << " current_path " << current_path
     << " reflectionCounter " << reflectionCounter << G4endl
     << " x " << position.x() << " y " << position.y()
     << " z " << position.z() << G4endl;
}

