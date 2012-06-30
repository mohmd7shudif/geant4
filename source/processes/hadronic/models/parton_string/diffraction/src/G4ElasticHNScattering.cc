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
//
// $Id: G4ElasticHNScattering.cc,v 1.14 2009-12-16 17:51:13 gunter Exp $
// ------------------------------------------------------------
//      GEANT 4 class implemetation file
//
//      ---------------- G4ElasticHNScattering --------------
//                   by V. Uzhinsky, March 2008.
//             elastic scattering used by Fritiof model
//	           Take a projectile and a target
//	           scatter the projectile and target
// ---------------------------------------------------------------------


#include "globals.hh"
#include "Randomize.hh"

#include "G4ElasticHNScattering.hh"
#include "G4LorentzRotation.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4VSplitableHadron.hh"
#include "G4ExcitedString.hh"
#include "G4FTFParameters.hh"
//#include "G4ios.hh"

G4ElasticHNScattering::G4ElasticHNScattering()          
{
}

G4bool G4ElasticHNScattering::
           ElasticScattering (G4VSplitableHadron *projectile, 
                              G4VSplitableHadron *target,
                              G4FTFParameters    *theParameters) const
{
// -------------------- Projectile parameters -----------------------------------
	   G4LorentzVector Pprojectile=projectile->Get4Momentum();

           if(Pprojectile.z() < 0.)
           {
            target->SetStatus(2);
            return false;
           } 

           G4bool PutOnMassShell(false);

	   G4double M0projectile = Pprojectile.mag();                        
           if(M0projectile < projectile->GetDefinition()->GetPDGMass()) 
           {
              PutOnMassShell=true;
              M0projectile=projectile->GetDefinition()->GetPDGMass();
           }

	   G4double Mprojectile2 = M0projectile * M0projectile;

           G4double AveragePt2=theParameters->GetAvaragePt2ofElasticScattering();

// -------------------- Target parameters ----------------------------------------------

  	   G4LorentzVector Ptarget=target->Get4Momentum();

	   G4double M0target = Ptarget.mag();

           if(M0target < target->GetDefinition()->GetPDGMass()) 
           {
              PutOnMassShell=true;
              M0target=target->GetDefinition()->GetPDGMass();
           }
     
   	   G4double Mtarget2 = M0target * M0target;                      

// Transform momenta to cms and then rotate parallel to z axis;

	   G4LorentzVector Psum;
	   Psum=Pprojectile+Ptarget;

	   G4LorentzRotation toCms(-1*Psum.boostVector());

	   G4LorentzVector Ptmp=toCms*Pprojectile;

	   if ( Ptmp.pz() <= 0. )                                
	   {
	   // "String" moving backwards in  CMS, abort collision !!
           //G4cout << " abort Collision!! " << G4endl;
                   target->SetStatus(2);
		   return false; 
	   }
	   		   
	   toCms.rotateZ(-1*Ptmp.phi());
	   toCms.rotateY(-1*Ptmp.theta());
	
	   G4LorentzRotation toLab(toCms.inverse());

	   Pprojectile.transform(toCms);
	   Ptarget.transform(toCms);

// ---------------------- Putting on mass-on-shell, if needed ------------------------
           G4double PZcms2, PZcms;                                          

           G4double S=Psum.mag2();                                          
//         G4double SqrtS=std::sqrt(S);                                     

	   PZcms2=(S*S+Mprojectile2*Mprojectile2+Mtarget2*Mtarget2-
                                 2*S*Mprojectile2-2*S*Mtarget2-2*Mprojectile2*Mtarget2)/4./S;

           if(PZcms2 < 0.)
           {  // It can be in an interaction with off-shell nuclear nucleon
            if(M0projectile > projectile->GetDefinition()->GetPDGMass()) 
            {  // An attempt to de-excite the projectile
               // It is assumed that the target is in the ground state
             M0projectile = projectile->GetDefinition()->GetPDGMass();
             Mprojectile2=M0projectile*M0projectile;
             PZcms2=(S*S+Mprojectile2*Mprojectile2+Mtarget2*Mtarget2-
                    2*S*Mprojectile2 - 2*S*Mtarget2 - 2*Mprojectile2*Mtarget2)
                    /4./S;
             if(PZcms2 < 0.){ return false;} // Non succesful attempt after the de-excitation
            }
            else // if(M0projectile > projectile->GetDefinition()->GetPDGMass())
            {
             target->SetStatus(2);                                   
             return false;                   // The projectile was not excited,
                                             // but the energy was too low to put
                                             // the target nucleon on mass-shell
            }   // end of if(M0projectile > projectile->GetDefinition()->GetPDGMass())
           }    // end of if(PZcms2 < 0.)

           PZcms = std::sqrt(PZcms2);

           if(PutOnMassShell)
           {
              if(Pprojectile.z() > 0.)
              {
                 Pprojectile.setPz( PZcms);
                 Ptarget.setPz(    -PZcms);
              }
              else  // if(Pprojectile.z() > 0.)
              {
                 Pprojectile.setPz(-PZcms);
                 Ptarget.setPz(     PZcms);
              };

              Pprojectile.setE(std::sqrt(Mprojectile2+
                                                      Pprojectile.x()*Pprojectile.x()+
                                                      Pprojectile.y()*Pprojectile.y()+
                                                      PZcms2));
              Ptarget.setE(std::sqrt(    Mtarget2    +
                                                      Ptarget.x()*Ptarget.x()+
                                                      Ptarget.y()*Ptarget.y()+
                                                      PZcms2));
           }  // end of if(PutOnMassShell)

           G4double maxPtSquare = PZcms2;

// ------ Now we can calculate the transfered Pt --------------------------
	   G4double Pt2;                                                    
           G4double ProjMassT2; //, ProjMassT;                                  
           G4double TargMassT2; //, TargMassT;

	   G4LorentzVector Qmomentum;

	   Qmomentum=G4LorentzVector(GaussianPt(AveragePt2,maxPtSquare),0);

	   Pt2=G4ThreeVector(Qmomentum.vect()).mag2();                  

           ProjMassT2=Mprojectile2+Pt2;                           
//           ProjMassT =std::sqrt(ProjMassT2);                            

           TargMassT2=Mtarget2+Pt2;                               
//           TargMassT =std::sqrt(TargMassT2);                            

           PZcms2=(S*S+ProjMassT2*ProjMassT2+                           
                       TargMassT2*TargMassT2-                           
                    2.*S*ProjMassT2-2.*S*TargMassT2-                 
                    2.*ProjMassT2*TargMassT2)/4./S;                  
           if(PZcms2 < 0 ) {PZcms2=0;};// to avoid the exactness problem
           PZcms =std::sqrt(PZcms2);                                    

	   Pprojectile.setPz( PZcms);  
	   Ptarget.setPz(    -PZcms); 

	   Pprojectile += Qmomentum;
	   Ptarget     -= Qmomentum;

// Transform back and update SplitableHadron Participant.
	   Pprojectile.transform(toLab);
	   Ptarget.transform(toLab);
/*  // Maybe it will be needed for an exact calculations--------------------
           G4double TargetMomentum=std::sqrt(Ptarget.x()*Ptarget.x()+
                                             Ptarget.y()*Ptarget.y()+
                                             Ptarget.z()*Ptarget.z());
*/

// Calculation of the creation time ---------------------
      projectile->SetTimeOfCreation(target->GetTimeOfCreation());
      projectile->SetPosition(target->GetPosition());
// Creation time and position of target nucleon were determined at
// ReggeonCascade() of G4FTFModel
// ------------------------------------------------------

	   projectile->Set4Momentum(Pprojectile);
	   target->Set4Momentum(Ptarget);

           projectile->IncrementCollisionCount(1);
           target->IncrementCollisionCount(1);

	   return true;
}


