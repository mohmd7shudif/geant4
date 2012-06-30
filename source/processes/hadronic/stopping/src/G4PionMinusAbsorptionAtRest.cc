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
//   G4PionMinusAbsorptionAtRest physics process
//   Larry Felawka (TRIUMF), April 1998
//---------------------------------------------------------------------

#include "G4PionMinusAbsorptionAtRest.hh"
#include "G4DynamicParticle.hh"
#include "G4ParticleTypes.hh"
#include "Randomize.hh" 
#include "G4HadronicProcessStore.hh"
#include <string.h>
#include <cmath>
#include <stdio.h>
 
#define MAX_SECONDARIES 100

// constructor
 
G4PionMinusAbsorptionAtRest::G4PionMinusAbsorptionAtRest(const G4String& processName,
                                      G4ProcessType   aType ) :
  G4VRestProcess (processName, aType),       // initialization
  massPionMinus(G4PionMinus::PionMinus()->GetPDGMass()/GeV),
  pdefGamma(G4Gamma::Gamma()),
  pdefPionZero(G4PionZero::PionZero()),
  pdefPionMinus(G4PionMinus::PionMinus()),
  pdefProton(G4Proton::Proton()),
  pdefNeutron(G4Neutron::Neutron()),
  pdefDeuteron(G4Deuteron::Deuteron()),
  pdefTriton(G4Triton::Triton()),
  pdefAlpha(G4Alpha::Alpha())
{
  if (verboseLevel>0) {
    G4cout << GetProcessName() << " is created "<< G4endl;
  }
  SetProcessSubType(fHadronAtRest);
  pv   = new G4GHEKinematicsVector [MAX_SECONDARIES+1];
  eve  = new G4GHEKinematicsVector [MAX_SECONDARIES];
  gkin = new G4GHEKinematicsVector [MAX_SECONDARIES];

  G4HadronicProcessStore::Instance()->RegisterExtraProcess(this);
}
 
// destructor
 
G4PionMinusAbsorptionAtRest::~G4PionMinusAbsorptionAtRest()
{
  G4HadronicProcessStore::Instance()->DeRegisterExtraProcess(this);
  delete [] pv;
  delete [] eve;
  delete [] gkin;
}
 
void G4PionMinusAbsorptionAtRest::PreparePhysicsTable(const G4ParticleDefinition& p) 
{
  G4HadronicProcessStore::Instance()->RegisterParticleForExtraProcess(this, &p);
}

void G4PionMinusAbsorptionAtRest::BuildPhysicsTable(const G4ParticleDefinition& p) 
{
  G4HadronicProcessStore::Instance()->PrintInfo(&p);
}
 
// methods.............................................................................
 
G4bool G4PionMinusAbsorptionAtRest::IsApplicable(
				 const G4ParticleDefinition& particle
				 )
{
   return ( &particle == pdefPionMinus );

}
 
// Warning - this method may be optimized away if made "inline"
G4int G4PionMinusAbsorptionAtRest::GetNumberOfSecondaries()
{
  return ( ngkine );

}

// Warning - this method may be optimized away if made "inline"
G4GHEKinematicsVector* G4PionMinusAbsorptionAtRest::GetSecondaryKinematics()
{
  return ( &gkin[0] );

}

G4double G4PionMinusAbsorptionAtRest::AtRestGetPhysicalInteractionLength(
				   const G4Track& track,
				   G4ForceCondition* condition
				   )
{
  // beggining of tracking 
  ResetNumberOfInteractionLengthLeft();

  // condition is set to "Not Forced"
  *condition = NotForced;

  // get mean life time
  currentInteractionLength = GetMeanLifeTime(track, condition);

  if ((currentInteractionLength <0.0) || (verboseLevel>2)){
    G4cout << "G4PionMinusAbsorptionAtRestProcess::AtRestGetPhysicalInteractionLength ";
    G4cout << "[ " << GetProcessName() << "]" <<G4endl;
    track.GetDynamicParticle()->DumpInfo();
    G4cout << " in Material  " << track.GetMaterial()->GetName() <<G4endl;
    G4cout << "MeanLifeTime = " << currentInteractionLength/ns << "[ns]" <<G4endl;
  }

  return theNumberOfInteractionLengthLeft * currentInteractionLength;

}

G4VParticleChange* G4PionMinusAbsorptionAtRest::AtRestDoIt(
                                            const G4Track& track,
					    const G4Step& 
					    )
