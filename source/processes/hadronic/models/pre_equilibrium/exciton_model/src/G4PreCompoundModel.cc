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
// $Id: G4PreCompoundModel.cc,v 1.30 2010-11-24 11:55:40 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// by V. Lara
//
// Modified:
// 01.04.2008 J.M.Quesada Several changes. Soft cut-off switched off. 
// 01.05.2008 J.M.Quesada Protection against non-physical preeq. 
//                        transitional regime has been set
// 03.09.2008 J.M.Quesada for external choice of inverse cross section option
// 06.09.2008 J.M.Quesada Also external choices have been added for:
//                      - superimposed Coulomb barrier (useSICB=true) 
//                      - "never go back"  hipothesis (useNGB=true) 
//                      - soft cutoff from preeq. to equlibrium (useSCO=true)
//                      - CEM transition probabilities (useCEMtr=true)  
// 20.08.2010 V.Ivanchenko Cleanup of the code: 
//                      - integer Z and A;
//                      - emission and transition classes created at initialisation
//                      - options are set at initialisation
//                      - do not use copy-constructors for G4Fragment  

#include "G4PreCompoundModel.hh"
#include "G4PreCompoundEmission.hh"
#include "G4PreCompoundTransitions.hh"
#include "G4GNASHTransitions.hh"
#include "G4ParticleDefinition.hh"
#include "G4Proton.hh"
#include "G4Neutron.hh"

#include "G4NucleiProperties.hh"
#include "G4PreCompoundParameters.hh"
#include "Randomize.hh"
#include "G4DynamicParticle.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4LorentzVector.hh"

G4PreCompoundModel::G4PreCompoundModel(G4ExcitationHandler * const value) 
  : G4VPreCompoundModel(value), useHETCEmission(false), useGNASHTransition(false), 
    OPTxs(3), useSICB(false), useNGB(false), useSCO(false), useCEMtr(true),
    maxZ(3), maxA(5) 
				      //maxZ(9), maxA(17)
{
  theParameters = G4PreCompoundParameters::GetAddress();

  theEmission = new G4PreCompoundEmission();
  if(useHETCEmission) { theEmission->SetHETCModel(); }
  else { theEmission->SetDefaultModel(); }
  theEmission->SetOPTxs(OPTxs);
  theEmission->UseSICB(useSICB);

  if(useGNASHTransition) { theTransition = new G4GNASHTransitions; }
  else { theTransition = new G4PreCompoundTransitions(); }
  theTransition->UseNGB(useNGB);
  theTransition->UseCEMtr(useCEMtr);

  proton = G4Proton::Proton();
  neutron = G4Neutron::Neutron();
}

G4PreCompoundModel::~G4PreCompoundModel() 
{
  delete theEmission;
  delete theTransition;
}

/////////////////////////////////////////////////////////////////////////////////////////

G4HadFinalState* G4PreCompoundModel::ApplyYourself(const G4HadProjectile & thePrimary,
						   G4Nucleus & theNucleus)
{  
  const G4ParticleDefinition* primary = thePrimary.GetDefinition();
  if(primary != neutron && primary != proton) {
    std::ostringstream errOs;
    errOs << "BAD primary type in G4PreCompoundModel: " 
	  << primary->GetParticleName() <<G4endl;
    throw G4HadronicException(__FILE__, __LINE__, errOs.str());
  }

  G4int Zp = 0;
  G4int Ap = 1;
  if(primary == proton) { Zp = 1; }

  G4int A = theNucleus.GetA_asInt();
  G4int Z = theNucleus.GetZ_asInt();
   
  //G4cout << "### G4PreCompoundModel::ApplyYourself: A= " << A << " Z= " << Z
  //	 << " Ap= " << Ap << " Zp= " << Zp << G4endl; 
  // 4-Momentum
  G4LorentzVector p = thePrimary.Get4Momentum();
  G4double mass = G4NucleiProperties::GetNuclearMass(A, Z);
  p += G4LorentzVector(0.0,0.0,0.0,mass);
  //G4cout << "Primary 4-mom " << p << "  mass= " << mass << G4endl;

  // prepare fragment
  G4Fragment anInitialState(A + Ap, Z + Zp, p);

  // projectile and target nucleons
  // Add nucleon on which interaction happens
  //++Ap;
  //if(A*G4UniformRand() <= G4double(Z)) { Zp += 1; }
  anInitialState.SetNumberOfExcitedParticle(2, 1);
  anInitialState.SetNumberOfHoles(1,0);
  //  anInitialState.SetNumberOfExcitedParticle(Ap, Zp);
  // anInitialState.SetNumberOfHoles(Ap,Zp);

  anInitialState.SetCreationTime(thePrimary.GetGlobalTime());
  
  // call excitation handler
  G4ReactionProductVector * result = DeExcite(anInitialState);

  // fill particle change
  theResult.Clear();
  theResult.SetStatusChange(stopAndKill);
  for(G4ReactionProductVector::iterator i= result->begin(); i != result->end(); ++i)
    {
      G4DynamicParticle * aNew = 
	new G4DynamicParticle((*i)->GetDefinition(),
			      (*i)->GetTotalEnergy(),
			      (*i)->GetMomentum());
      delete (*i);
      theResult.AddSecondary(aNew);
    }
  delete result;
  
  //return the filled particle change
  return &theResult;
}

