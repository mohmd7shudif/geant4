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
// $Id: G4HEXiMinusInelastic.cc,v 1.17 2010-11-29 05:44:44 dennis Exp $
// GEANT4 tag $Name: not supported by cvs2svn $

#include "globals.hh"
#include "G4ios.hh"

// G4 Process: Gheisha High Energy Collision model.
// This includes the high energy cascading model, the two-body-resonance model
// and the low energy two-body model. Not included are the low energy stuff
// like nuclear reactions, nuclear fission without any cascading and all
// processes for particles at rest.  
// First work done by J.L.Chuma and F.W.Jones, TRIUMF, June 96.  
// H. Fesefeldt, RWTH-Aachen, 23-October-1996
 
#include "G4HEXiMinusInelastic.hh"


void G4HEXiMinusInelastic::ModelDescription(std::ostream& outFile) const
{
  outFile << "G4HEXiMinusInelastic is one of the High Energy\n"
          << "Parameterized (HEP) models used to implement inelastic\n"
          << "Xi- scattering from nuclei.  It is a re-engineered\n"
          << "version of the GHEISHA code of H. Fesefeldt.  It divides the\n"
          << "initial collision products into backward- and forward-going\n"
          << "clusters which are then decayed into final state hadrons.\n"
          << "The model does not conserve energy on an event-by-event\n"
          << "basis.  It may be applied to Xi- with initial energies\n"
          << "above 20 GeV.\n";
}


G4HadFinalState*
G4HEXiMinusInelastic::ApplyYourself(const G4HadProjectile& aTrack,
                                    G4Nucleus& targetNucleus)
{
  G4HEVector* pv = new G4HEVector[MAXPART];
  const G4HadProjectile* aParticle = &aTrack;
  const G4double A = targetNucleus.GetA_asInt();
  const G4double Z = targetNucleus.GetZ_asInt();
  G4HEVector incidentParticle(aParticle);
     
  G4double atomicNumber = Z;
  G4double atomicWeight = A;

  G4int incidentCode = incidentParticle.getCode();
  G4double incidentMass = incidentParticle.getMass();
  G4double incidentTotalEnergy = incidentParticle.getEnergy();

  // G4double incidentTotalMomentum = incidentParticle.getTotalMomentum();
  // DHW 19 May 2011: variable set but not used

  G4double incidentKineticEnergy = incidentTotalEnergy - incidentMass;

  if (incidentKineticEnergy < 1.)
    G4cout << "GHEXiMinusInelastic: incident energy < 1 GeV" << G4endl;

  if (verboseLevel > 1) {
    G4cout << "G4HEXiMinusInelastic::ApplyYourself" << G4endl;
    G4cout << "incident particle " << incidentParticle.getName()
           << "mass "              << incidentMass
           << "kinetic energy "    << incidentKineticEnergy
           << G4endl;
    G4cout << "target material with (A,Z) = (" 
           << atomicWeight << "," << atomicNumber << ")" << G4endl;
  }
    
  G4double inelasticity = NuclearInelasticity(incidentKineticEnergy, 
                                              atomicWeight, atomicNumber);
  if (verboseLevel > 1)
    G4cout << "nuclear inelasticity = " << inelasticity << G4endl;
    
  incidentKineticEnergy -= inelasticity;
    
  G4double excitationEnergyGNP = 0.;
  G4double excitationEnergyDTA = 0.; 

  G4double excitation = NuclearExcitation(incidentKineticEnergy,
                                          atomicWeight, atomicNumber,
                                          excitationEnergyGNP,
                                          excitationEnergyDTA);
  if (verboseLevel > 1)
    G4cout << "nuclear excitation = " << excitation << excitationEnergyGNP 
           << excitationEnergyDTA << G4endl;             

  incidentKineticEnergy -= excitation;
  incidentTotalEnergy = incidentKineticEnergy + incidentMass;
  // incidentTotalMomentum = std::sqrt( (incidentTotalEnergy-incidentMass)
  //                                   *(incidentTotalEnergy+incidentMass));
  // DHW 19 May 2011: variable set but not used

  G4HEVector targetParticle;
  if (G4UniformRand() < atomicNumber/atomicWeight) { 
    targetParticle.setDefinition("Proton");
  } else { 
    targetParticle.setDefinition("Neutron");
  }

  G4double targetMass = targetParticle.getMass();
  G4double centerOfMassEnergy = std::sqrt(incidentMass*incidentMass
                                        + targetMass*targetMass
                                        + 2.0*targetMass*incidentTotalEnergy);
  G4double availableEnergy = centerOfMassEnergy - targetMass - incidentMass;

  G4bool inElastic = true;
  vecLength = 0;
        
  if (verboseLevel > 1)
    G4cout << "ApplyYourself: CallFirstIntInCascade for particle "
           << incidentCode << G4endl;

  G4bool successful = false; 
    
  FirstIntInCasXiMinus(inElastic, availableEnergy, pv, vecLength,
                       incidentParticle, targetParticle, atomicWeight);

  if (verboseLevel > 1)
    G4cout << "ApplyYourself::StrangeParticlePairProduction" << G4endl;

  if ((vecLength > 0) && (availableEnergy > 1.)) 
    StrangeParticlePairProduction(availableEnergy, centerOfMassEnergy,
                                  pv, vecLength,
                                  incidentParticle, targetParticle);

  HighEnergyCascading(successful, pv, vecLength,
                      excitationEnergyGNP, excitationEnergyDTA,
                      incidentParticle, targetParticle,
                      atomicWeight, atomicNumber);
  if (!successful)
    HighEnergyClusterProduction(successful, pv, vecLength,
                                excitationEnergyGNP, excitationEnergyDTA,
                                incidentParticle, targetParticle,
                                atomicWeight, atomicNumber);
  if (!successful) 
    MediumEnergyCascading(successful, pv, vecLength, 
                          excitationEnergyGNP, excitationEnergyDTA, 
                          incidentParticle, targetParticle,
                          atomicWeight, atomicNumber);

  if (!successful)
    MediumEnergyClusterProduction(successful, pv, vecLength,
                                  excitationEnergyGNP, excitationEnergyDTA,       
                                  incidentParticle, targetParticle,
                                  atomicWeight, atomicNumber);
  if (!successful)
    QuasiElasticScattering(successful, pv, vecLength,
                           excitationEnergyGNP, excitationEnergyDTA,
                           incidentParticle, targetParticle, 
                           atomicWeight, atomicNumber);
  if (!successful)
    ElasticScattering(successful, pv, vecLength,
                      incidentParticle,    
                      atomicWeight, atomicNumber);

  if (!successful)
    G4cout << "GHEInelasticInteraction::ApplyYourself fails to produce final state particles" 
           << G4endl;

  FillParticleChange(pv, vecLength);
  delete [] pv;
  theParticleChange.SetStatusChange(stopAndKill);
  return &theParticleChange;
}