//
// Handles PionMinuss at rest; a PionMinus can either create secondaries or
// do nothing (in which case it should be sent back to decay-handling
// section
//
{

//   Initialize ParticleChange
//     all members of G4VParticleChange are set to equal to 
//     corresponding member in G4Track

  aParticleChange.Initialize(track);

//   Store some global quantities that depend on current material and particle

  globalTime = track.GetGlobalTime()/s;
  G4Material * aMaterial = track.GetMaterial();
  const G4int numberOfElements = aMaterial->GetNumberOfElements();
  const G4ElementVector* theElementVector = aMaterial->GetElementVector();

  const G4double* theAtomicNumberDensity = aMaterial->GetAtomicNumDensityVector();
  G4double normalization = 0;
  for ( G4int i1=0; i1 < numberOfElements; i1++ )
  {
    normalization += theAtomicNumberDensity[i1] ; // change when nucleon specific
                                                  // probabilities are included.
  }
  G4double runningSum= 0.;
  G4double random = G4UniformRand()*normalization;
  for ( G4int i2=0; i2 < numberOfElements; i2++ )
  {
    runningSum += theAtomicNumberDensity[i2]; // change when nucleon specific
                                              // probabilities are included.
    if (random<=runningSum)
    {
      targetCharge = G4double((*theElementVector)[i2]->GetZ());
      targetAtomicMass = (*theElementVector)[i2]->GetN();
    }
  }
  if (random>runningSum)
  {
    targetCharge = G4double((*theElementVector)[numberOfElements-1]->GetZ());
    targetAtomicMass = (*theElementVector)[numberOfElements-1]->GetN();

  }

  if (verboseLevel>1) {
    G4cout << "G4PionMinusAbsorptionAtRest::AtRestDoIt is invoked " <<G4endl;
    }

  G4ParticleMomentum momentum;
  G4float localtime;

  G4ThreeVector   position = track.GetPosition();

  GenerateSecondaries(); // Generate secondaries

  aParticleChange.SetNumberOfSecondaries( ngkine ); 

  for ( G4int isec = 0; isec < ngkine; isec++ ) {
    G4DynamicParticle* aNewParticle = new G4DynamicParticle;
    aNewParticle->SetDefinition( gkin[isec].GetParticleDef() );
    aNewParticle->SetMomentum( gkin[isec].GetMomentum() * GeV );

    localtime = globalTime + gkin[isec].GetTOF();

    G4Track* aNewTrack = new G4Track( aNewParticle, localtime*s, position );
		aNewTrack->SetTouchableHandle(track.GetTouchableHandle());
    aParticleChange.AddSecondary( aNewTrack );

  }

  aParticleChange.ProposeLocalEnergyDeposit( 0.0*GeV );

  aParticleChange.ProposeTrackStatus(fStopAndKill); // Kill the incident PionMinus

//   clear InteractionLengthLeft

  ResetNumberOfInteractionLengthLeft();

  return &aParticleChange;

}


void G4PionMinusAbsorptionAtRest::GenerateSecondaries()
{
  static G4int index;
  static G4int l;
  static G4int nopt;
  static G4int i;
  // DHW 15 May 2011: unused: static G4ParticleDefinition* jnd;

  for (i = 1; i <= MAX_SECONDARIES; ++i) {
    pv[i].SetZero();
  }

  ngkine = 0;            // number of generated secondary particles
  ntot = 0;
  result.SetZero();
  result.SetMass( massPionMinus );
  result.SetKineticEnergyAndUpdate( 0. );
  result.SetTOF( 0. );
  result.SetParticleDef( pdefPionMinus );

  PionMinusAbsorption(&nopt);

  // *** CHECK WHETHER THERE ARE NEW PARTICLES GENERATED ***
  if (ntot != 0 || result.GetParticleDef() != pdefPionMinus) {
    // *** CURRENT PARTICLE IS NOT THE SAME AS IN THE BEGINNING OR/AND ***
    // *** ONE OR MORE SECONDARIES HAVE BEEN GENERATED ***

    // --- INITIAL PARTICLE TYPE HAS BEEN CHANGED ==> PUT NEW TYPE ON ---
    // --- THE GEANT TEMPORARY STACK ---

    // --- PUT PARTICLE ON THE STACK ---
    gkin[0] = result;
    gkin[0].SetTOF( result.GetTOF() * 5e-11 );
    ngkine = 1;

    // --- ALL QUANTITIES ARE TAKEN FROM THE GHEISHA STACK WHERE THE ---
    // --- CONVENTION IS THE FOLLOWING ---

    // --- ONE OR MORE SECONDARIES HAVE BEEN GENERATED ---
    for (l = 1; l <= ntot; ++l) {
      index = l - 1;
      // DHW 15 May 2011: unused: jnd = eve[index].GetParticleDef();

      // --- ADD PARTICLE TO THE STACK IF STACK NOT YET FULL ---
      if (ngkine < MAX_SECONDARIES) {
	gkin[ngkine] = eve[index];
	gkin[ngkine].SetTOF( eve[index].GetTOF() * 5e-11 );
	++ngkine;
      }
    }
  }
  else {
    // --- NO SECONDARIES GENERATED AND PARTICLE IS STILL THE SAME ---
    // --- ==> COPY EVERYTHING BACK IN THE CURRENT GEANT STACK ---
    ngkine = 0;
    ntot = 0;
    globalTime += result.GetTOF() * G4float(5e-11);
  }

  // --- LIMIT THE VALUE OF NGKINE IN CASE OF OVERFLOW ---
  ngkine = G4int(std::min(ngkine,G4int(MAX_SECONDARIES)));

} // GenerateSecondaries