/////////////////////////////////////////////////////////////////////////////////////////

G4ReactionProductVector* G4PreCompoundModel::DeExcite(G4Fragment& aFragment)
{
  G4ReactionProductVector * Result = new G4ReactionProductVector;
  G4double Eex = aFragment.GetExcitationEnergy();
  G4int Z = aFragment.GetZ_asInt(); 
  G4int A = aFragment.GetA_asInt(); 

  //G4cout << "### G4PreCompoundModel::DeExcite" << G4endl;
  //G4cout << aFragment << G4endl;
 
  // Perform Equilibrium Emission 
  if ((Z < maxZ && A < maxA) || Eex < MeV /*|| Eex > 3.*MeV*A*/) {
    PerformEquilibriumEmission(aFragment, Result);
    return Result;
  }
  
  // main loop  
  for (;;) {
    
    //fragment++;
    //G4cout<<"-------------------------------------------------------------------"<<G4endl;
    //G4cout<<"Fragment number .. "<<fragment<<G4endl;
    
    // Initialize fragment according with the nucleus parameters
    //G4cout << "### Loop over fragment" << G4endl;
    //G4cout << aFragment << G4endl;

    theEmission->Initialize(aFragment);
    
    G4double g = (6.0/pi2)*aFragment.GetA_asInt()*theParameters->GetLevelDensity();
    
    G4int EquilibriumExcitonNumber = 
      static_cast<G4int>(std::sqrt(2.0*g*aFragment.GetExcitationEnergy())+ 0.5);
    //   
    //    G4cout<<"Neq="<<EquilibriumExcitonNumber<<G4endl;
    //
    // J. M. Quesada (Jan. 08)  equilibrium hole number could be used as preeq.
    // evap. delimiter (IAEA report)
    
    // Loop for transitions, it is performed while there are preequilibrium transitions.
    G4bool ThereIsTransition = false;
    
    //        G4cout<<"----------------------------------------"<<G4endl;
    //        G4double NP=aFragment.GetNumberOfParticles();
    //        G4double NH=aFragment.GetNumberOfHoles();
    //        G4double NE=aFragment.GetNumberOfExcitons();
    //        G4cout<<" Ex. Energy="<<aFragment.GetExcitationEnergy()<<G4endl;
    //        G4cout<<"N. excitons="<<NE<<"  N. Part="<<NP<<"N. Holes ="<<NH<<G4endl;
    //G4int transition=0;
    do {
      //transition++;
      //G4cout<<"transition number .."<<transition<<G4endl;
      //G4cout<<" n ="<<aFragment.GetNumberOfExcitons()<<G4endl;
      G4bool go_ahead = false;
      // soft cutoff criterium as an "ad-hoc" solution to force increase in  evaporation  
      G4int test = aFragment.GetNumberOfExcitons();
      if (test <= EquilibriumExcitonNumber) { go_ahead=true; }

      //J. M. Quesada (Apr. 08): soft-cutoff switched off by default
      if (useSCO && !go_ahead)
	{
	  G4double x = G4double(test)/G4double(EquilibriumExcitonNumber) - 1;
	  if( G4UniformRand() < 1.0 -  std::exp(-x*x/0.32) ) { go_ahead = true; }
	} 
        
      // JMQ: WARNING:  CalculateProbability MUST be called prior to Get methods !! 
      // (O values would be returned otherwise)
      G4double TotalTransitionProbability = 
	theTransition->CalculateProbability(aFragment);
      G4double P1 = theTransition->GetTransitionProb1();
      G4double P2 = theTransition->GetTransitionProb2();
      G4double P3 = theTransition->GetTransitionProb3();
      //G4cout<<"#0 P1="<<P1<<" P2="<<P2<<"  P3="<<P3<<G4endl;
      
      //J.M. Quesada (May 2008) Physical criterium (lamdas)  PREVAILS over 
      //                        approximation (critical exciton number)
      //V.Ivanchenko (May 2011) added check on number of nucleons
      //                        to send a fragment to FermiBreakUp
      if(!go_ahead || P1 <= P2+P3 || 
	 (aFragment.GetZ_asInt() < maxZ && aFragment.GetA_asInt() < maxA) )        
	{
	  //G4cout<<"#4 EquilibriumEmission"<<G4endl; 
	  PerformEquilibriumEmission(aFragment,Result);
	  return Result;
	}
      else 
	{
	  G4double TotalEmissionProbability = 
	    theEmission->GetTotalProbability(aFragment);
	  //
	  //G4cout<<"#1 TotalEmissionProbability="<<TotalEmissionProbability<<" Nex= " 
	  //	<<aFragment.GetNumberOfExcitons()<<G4endl;
	  //
	  // Check if number of excitons is greater than 0
	  // else perform equilibrium emission
	  if (aFragment.GetNumberOfExcitons() <= 0) 
	    {
	      PerformEquilibriumEmission(aFragment,Result);
	      return Result;
	    }
	    
	  //J.M.Quesada (May 08) this has already been done in order to decide  
	  //                     what to do (preeq-eq) 
	  // Sum of all probabilities
	  G4double TotalProbability = TotalEmissionProbability 
	    + TotalTransitionProbability;
            
	  // Select subprocess
	  if (TotalProbability*G4UniformRand() > TotalEmissionProbability) 
	    {
	      //G4cout<<"#2 Transition"<<G4endl; 
	      // It will be transition to state with a new number of excitons
	      ThereIsTransition = true;		
	      // Perform the transition
	      theTransition->PerformTransition(aFragment);
	    } 
	  else 
	    {
	      //G4cout<<"#3 Emission"<<G4endl; 
	      // It will be fragment emission
	      ThereIsTransition = false;
	      Result->push_back(theEmission->PerformEmission(aFragment));
	    }
	}
    } while (ThereIsTransition);   // end of do loop
  } // end of for (;;) loop
  return Result;
}