// --------- private methods ----------------------

G4ThreeVector G4ElasticHNScattering::GaussianPt(G4double AveragePt2, G4double maxPtSquare) const
{            //  @@ this method is used in FTFModel as well. Should go somewhere common!
	
	G4double Pt2(0.);
        if(AveragePt2 <= 0.) {Pt2=0.;}
        else
        {
         Pt2 = -AveragePt2 * std::log(1. + G4UniformRand() * 
                (std::exp(-maxPtSquare/AveragePt2)-1.)); 
	}
	G4double Pt=std::sqrt(Pt2);
	G4double phi=G4UniformRand() * twopi;
	
	return G4ThreeVector (Pt*std::cos(phi), Pt*std::sin(phi), 0.);    
}

G4ElasticHNScattering::G4ElasticHNScattering(const G4ElasticHNScattering &)
{
	throw G4HadronicException(__FILE__, __LINE__, "G4ElasticHNScattering copy contructor not meant to be called");
}


G4ElasticHNScattering::~G4ElasticHNScattering()
{
}


const G4ElasticHNScattering & G4ElasticHNScattering::operator=(const G4ElasticHNScattering &)
{
	throw G4HadronicException(__FILE__, __LINE__, "G4ElasticHNScattering = operator meant to be called");
	return *this;
}


int G4ElasticHNScattering::operator==(const G4ElasticHNScattering &) const
{
	throw G4HadronicException(__FILE__, __LINE__, "G4ElasticHNScattering == operator meant to be called");
	return false;
}

int G4ElasticHNScattering::operator!=(const G4ElasticHNScattering &) const
{
	throw G4HadronicException(__FILE__, __LINE__, "G4ElasticHNScattering != operator meant to be called");
	return true;
}
