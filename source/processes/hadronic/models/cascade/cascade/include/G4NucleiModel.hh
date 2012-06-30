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
// $Id: G4NucleiModel.hh,v 1.34 2010/10/20 23:51:07 mkelsey Exp $
// GEANT4 tag: $Name:  $
//
// 20100319  M. Kelsey -- Remove "using" directory and unnecessary #includes,
//		move ctor to .cc file
// 20100407  M. Kelsey -- Create "partners thePartners" data member to act
//		as buffer between ::generateInteractionPartners() and 
//		::generateParticleFate(), and make "outgoing_cparticles" a
//		data member returned from the latter by const-ref.  Replace
//		return-by-value of initializeCascad() with an input buffer.
// 20100409  M. Kelsey -- Add function to sort list of partnerts by pathlen,
//		move non-inlinable code to .cc.
// 20100421  M. Kelsey -- Move getFermiKinetic() to .cc, no hardwired masses.
// 20100517  M. Kelsey -- Change cross-section tables to static arrays.  Move
//		absorptionCrossSection() from SpecialFunc.
// 20100520  M. Kelsey -- Add function to separate momentum from nucleon
// 20100617  M. Kelsey -- Add setVerboseLevel() function, add generateModel()
//		with particle input, and ctor with A/Z input.
// 20100715  M. Kelsey -- Add G4InuclNuclei object for use with balance checks
// 20100723  M. Kelsey -- Move G4CollisionOutput buffer, along with all
//		std::vector<> buffers, here for reuse
// 20100914  M. Kelsey -- Migrate to integer A and Z
// 20101004  M. Kelsey -- Rename and create functions used to generate model
// 20101019  M. Kelsey -- CoVerity report: dtor leak; move dtor to .cc file
// 20110223  M. Kelsey -- Add static parameters for radius and cross-section
//		scaling factors.
// 20110303  M. Kelsey -- Add accessors for model parameters and units
// 20110304  M. Kelsey -- Extend reset() to fill neutron and proton counts
// 20110324  D. Wright -- Add list of nucleon interaction points, and nucleon
//		effective radius, for trailing effect.
// 20110324  M. Kelsey -- Extend reset() to pass list of points; move
//		implementation to .cc file.
// 20110405  M. Kelsey -- Add "passTrailing()" to encapsulate trailing effect
// 20110808  M. Kelsey -- Pass buffer into generateParticleFate instead of
//		returning a vector to be copied.
// 20110823  M. Kelsey -- Remove local cross-section tables entirely

#ifndef G4NUCLEI_MODEL_HH
#define G4NUCLEI_MODEL_HH

#include "G4InuclElementaryParticle.hh"
#include "G4CascadParticle.hh"
#include "G4CollisionOutput.hh"
#include <algorithm>
#include <vector>

class G4InuclNuclei;
class G4ElementaryParticleCollider;

class G4NucleiModel {
public:
  G4NucleiModel();
  G4NucleiModel(G4int a, G4int z);
  explicit G4NucleiModel(G4InuclNuclei* nuclei);

  ~G4NucleiModel();

  void setVerboseLevel(G4int verbose) { verboseLevel = verbose; }

  void generateModel(G4InuclNuclei* nuclei);
  void generateModel(G4int a, G4int z);

  // Arguments here are used for rescattering (::Propagate)
  void reset(G4int nHitNeutrons=0, G4int nHitProtons=0,
	     const std::vector<G4ThreeVector>* hitPoints=0);

  void printModel() const; 

  G4double getDensity(G4int ip, G4int izone) const {
    return nucleon_densities[ip - 1][izone];
  }

  G4double getFermiMomentum(G4int ip, G4int izone) const {
    return fermi_momenta[ip - 1][izone];
  }

  G4double getFermiKinetic(G4int ip, G4int izone) const;

  G4double getPotential(G4int ip, G4int izone) const {
    G4int ip0 = ip < 3 ? ip - 1 : 2;
    if (ip > 10 && ip < 18) ip0 = 3;
    if (ip > 20) ip0 = 4;
    return izone < number_of_zones ? zone_potentials[ip0][izone] : 0.0;
  }

  // Factor to convert GEANT4 lengths to internal units
  G4double getRadiusUnits() const { return radiusUnits*fermi; }

  G4double getRadius() const { return nuclei_radius; }
  G4double getRadius(G4int izone) const {
    return ( (izone<0) ? 0 
	     : (izone<number_of_zones) ? zone_radii[izone] : nuclei_radius);
  }

  G4int getNumberOfZones() const { return number_of_zones; }
  G4int getZone(G4double r) const {
    for (G4int iz=0; iz<number_of_zones; iz++) if (r<zone_radii[iz]) return iz;
    return number_of_zones;
  }

  G4int getNumberOfNeutrons() const { return neutronNumberCurrent; }
  G4int getNumberOfProtons() const  { return protonNumberCurrent; }

  G4bool empty() const { 
    return neutronNumberCurrent < 1 && protonNumberCurrent < 1; 
  }

