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
// $Id: G4InuclElementaryParticle.hh,v 1.25 2010-09-16 05:21:00 mkelsey Exp $
// Geant4 tag: $Name: not supported by cvs2svn $
//
// 20100114  M. Kelsey -- Remove G4CascadeMomentum, use G4LorentzVector directly
// 20100409  M. Kelsey -- Drop unused string argument from ctors.
// 20100429  M. Kelsey -- Change "photon()" to "isPhoton()", use enum names
// 20100914  M. Kelsey -- Move printout to .cc file
// 20100915  M. Kelsey -- Add hyperon() identification function, ctor for
//		G4DynamicParticle
// 20110117  M. Kelsey -- Add antinucleon() and antibaryon() flag
// 20110127  M. Kelsey -- Drop generation.
// 20110214  M. Kelsey -- Replace integer "model" with enum
// 20110321  M. Kelsey -- Fix getStrangeness() to return int
// 20110721  M. Kelsey -- Add constructors to take G4ParticleDefinition as
//		input instead of type code, to allow pass-through of unusable
//		particles during rescattering.  Modify ctors to pass model to
//		base ctor.
// 20110801  M. Kelsey -- Add fill() functions to replicate ctors, allowing
//		reuse of objects as buffers; c.f. G4InuclNuclei.
// 20110922  M. Kelsey -- Add stream argument to printParticle() => print()

#ifndef G4INUCL_ELEMENTARY_PARTICLE_HH
#define G4INUCL_ELEMENTARY_PARTICLE_HH

#include "G4InuclParticle.hh"
#include "G4InuclParticleNames.hh"
#include "globals.hh"

class G4ParticleDefinition;


class G4InuclElementaryParticle : public G4InuclParticle {
public:
  G4InuclElementaryParticle() 
    : G4InuclParticle() {}

  G4InuclElementaryParticle(G4int type, Model model=DefaultModel) 
    : G4InuclParticle(makeDefinition(type), model) {}

  G4InuclElementaryParticle(const G4DynamicParticle& dynPart,
			    Model model=DefaultModel)
    : G4InuclParticle(dynPart, model) {}

  G4InuclElementaryParticle(const G4LorentzVector& mom,
			    G4int type, Model model=DefaultModel)
    : G4InuclParticle(makeDefinition(type), mom, model) {}

  G4InuclElementaryParticle(G4double ekin, G4int type,
			    Model model=DefaultModel) 
    : G4InuclParticle(makeDefinition(type), ekin, model) {}

  // WARNING:  This may create a particle without a valid type code!
  G4InuclElementaryParticle(const G4LorentzVector& mom,
			    G4ParticleDefinition* pd, Model model=DefaultModel)
    : G4InuclParticle(pd, mom, model) {}

  // Copy and assignment constructors for use with std::vector<>
  G4InuclElementaryParticle(const G4InuclElementaryParticle& right)
    : G4InuclParticle(right) {}

  G4InuclElementaryParticle& operator=(const G4InuclElementaryParticle& right);

  // Overwrite data structure (avoids creating/copying temporaries)
  void fill(G4int type, Model model=DefaultModel) { fill(0., type, model); }

  void fill(const G4LorentzVector& mom, G4int type, Model model=DefaultModel);

  void fill(G4double ekin, G4int type, Model model=DefaultModel);

  // WARNING:  This may create a particle without a valid type code!
  void fill(const G4LorentzVector& mom, G4ParticleDefinition* pd,
	    Model model=DefaultModel);

  // Assignment and accessor functions
  void setType(G4int ityp);
  G4int type() const { return type(getDefinition()); }

  static G4int type(const G4ParticleDefinition* pd);

  G4bool isPhoton() const { return (type() == G4InuclParticleNames::photon); }

  G4bool pion() const { return (type()==G4InuclParticleNames::pionPlus ||
				type()==G4InuclParticleNames::pionMinus ||
				type()==G4InuclParticleNames::pionZero); }

  G4bool nucleon() const { return (type()==G4InuclParticleNames::proton ||
				   type()==G4InuclParticleNames::neutron); }

  G4bool antinucleon() const {
    return (type()==G4InuclParticleNames::antiProton ||
	    type()==G4InuclParticleNames::antiNeutron); }

  G4int baryon() const { 		// Can use as a bool (!=0 ==> true)
    return getDefinition()->GetBaryonNumber();
  }

  G4bool antibaryon() const { return baryon() < 0; }

  G4bool hyperon() const { return (baryon() && getStrangeness()); }

  G4bool quasi_deutron() const { return (type() > 100); }

  G4int getStrangeness() const { return getStrangeness(type()); }

  G4bool valid() const { return type()>0; }

  virtual void print(std::ostream& os) const;

  static G4int getStrangeness(G4int type);
  static G4double getParticleMass(G4int type);

protected:
  // Convert internal type code to standard GEANT4 pointer
  static G4ParticleDefinition* makeDefinition(G4int ityp);
};        

#endif // G4INUCL_ELEMENTARY_PARTICLE_HH 
