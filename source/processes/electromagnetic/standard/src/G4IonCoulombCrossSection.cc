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
//      G4IonCoulombCrossSection.cc
//-------------------------------------------------------------------
//
// GEANT4 Class header file
//
// File name:    G4IonCoulombCrossSection
//
// Author:      Cristina Consolandi
//
// Creation date: 05.10.2010 from G4eCoulombScatteringModel 
//                                 
// Class Description:
//	Computation of Screen-Coulomb Cross Section 
//	for protons, alpha and heavy Ions
//
//
// Reference:
//      M.J. Boschini et al. "Nuclear and Non-Ionizing Energy-Loss 
//      for Coulomb Scattered Particles from Low Energy up to Relativistic 
//      Regime in Space Radiation Environment"
//      Accepted for publication in the Proceedings of  the  ICATPP Conference
//      on Cosmic Rays for Particle and Astroparticle Physics, Villa  Olmo, 7-8
//      October,  2010, to be published by World Scientific (Singapore).
//
//      Available for downloading at:
//      http://arxiv.org/abs/1011.4822
//
// -------------------------------------------------------------------
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "G4IonCoulombCrossSection.hh"
#include "Randomize.hh"
#include "G4Proton.hh"
#include "G4LossTableManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


using namespace std;

G4IonCoulombCrossSection::G4IonCoulombCrossSection():
   cosThetaMin(1.0),
   cosThetaMax(-1.0),
   alpha2(fine_structure_const*fine_structure_const)
{
  	fNistManager = G4NistManager::Instance();
        theProton   = G4Proton::Proton();
        particle=0;

  	G4double p0 = electron_mass_c2*classic_electr_radius;
  	coeff  = twopi*p0*p0;

	cosTetMinNuc=0;
	cosTetMaxNuc=0;
	nucXSection =0;


	chargeSquare = spin = mass =0;
	tkinLab = momLab2 = invbetaLab2=0;
        tkin = mom2 = invbeta2=0;

	targetZ = targetMass = screenZ =0;
        ScreenRSquare=0.;

	etag = ecut = 0.0;

}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4IonCoulombCrossSection::~G4IonCoulombCrossSection()
{}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4IonCoulombCrossSection::Initialise(const G4ParticleDefinition* p,
                                          G4double CosThetaLim)
{

	SetupParticle(p);
  	nucXSection = 0.0;
  	tkin = targetZ = mom2 = DBL_MIN;
  	ecut = etag = DBL_MAX;
  	particle = p;

		
  	cosThetaMin = CosThetaLim; 

}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4IonCoulombCrossSection::SetupKinematic(G4double ekin,
                                                     G4double cut,G4int iz )
{
        if(ekin != tkinLab || ecut != cut) {

                // lab
                tkinLab = ekin;
                momLab2 = tkinLab*(tkinLab + 2.0*mass);
                invbetaLab2 = 1.0 +  mass*mass/momLab2;

                G4double etot = tkinLab + mass;
                G4double ptot = sqrt(momLab2);
                G4double m12  = mass*mass;

                targetMass=fNistManager->GetAtomicMassAmu(iz)*amu_c2;
                G4double m2   = targetMass;

        // relativistic reduced mass from publucation
        // A.P. Martynenko, R.N. Faustov, Teoret. mat. Fiz. 64 (1985) 179
        
		//incident particle & target nucleus
	        G4double Ecm=sqrt(m12 + m2*m2 + 2.0*etot*m2);
                G4double mu_rel=mass*m2/Ecm;
                G4double momCM= ptot*m2/Ecm;
                // relative system
                mom2 = momCM*momCM;
                invbeta2 = 1.0 +  mu_rel*mu_rel/mom2;
                tkin = momCM*sqrt(invbeta2) - mu_rel;//Ekin of mu_rel
                //.........................................................

                cosTetMinNuc = cosThetaMin;
                cosTetMaxNuc = cosThetaMax;

        }

}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....


void G4IonCoulombCrossSection::SetupTarget(G4double Z, G4double e, G4int heavycorr)
{
        if(Z != targetZ || e != etag) {
                etag    = e;
                targetZ = Z;
                G4int iz= G4int(Z);

                SetScreenRSquare(iz);
  		screenZ =0;
        	screenZ = ScreenRSquare/mom2;

	//	G4cout<< "heavycorr "<<heavycorr<<G4endl;

		if(heavycorr!=0 && particle != theProton){
			G4double corr=5.*twopi*Z*std::sqrt(chargeSquare*alpha2);
			corr=std::pow(corr,0.12);
			screenZ *=(1.13 + corr*3.76*Z*Z*chargeSquare*invbeta2*alpha2)/2.;
//			G4cout<<" heavycorr Z e corr....2As "<< heavycorr << "\t"
//				<<Z <<"\t"<<e/MeV <<"\t"<<screenZ<<G4endl;

		}else{ screenZ *=(1.13 + 3.76*Z*Z*chargeSquare*invbeta2*alpha2)/2.;
//                        G4cout<<"  heavycorr Z e....2As "<< heavycorr << "\t"
//				<<Z <<"\t"<< e/MeV <<"\t"  <<screenZ<<G4endl;
		}

                if(1 == iz && particle == theProton && cosTetMaxNuc < 0.0) {
                        cosTetMaxNuc = 0.0;
                }

        }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void G4IonCoulombCrossSection::SetScreenRSquare(G4int iz){

                G4double a0 = electron_mass_c2/0.88534;

                //target nucleus
       	        G4double Z1=std::sqrt(chargeSquare);
                G4double Z2=targetZ;
        
               	G4double Z1023=std::pow(Z1,0.23);
        	G4double Z2023=std::pow(Z2,0.23);
        
		// Universal screening length
		G4double x=a0*(Z1023+Z2023);

        	//for proton Thomas-Fermi screening length	
		if(particle == theProton){ 
				x = a0*fNistManager->GetZ13(iz);
                 		}		
                ScreenRSquare  = alpha2*x*x;

	 
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double G4IonCoulombCrossSection::NuclearCrossSection()
{
  	// This method needs initialisation before be called

	// scattering with target nucleus
        G4double fac = coeff*targetZ*targetZ*chargeSquare*invbeta2/mom2;

  	nucXSection  = 0.0;

  	G4double x  = 1.0 - cosTetMinNuc;
  	G4double x1 = x + screenZ;

	// scattering with nucleus
  	if(cosTetMaxNuc < cosTetMinNuc) {

		nucXSection =fac*(cosTetMinNuc - cosTetMaxNuc)/
                        (x1*(1.0 - cosTetMaxNuc + screenZ));

    		}

  return nucXSection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double G4IonCoulombCrossSection::SampleCosineTheta()
{
  	
  	if(cosTetMaxNuc >= cosTetMinNuc) return 0.0;

  	G4double x1 = 1. - cosTetMinNuc + screenZ;
  	G4double x2 = 1. - cosTetMaxNuc + screenZ;
  	G4double dx = cosTetMinNuc - cosTetMaxNuc;
  	G4double /*grej,*/ z1;

    		z1 = x1*x2/(x1 + G4UniformRand()*dx) - screenZ;
                //grej = 1.0/(1.0 + z1);
  return z1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......