  G4bool stillInside(const G4CascadParticle& cparticle) {
    return cparticle.getCurrentZone() < number_of_zones;
  }


  G4CascadParticle initializeCascad(G4InuclElementaryParticle* particle);

  typedef std::pair<std::vector<G4CascadParticle>, std::vector<G4InuclElementaryParticle> > modelLists;

  void initializeCascad(G4InuclNuclei* bullet, G4InuclNuclei* target,
			modelLists& output);


  std::pair<G4int, G4int> getTypesOfNucleonsInvolved() const {
    return std::pair<G4int, G4int>(current_nucl1, current_nucl2);
  }

  void generateParticleFate(G4CascadParticle& cparticle,
			    G4ElementaryParticleCollider* theEPCollider,
			    std::vector<G4CascadParticle>& cascade); 

  G4bool worthToPropagate(const G4CascadParticle& cparticle) const; 
    
  G4InuclElementaryParticle generateNucleon(G4int type, G4int zone) const;

  G4LorentzVector generateNucleonMomentum(G4int type, G4int zone) const;

  G4double absorptionCrossSection(G4double e, G4int type) const;
  G4double totalCrossSection(G4double ke, G4int rtype) const;

private:
  G4int verboseLevel;

  G4bool passFermi(const std::vector<G4InuclElementaryParticle>& particles, 
		   G4int zone);

  G4bool passTrailing(const G4ThreeVector& hit_position);

  void boundaryTransition(G4CascadParticle& cparticle);

  G4InuclElementaryParticle generateQuasiDeutron(G4int type1, 
						 G4int type2,
						 G4int zone) const;

  typedef std::pair<G4InuclElementaryParticle, G4double> partner;

  std::vector<partner> thePartners;		// Buffer for output below
  void generateInteractionPartners(G4CascadParticle& cparticle);

  // Function for std::sort() to use in organizing partners by path length
  static G4bool sortPartners(const partner& p1, const partner& p2) {
    return (p2.second > p1.second);
  }

  // Functions used to generate model nuclear structure
  void fillBindingEnergies();

  void fillZoneRadii(G4double nuclearRadius);

  G4double fillZoneVolumes(G4double nuclearRadius);

  void fillPotentials(G4int type, G4double tot_vol);

  G4double zoneIntegralWoodsSaxon(G4double ur1, G4double ur2,
				  G4double nuclearRadius) const;

  G4double zoneIntegralGaussian(G4double ur1, G4double ur2,
				G4double nuclearRadius) const; 

  G4double getRatio(G4int ip) const;

  // Buffers for processing interactions on each cycle
  G4CollisionOutput EPCoutput;		// For N-body inelastic collisions

  std::vector<G4InuclElementaryParticle> qdeutrons;	// For h+(NN) trials
  std::vector<G4double> acsecs;
    
  std::vector<G4ThreeVector> coordinates;	// for initializeCascad()
  std::vector<G4LorentzVector> momentums;
  std::vector<G4InuclElementaryParticle> raw_particles;

  std::vector<G4ThreeVector> collisionPts;

  // Temporary buffers for computing nuclear model
  G4double ur[7];		// Number of skin depths for each zone
  G4double v[6];		// Density integrals by zone
  G4double v1[6];		// Pseudo-volume (delta r^3) by zone
  std::vector<G4double> rod;	// Nucleon density
  std::vector<G4double> pf;	// Fermi momentum
  std::vector<G4double> vz;	// Nucleo potential

  // Nuclear model configuration
  std::vector<std::vector<G4double> > nucleon_densities;
  std::vector<std::vector<G4double> > zone_potentials;
  std::vector<std::vector<G4double> > fermi_momenta;
  std::vector<G4double> zone_radii;
  std::vector<G4double> binding_energies;
  G4double nuclei_radius;
  G4int number_of_zones;

  G4int A;
  G4int Z;
  G4InuclNuclei* theNucleus;

  G4int neutronNumber;
  G4int protonNumber;

  G4int neutronNumberCurrent;
  G4int protonNumberCurrent;

  G4int current_nucl1;
  G4int current_nucl2;

  // Symbolic names for nuclear potentials
  enum PotentialType { WoodsSaxon=0, Gaussian=1 };

  // Parameters for nuclear structure
  static const G4double skinDepth;
  static const G4double radiusScale;	// Coefficients for two-parameter fit
  static const G4double radiusScale2;	//   R = 1.16*cbrt(A) - 1.3456/cbrt(A)
  static const G4double radiusForSmall; // Average radius of light A<5 nuclei
  static const G4double radScaleAlpha;	// Scaling factor R_alpha/R_small
  static const G4double fermiMomentum;
  static const G4double R_nucleon;
  static const G4double alfa3[3], alfa6[6];
  static const G4double pion_vp;
  static const G4double pion_vp_small;
  static const G4double kaon_vp;
  static const G4double hyperon_vp;

  // FIXME:  We should not be using this!
  static const G4double piTimes4thirds;
  static const G4double crossSectionUnits;
  static const G4double radiusUnits;
};        

#endif // G4NUCLEI_MODEL_HH 