void G4PionMinusAbsorptionAtRest::PionMinusAbsorption(G4int *nopt)
{
  static G4int i;
  static G4int nt, nbl;
  static G4float ran, tex;
  static G4int isw;
  static G4float ran2, tof1, ekin;
  static G4float ekin1, ekin2, black;
  static G4float pnrat;
  static G4ParticleDefinition* ipa1;
  static G4ParticleDefinition* inve;

  // *** CHARGED PION ABSORPTION BY A NUCLEUS ***
  // *** NVE 04-MAR-1988 CERN GENEVA ***

  // ORIGIN : H.FESEFELDT (09-JULY-1987)

  // PANOFSKY RATIO (PI- P --> N PI0/PI- P --> N GAMMA) = 3/2
  // FOR CAPTURE ON PROTON (HYDROGEN),
  // STAR PRODUCTION FOR HEAVIER ELEMENTS

  pv[1].SetZero();
  pv[1].SetMass( massPionMinus );
  pv[1].SetKineticEnergyAndUpdate( 0. );
  pv[1].SetTOF( result.GetTOF() );
  pv[1].SetParticleDef( result.GetParticleDef() );
  if (targetAtomicMass <= G4float(1.5)) {
    ran = G4UniformRand();
    isw = 1;
    if (ran < G4float(.33)) {
      isw = 2;
    }
    *nopt = isw;
    ran = G4UniformRand();
    tof1 = std::log(ran) * G4float(-25.);
    tof1 *= G4float(20.);
    if (isw != 1) {
      pv[2].SetZero();
      pv[2].SetMass( 0. );
      pv[2].SetKineticEnergyAndUpdate( .02 );
      pv[2].SetTOF( result.GetTOF() + tof1 );
      pv[2].SetParticleDef( pdefGamma );
    }
    else {
      pv[2] = pv[1];
      pv[2].SetTOF( result.GetTOF() + tof1 );
      pv[2].SetParticleDef( pdefPionZero );
    }
    result = pv[2];
  }
  else {
    // **
    // ** STAR PRODUCTION FOR PION ABSORPTION IN HEAVY ELEMENTS
    // **
    evapEnergy1 = G4float(.0135);
    evapEnergy3 = G4float(.0058);
    nt = 1;
    tex = evapEnergy1;
    black = std::log(targetAtomicMass) * G4float(.5);
    Poisso(black, &nbl);
    if (nbl <= 0) {
      nbl = 1;
    }
    if (nt + nbl > (MAX_SECONDARIES - 2)) {
      nbl = (MAX_SECONDARIES - 2) - nt;
    }
    ekin = tex / nbl;
    ekin2 = G4float(0.);
    for (i = 1; i <= nbl; ++i) {
      if (nt == (MAX_SECONDARIES - 2)) {
	continue;
      }
      ran2 = G4UniformRand();
      ekin1 = -G4double(ekin) * std::log(ran2);
      ekin2 += ekin1;
      ipa1 = pdefNeutron;
      pnrat = G4float(1.) - targetCharge / targetAtomicMass;
      if (G4UniformRand() > pnrat) {
	ipa1 = pdefProton;
      }
      ++nt;
      pv[nt].SetZero();
      pv[nt].SetMass( ipa1->GetPDGMass()/GeV );
      pv[nt].SetKineticEnergyAndUpdate( ekin1 );
      pv[nt].SetTOF( 2. );
      pv[nt].SetParticleDef( ipa1 );
      if (ekin2 > tex) {
	break;
      }
    }
    tex = evapEnergy3;
    black = std::log(targetAtomicMass) * G4float(.5);
    Poisso(black, &nbl);
    if (nt + nbl > (MAX_SECONDARIES - 2)) {
      nbl = (MAX_SECONDARIES - 2) - nt;
    }
    if (nbl <= 0) {
      nbl = 1;
    }
    ekin = tex / nbl;
    ekin2 = G4float(0.);
    for (i = 1; i <= nbl; ++i) {
      if (nt == (MAX_SECONDARIES - 2)) {
	continue;
      }
      ran2 = G4UniformRand();
      ekin1 = -G4double(ekin) * std::log(ran2);
      ekin2 += ekin1;
      ++nt;
      ran = G4UniformRand();
      inve= pdefDeuteron;
      if (ran > G4float(.6)) {
	inve = pdefTriton;
      }
      if (ran > G4float(.9)) {
	inve = pdefAlpha;
      }
      pv[nt].SetZero();
      pv[nt].SetMass( inve->GetPDGMass()/GeV );
      pv[nt].SetKineticEnergyAndUpdate( ekin1 );
      pv[nt].SetTOF( 2. );
      pv[nt].SetParticleDef( inve );
      if (ekin2 > tex) {
	break;
      }
    }
    // **
    // ** STORE ON EVENT COMMON
    // **
    ran = G4UniformRand();
    tof1 = std::log(ran) * G4float(-25.);
    tof1 *= G4float(20.);
    for (i = 2; i <= nt; ++i) {
      pv[i].SetTOF( result.GetTOF() + tof1 );
    }
    result = pv[2];
    for (i = 3; i <= nt; ++i) {
      if (ntot >= MAX_SECONDARIES) {
	break;
      }
      eve[ntot++] = pv[i];
    }
  }

} // PionMinusAbsorption