void
G4HEXiMinusInelastic::FirstIntInCasXiMinus(G4bool& inElastic,
                                           const G4double availableEnergy,
                                           G4HEVector pv[],
                                           G4int& vecLen,
                                           const G4HEVector& incidentParticle,
                                           const G4HEVector& targetParticle,
                                           const G4double atomicWeight)

// Xi0 undergoes interaction with nucleon within a nucleus.  Check if it is
// energetically possible to produce pions/kaons.  In not, assume nuclear excitation
// occurs and input particle is degraded in energy. No other particles are produced.
// If reaction is possible, find the correct number of pions/protons/neutrons
// produced using an interpolation to multiplicity data.  Replace some pions or
// protons/neutrons by kaons or strange baryons according to the average
// multiplicity per inelastic reaction.
{
  static const G4double expxu = std::log(MAXFLOAT); // upper bound for arg. of exp
  static const G4double expxl = -expxu;             // lower bound for arg. of exp

  static const G4double protb = 0.7;
  static const G4double neutb = 0.7;              
  static const G4double     c = 1.25;

  static const G4int numMul = 1200;
  static const G4int numSec = 60;

  G4int neutronCode = Neutron.getCode();
  G4int protonCode = Proton.getCode();

  G4int targetCode = targetParticle.getCode();
  G4double incidentTotalMomentum = incidentParticle.getTotalMomentum();

  static G4bool first = true;
  static G4double protmul[numMul], protnorm[numSec];  // proton constants
  static G4double neutmul[numMul], neutnorm[numSec];  // neutron constants

  //  misc. local variables
  //  np = number of pi+,  nm = number of pi-,  nz = number of pi0

  G4int i, counter, nt, np, nm, nz;

   if( first ) 
     {   // compute normalization constants, this will only be done once
       first = false;
       for( i=0; i<numMul; i++ )protmul[i]  = 0.0;
       for( i=0; i<numSec; i++ )protnorm[i] = 0.0;
       counter = -1;
       for( np=0; np<(numSec/3); np++ ) 
          {
            for( nm=std::max(0,np-1); nm<=(np+1); nm++ ) 
               {
                 for( nz=0; nz<numSec/3; nz++ ) 
                    {
                      if( ++counter < numMul ) 
                        {
                          nt = np+nm+nz;
                          if( (nt>0) && (nt<=numSec) ) 
                            {
                              protmul[counter] = pmltpc(np,nm,nz,nt,protb,c);
                              protnorm[nt-1] += protmul[counter];
                            }
                        }
                    }
               }
          }
       for( i=0; i<numMul; i++ )neutmul[i]  = 0.0;
       for( i=0; i<numSec; i++ )neutnorm[i] = 0.0;
       counter = -1;
       for( np=0; np<numSec/3; np++ ) 
          {
            for( nm=np; nm<=(np+2); nm++ ) 
               {
                 for( nz=0; nz<numSec/3; nz++ ) 
                    {
                      if( ++counter < numMul ) 
                        {
                          nt = np+nm+nz;
                          if( (nt>0) && (nt<=numSec) ) 
                            {
                               neutmul[counter] = pmltpc(np,nm,nz,nt,neutb,c);
                               neutnorm[nt-1] += neutmul[counter];
                            }
                        }
                    }
               }
          }
       for( i=0; i<numSec; i++ ) 
          {
            if( protnorm[i] > 0.0 )protnorm[i] = 1.0/protnorm[i];
            if( neutnorm[i] > 0.0 )neutnorm[i] = 1.0/neutnorm[i];
          }
     }                                          // end of initialization

         
                                              // initialize the first two places
                                              // the same as beam and target                                    
   pv[0] = incidentParticle;
   pv[1] = targetParticle;
   vecLen = 2;

   if( !inElastic ) 
     {                                     // quasi-elastic scattering, no pions produced
       G4double cech[] = {0.50, 0.45, 0.40, 0.35, 0.30, 0.25, 0.06, 0.04, 0.005, 0.};
       G4int iplab = G4int( std::min( 9.0, incidentTotalMomentum*2.5 ) );
       if( G4UniformRand() < cech[iplab]/std::pow(atomicWeight,0.42) ) 
         {                                           
           G4double ran = G4UniformRand();
           if( targetCode == neutronCode)
             {
               if (ran < 0.2)
                 {
                   pv[0] = SigmaMinus;
                   pv[1] = SigmaZero;
                 } 
               else if (ran < 0.4)
                 {
                   pv[0] = SigmaZero;
                   pv[1] = SigmaMinus;
                 }
               else if (ran < 0.6)
                 {
                   pv[0] = SigmaMinus;
                   pv[1] = Lambda;
                 }
               else if (ran < 0.8)
                 {
                   pv[0] = Lambda;
                   pv[1] = SigmaMinus;
                 }
               else
                 {
                   pv[0] = Neutron;
                   pv[1] = XiMinus;
                 }         
             }              
           else
             { 
               if (ran < 0.2)
                 {
                   pv[0] = SigmaZero;
                   pv[1] = SigmaZero;  
                 }
               else if (ran < 0.3)
                 {
                   pv[0] = Lambda;
                   pv[1] = Lambda;
                 }
               else if (ran < 0.4)
                 {
                   pv[0] = SigmaZero;
                   pv[1] = Lambda;
                 } 
               else if (ran < 0.5)
                 {
                   pv[0] = Lambda;
                   pv[1] = SigmaZero;
                 }
               else if (ran < 0.7)
                 {
                   pv[0] = SigmaZero;
                   pv[1] = Neutron;
                 }  
               else if (ran < 0.8)
                 {
                   pv[0] = Neutron;
                   pv[1] = SigmaZero;
                 }
               else
                 {
                   pv[0] = Proton;
                   pv[1] = XiMinus;
                 }  
             }
         }
       return;
     }
   else if (availableEnergy <= PionPlus.getMass())
       return;

                                                  //   inelastic scattering

   np = 0; nm = 0; nz = 0;

//                    number of total particles vs. centre of mass Energy - 2*proton mass
   
   G4double aleab = std::log(availableEnergy);
   G4double n     = 3.62567+aleab*(0.665843+aleab*(0.336514
                    + aleab*(0.117712+0.0136912*aleab))) - 2.0;
   
//                    normalization constant for kno-distribution.
//                    calculate first the sum of all constants, check for numerical problems.             
   G4double test, dum, anpn = 0.0;

   for (nt=1; nt<=numSec; nt++) {
     test = std::exp( std::min( expxu, std::max( expxl, -(pi/4.0)*(nt*nt)/(n*n) ) ) );
     dum = pi*nt/(2.0*n*n);
     if (std::fabs(dum) < 1.0) {
       if (test >= 1.0e-10) anpn += dum*test;
     } else { 
       anpn += dum*test;
     }
   }
   
   G4double ran = G4UniformRand();
   G4double excs = 0.0;
   if( targetCode == protonCode ) 
     {
       counter = -1;
       for (np=0; np<numSec/3; np++) {
         for (nm=std::max(0,np-1); nm<=(np+1); nm++) {
           for (nz=0; nz<numSec/3; nz++) {
             if (++counter < numMul) {
               nt = np+nm+nz;
               if ( (nt>0) && (nt<=numSec) ) {
                 test = std::exp( std::min( expxu, std::max( expxl, -(pi/4.0)*(nt*nt)/(n*n) ) ) );
                 dum = (pi/anpn)*nt*protmul[counter]*protnorm[nt-1]/(2.0*n*n);
                 if (std::fabs(dum) < 1.0) { 
                   if (test >= 1.0e-10) excs += dum*test;
                 } else { 
                   excs += dum*test;
		 }
                 if (ran < excs) goto outOfLoop;      //----------------------->
	       }
	     }
	   }
	 }
       }
       
       // 3 previous loops continued to the end
       inElastic = false;                 // quasi-elastic scattering   
       return;
     }
   else   
     {                                         // target must be a neutron
       counter = -1;
       for (np=0; np<numSec/3; np++) {
         for (nm=np; nm<=(np+2); nm++) { 
           for( nz=0; nz<numSec/3; nz++) {
             if (++counter < numMul) {
               nt = np+nm+nz;
               if ( (nt>=1) && (nt<=numSec) ) {
                 test = std::exp( std::min( expxu, std::max( expxl, -(pi/4.0)*(nt*nt)/(n*n) ) ) );
                 dum = (pi/anpn)*nt*neutmul[counter]*neutnorm[nt-1]/(2.0*n*n);
                 if (std::fabs(dum) < 1.0) { 
                   if( test >= 1.0e-10 )excs += dum*test;
                 } else { 
                   excs += dum*test;
	         }
                 if (ran < excs) goto outOfLoop;       // --------------------->
	       }
	     }
	   }
	 }
       }
       // 3 previous loops continued to the end
       inElastic = false;                         // quasi-elastic scattering.
       return;
     }
 
   outOfLoop:           //  <---------------------------------------------------   
    
                                                   // in the following we do not consider
   ran = G4UniformRand();                          // strangeness transfer in high multiplicity 
   if( targetCode == neutronCode)                  // events. YK combinations are added in 
     {                                             // StrangeParticlePairProduction
       if( np == nm)
         { 
         }
       else if (np == (nm-1))
         {
           if( ran < 0.50)
             {
               pv[0] = XiZero;
             }
           else
             {
               pv[1] = Proton;
             }
         }
       else
         {
           pv[0] = XiZero;
           pv[1] = Proton;
         }   
     }  
   else
     {
       if (np == nm)
         {
           if (ran < 0.5) 
             {
             }
           else
             {
               pv[0] = XiZero;
               pv[1] = Neutron;
             }  
         }  
       else if (np == (nm+1))
         {  
           pv[1] = Neutron;
         } 
       else 
         {
           pv[0] = XiZero;
         }
     }      


   nt = np + nm + nz;
   while ( nt > 0)
       {
         G4double ran = G4UniformRand();
         if ( ran < (G4double)np/nt)
            { 
              if( np > 0 ) 
                { pv[vecLen++] = PionPlus;
                  np--;
                }
            }
         else if ( ran < (G4double)(np+nm)/nt)
            {   
              if( nm > 0 )
                { 
                  pv[vecLen++] = PionMinus;
                  nm--;
                }
            }
         else
            {
              if( nz > 0 )
                { 
                  pv[vecLen++] = PionZero;
                  nz--;
                }
            }
         nt = np + nm + nz;
       } 
   if (verboseLevel > 1)
      {
        G4cout << "Particles produced: " ;
        G4cout << pv[0].getName() << " " ;
        G4cout << pv[1].getName() << " " ;
        for (i=2; i < vecLen; i++)   
            { 
              G4cout << pv[i].getName() << " " ;
            }
         G4cout << G4endl;
      }
   return;
 }