/////////////////////////////////////////////////////////////////////////////////////////
//       Initialisation
/////////////////////////////////////////////////////////////////////////////////////////

void G4PreCompoundModel::UseHETCEmission() 
{ 
  useHETCEmission = true; 
  theEmission->SetHETCModel();
}

void G4PreCompoundModel::UseDefaultEmission() 
{ 
  useHETCEmission = false; 
  theEmission->SetDefaultModel();
}

void G4PreCompoundModel::UseGNASHTransition() { 
  useGNASHTransition = true; 
  delete theTransition;
  theTransition = new G4GNASHTransitions;
  theTransition->UseNGB(useNGB);
  theTransition->UseCEMtr(useCEMtr);
}

void G4PreCompoundModel::UseDefaultTransition() { 
  useGNASHTransition = false; 
  delete theTransition;
  theTransition = new G4PreCompoundTransitions();
  theTransition->UseNGB(useNGB);
  theTransition->UseCEMtr(useCEMtr);
}

void G4PreCompoundModel::SetOPTxs(G4int opt) 
{ 
  OPTxs = opt; 
  theEmission->SetOPTxs(OPTxs);
}

void G4PreCompoundModel::UseSICB() 
{ 
  useSICB = true; 
  theEmission->UseSICB(useSICB);
}

void G4PreCompoundModel::UseNGB()  
{ 
  useNGB = true; 
}

void G4PreCompoundModel::UseSCO()  
{ 
  useSCO = true; 
}

void G4PreCompoundModel::UseCEMtr() 
{ 
  useCEMtr = true; 
}

/////////////////////////////////////////////////////////////////////////////////////////