void G4PionMinusAbsorptionAtRest::Poisso(G4float xav, G4int *iran)
{
  static G4int i;
  static G4float r, p1, p2, p3;
  static G4int mm;
  static G4float rr, ran, rrr, ran1;

  // *** GENERATION OF POISSON DISTRIBUTION ***
  // *** NVE 16-MAR-1988 CERN GENEVA ***
  // ORIGIN : H.FESEFELDT (27-OCT-1983)

  // --- USE NORMAL DISTRIBUTION FOR <X> > 9.9 ---
  if (xav > G4float(9.9)) {
    // ** NORMAL DISTRIBUTION WITH SIGMA**2 = <X>
    Normal(&ran1);
    ran1 = xav + ran1 * std::sqrt(xav);
    *iran = G4int(ran1);
    if (*iran < 0) {
      *iran = 0;
    }
  }
  else {
    mm = G4int(xav * G4float(5.));
    *iran = 0;
    if (mm > 0) {
      r = std::exp(-G4double(xav));
      ran1 = G4UniformRand();
      if (ran1 > r) {
	rr = r;
	for (i = 1; i <= mm; ++i) {
	  ++(*iran);
	  if (i <= 5) {
	    rrr = std::pow(xav, G4float(i)) / NFac(i);
	  }
	  // ** STIRLING' S FORMULA FOR LARGE NUMBERS
	  if (i > 5) {
	    rrr = std::exp(i * std::log(xav) -
		      (i + G4float(.5)) * std::log(i * G4float(1.)) +
		      i - G4float(.9189385));
	  }
	  rr += r * rrr;
	  if (ran1 <= rr) {
	    break;
	  }
	}
      }
    }
    else {
      // ** FOR VERY SMALL XAV TRY IRAN=1,2,3
      p1 = xav * std::exp(-G4double(xav));
      p2 = xav * p1 / G4float(2.);
      p3 = xav * p2 / G4float(3.);
      ran = G4UniformRand();
      if (ran >= p3) {
	if (ran >= p2) {
	  if (ran >= p1) {
	    *iran = 0;
	  }
	  else {
	    *iran = 1;
	  }
	}
	else {
	  *iran = 2;
	}
      }
      else {
	*iran = 3;
      }
    }
  }

} // Poisso


G4int G4PionMinusAbsorptionAtRest::NFac(G4int n)
{
  G4int ret_val;

  static G4int i, m;

  // *** NVE 16-MAR-1988 CERN GENEVA ***
  // ORIGIN : H.FESEFELDT (27-OCT-1983)

  ret_val = 1;
  m = n;
  if (m > 1) {
    if (m > 10) {
      m = 10;
    }
    for (i = 2; i <= m; ++i) {
      ret_val *= i;
    }
  }
  return ret_val;

} // NFac


void G4PionMinusAbsorptionAtRest::Normal(G4float *ran)
{
  static G4int i;

  // *** NVE 14-APR-1988 CERN GENEVA ***
  // ORIGIN : H.FESEFELDT (27-OCT-1983)

  *ran = G4float(-6.);
  for (i = 1; i <= 12; ++i) {
    *ran += G4UniformRand();
  }

} // Normal
