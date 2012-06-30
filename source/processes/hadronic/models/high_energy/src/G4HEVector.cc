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
// $Id: G4HEVector.cc,v 1.22 2010-11-29 05:44:44 dennis Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//

#include "globals.hh"
#include "G4ios.hh"

//
// G4 Gheisha friend class G4GHEVector
// J.L. Chuma, TRIUMF, 22-Feb-1996
// last modified: H. Fesefeldt 02-July--1998
// Fesefeldt, bug fixed in Defs1, 14 August 2000

#include "G4HEVector.hh"
#include "G4ParticleDefinition.hh"

G4HEVector::G4HEVector(const G4HadProjectile * aParticle)
  {
     G4ThreeVector aMom = 1./GeV*aParticle->Get4Momentum().vect();
     px               = aMom.x();
     py               = aMom.y();
     pz               = aMom.z();
     energy           = aParticle->GetTotalEnergy()/GeV;
     kineticEnergy    = aParticle->GetKineticEnergy()/GeV;
     mass             = aParticle->GetDefinition()->GetPDGMass()/GeV;
     charge           = aParticle->GetDefinition()->GetPDGCharge()/eplus;
     timeOfFlight     = 0.0;
     side             = 0;
     flag             = false;
     code             = aParticle->GetDefinition()->GetPDGEncoding();
     baryon           = aParticle->GetDefinition()->GetBaryonNumber();
     particleName     = getParticleName(code, baryon);
     particleType     = aParticle->GetDefinition()->GetParticleType();
     strangeness       = aParticle->GetDefinition()->GetQuarkContent(3);
  }
  
G4double G4HEVector::Amax(G4double a, G4double b)
  {
     G4double c = a;
     if(b > a) c = b;
     return c;
  } 

G4String G4HEVector::getParticleName(G4int aCode, G4int aBaryon)
   {
        G4String name;
	if(aCode == 211) name = "PionPlus";
        else if(aCode == 111) name = "PionZero";
        else if(aCode == -211) name = "PionMinus";
        else if(aCode == 321) name = "KaonPlus";
        else if(aCode == 311) name = "KaonZero";
        else if(aCode == -311) name = "AntiKaonZero";
        else if(aCode == -321) name = "KaonMinus";
        else if(aCode == 310) name = "KaonZeroShort";
        else if(aCode == 130) name = "KaonZeroLong";
        else if(aCode == 2212) name = "Proton";
        else if(aCode == -2212) name = "AntiProton";
        else if(aCode == 2112) name = "Neutron";
        else if(aCode == -2112) name = "AntiNeutron";
        else if(aCode == 3122) name = "Lambda";
        else if(aCode == -3122) name = "AntiLambda";
        else if(aCode == 3222) name = "SigmaPlus";
        else if(aCode == 3212) name = "SigmaZero";
        else if(aCode == 3112) name = "SigmaMinus";
        else if(aCode == -3222) name = "AntiSigmaPlus";
        else if(aCode == -3212) name = "AntiSigmaZero";
        else if(aCode == -3112) name = "AntiSigmaMinus";
        else if(aCode == 3322) name = "XiZero";
        else if(aCode == 3312) name = "XiMinus";
        else if(aCode == -3322) name = "AntiXiZero";
        else if(aCode == -3312) name = "AntiXiMinus";
        else if(aCode == 3334) name = "OmegaMinus";
        else if(aCode == -3334) name = "AntiOmegaMinus";
        else if(aCode == 0)
	{ 
	  if(aBaryon==2) name = "Deuteron";
          else if(aBaryon==3) name = "Triton";
          else if(aBaryon==4) name = "Alpha";
	}
        else if(aCode == 22) name = "Gamma";
        else
          {
               G4cout << "particle " << aCode << "  "  <<aBaryon<< " not known in this generator!!" << G4endl;
          }
        return name;
   } 


void 
G4HEVector::setMomentum(const G4ParticleMomentum mom ) 
   {
      px  = mom.x();
      py  = mom.y();
      pz  = mom.z(); 
      return; 
   }

void 
G4HEVector::setMomentum(const G4ParticleMomentum * mom ) 
   {
      px  = mom->x();
      py  = mom->y();
      pz  = mom->z(); 
      return; 
   }

void 
G4HEVector::setMomentumAndUpdate( const G4ParticleMomentum mom )
   {
     px = mom.x();
     py = mom.y();
     pz = mom.z();
     energy        = std::sqrt(mass*mass + px*px + py*py + pz*pz);
     kineticEnergy = Amax(0.,energy - mass);
     return;
   }

void 
G4HEVector::setMomentumAndUpdate( const G4ParticleMomentum * mom )
   {
     px = mom->x();
     py = mom->y();
     pz = mom->z();
     energy        = std::sqrt(mass*mass + px*px + py*py + pz*pz);
     kineticEnergy = Amax(0.,energy - mass);
     return;
   }

const G4ParticleMomentum 
G4HEVector::getMomentum() const 
   { 
     G4ParticleMomentum mom;
     mom.setX(px);
     mom.setY(py);
     mom.setZ(pz);
     return mom; 
   }

G4double G4HEVector::getTotalMomentum() const
{
  return std::sqrt(px*px + py*py + pz*pz);
}

void
G4HEVector::setMomentum(G4double x, G4double y, G4double z)
{ 
  px = x;
  py = y;
  pz = z;
  return;
} 

void 
G4HEVector::setMomentumAndUpdate(G4double x, G4double y, G4double z)
{
  px = x;
  py = y;
  pz = z;
  energy = std::sqrt(mass*mass + px*px + py*py + pz*pz);
  kineticEnergy = Amax(0.,energy-mass);
  return;
}

void 
G4HEVector::setMomentum(G4double x, G4double y)
{
  px = x;
  py = y;
  return;
}

void 
G4HEVector::setMomentumAndUpdate( G4double x, G4double y )
   {
     px = x;
     py = y;
     energy = std::sqrt(mass*mass + px*px + py*py + pz*pz);
     kineticEnergy = Amax(0.,energy-mass);
     return;
   }

void 
G4HEVector::setMomentum( G4double z )
   {
     pz = z;
     return;
   }

void 
G4HEVector::setMomentumAndUpdate( G4double z )
   {
     pz = z;
     energy = std::sqrt(mass*mass + px*px + py*py + pz*pz);
     kineticEnergy = Amax(0.,energy-mass);
     return;
   }

void 
G4HEVector::setEnergy( G4double e ) 
   { 
     energy = e; 
     return; 
   }

void 
G4HEVector::setEnergyAndUpdate( G4double e )
   {
     if (e <= mass)
       { 
         energy        = mass;
         kineticEnergy = 0.;
         px            = 0.;
         py            = 0.;
         pz            = 0.;
       }
     else
       {
         energy = e;
         kineticEnergy   = energy - mass;
         G4double momold = std::sqrt(px*px + py*py + pz*pz);
         G4double momnew = std::sqrt(energy*energy - mass*mass);
         if (momold == 0.)
           {
             G4double cost = 1.0- 2.0*G4UniformRand();
             G4double sint = std::sqrt(1. - cost*cost);
             G4double phi  = twopi* G4UniformRand();
             px            = momnew * sint * std::cos(phi);
             py            = momnew * sint * std::sin(phi);
             pz            = momnew * cost;
           }
         else
           {
             momnew /= momold;
             px     *= momnew;
             py     *= momnew;
             pz     *= momnew;
           }
       }    
     return;
   }

void 
G4HEVector::setKineticEnergy( G4double ekin ) 
   { 
     kineticEnergy = ekin;
     return; 
   }
 
void 
G4HEVector::setKineticEnergyAndUpdate(G4double ekin) 
   {
     if (ekin <= 0.)
       { 
         energy        = mass;
         kineticEnergy = 0.;
         px            = 0.;
         py            = 0.;
         pz            = 0.;
       }
     else
       {
         energy = ekin + mass;
         kineticEnergy   = ekin;
         G4double momold = std::sqrt(px*px + py*py + pz*pz);
         G4double momnew = std::sqrt(energy*energy - mass*mass);
         if (momold == 0.)
           {
             G4double cost = 1.0-2.0*G4UniformRand();
             G4double sint = std::sqrt(1. - cost*cost);
             G4double phi  = twopi* G4UniformRand();
             px            = momnew * sint * std::cos(phi);
             py            = momnew * sint * std::sin(phi);
             pz            = momnew * cost;
           }
         else
           {
             momnew /= momold;
             px     *= momnew;
             py     *= momnew;
             pz     *= momnew;
           }
       }    
     return;
   }

G4double G4HEVector::getEnergy() const 
{
  return energy;
}

G4double G4HEVector::getKineticEnergy() const 
{
  return kineticEnergy;
}

void 
G4HEVector::setMass( G4double m ) 
   { 
     mass = m; 
     return; 
   }

void 
G4HEVector::setMassAndUpdate( G4double m )
   {
     kineticEnergy = Amax(0., energy - mass);
     mass = m;
     energy = kineticEnergy + mass;
     G4double momnew = std::sqrt(Amax(0., energy*energy - mass*mass));
     if ( momnew == 0.0) 
        {
         px = 0.;
         py = 0.;
         pz = 0.;
        }
    else
        {
         G4double momold = std::sqrt(px*px + py*py + pz*pz);
         if (momold == 0.)
            { 
              G4double cost = 1.-2.*G4UniformRand();
              G4double sint = std::sqrt(1.-cost*cost);
              G4double phi  = twopi*G4UniformRand();
              px            = momnew*sint*std::cos(phi);
              py            = momnew*sint*std::sin(phi);
              pz            = momnew*cost;
            }
         else
            {
              momnew /= momold;
              px     *= momnew ;
              py     *= momnew ;
              pz     *= momnew ;
            }
        }     
     return;
   }    

G4double G4HEVector::getMass() const 
{ 
  return mass; 
}

void 
G4HEVector::setCharge( G4double c ) 
   { 
     charge = c; 
     return; 
   }

G4double G4HEVector::getCharge() const 
{
  return charge; 
}

void 
G4HEVector::setTOF( G4double t ) 
   { 
     timeOfFlight = t; 
     return; 
   }

G4double 
G4HEVector::getTOF() 
   { 
     return timeOfFlight; 
   }

void 
G4HEVector::setSide( G4int s ) 
   { 
     side = s; 
     return; 
   }

G4int 
G4HEVector::getSide() 
   { 
     return side; 
   }


void 
G4HEVector::setFlag( G4bool f ) 
   { 
     flag = f; 
     return; 
   }

G4bool 
G4HEVector::getFlag() 
   { 
     return flag; 
   }

void 
G4HEVector::setCode( G4int c ) 
   { 
     code = c; 
     return; 
   }

G4int G4HEVector::getCode() const 
{ 
  return code; 
} 

G4String G4HEVector::getName() const
{
  return particleName;
}

G4String G4HEVector::getType() const
{
  return particleType;
}
 
G4int G4HEVector::getBaryonNumber() const
{
  return baryon;
}

G4int G4HEVector::getStrangenessNumber() const
{
  return strangeness;
}

G4int 
G4HEVector::getQuarkContent(G4int flavor)
   {
     if(flavor > 0 && flavor < 8)
     { 
       G4int check;
       check = FillQuarkContent();
       if(check  != code) 
       { 
         return 0;
       }
       else
       {
         return theQuarkContent[flavor-1];
       }
     }
     else
     {
       return 0;
     }
   } 

G4int 
G4HEVector::getAntiQuarkContent(G4int flavor)
   {
     if(flavor > 0 && flavor < 8)
     { 
       G4int check;
       check = FillQuarkContent();
       if(check  != code) 
       { 
         return 0;
       }
       else
       {
         return theAntiQuarkContent[flavor-1];
       }
     }
     else
     {
       return 0;
     }
   } 

void 
G4HEVector::setZero()
   {
     px            = 0.0;
     py            = 0.0;
     pz            = 0.0;
     energy        = 0.0;
     kineticEnergy = 0.0;
     mass          = 0.0;
     charge        = 0.0;
     timeOfFlight  = 0.0;
     side          = 0;
     flag          = false;
     code          = 0;
     particleName  = "";
     particleType  = "";
     baryon        = 0;
     strangeness   = 0;
   }

void 
G4HEVector::Add( const G4HEVector & p1, const G4HEVector & p2 )
   {
     px  = p1.px + p2.px;
     py  = p1.py + p2.py;
     pz  = p1.pz + p2.pz;
     energy = p1.energy + p2.energy;
     G4double b = energy*energy - px*px - py*py - pz*pz;
     if( b < 0 )
       mass = -1. * std::sqrt( -b );
     else
       mass = std::sqrt( b );
     kineticEnergy = Amax(0.,energy - mass);
     charge        = p1.charge + p2.charge;
     code          = 0;
     particleName  = "";
     particleType  = "";
     baryon        = 0;
     strangeness   = 0;
   }

void 
G4HEVector::Sub( const G4HEVector & p1, const G4HEVector & p2 )
   {
     px  = p1.px - p2.px;
     py  = p1.py - p2.py;
     pz  = p1.pz - p2.pz;
     energy = p1.energy - p2.energy;
     G4double b = energy*energy - px*px - py*py - pz*pz;
     if( b < 0 )
       mass = -1. * std::sqrt( -b );
     else
       mass = std::sqrt( b );
     kineticEnergy = Amax(0.,energy - mass);
     charge        = p1.charge - p2.charge;
     code          = 0;
     particleName  = "";
     particleType  = "";
     baryon        = 0;
     strangeness   = 0;
   }

void 
G4HEVector::Lor( const G4HEVector & p1, const G4HEVector & p2 )
   {
     G4double a;
     a  = ( Dot(p1,p2)/(p2.energy+p2.mass) - p1.energy ) / p2.mass;
     px = p1.px + a*p2.px;
     py = p1.py + a*p2.py;
     pz = p1.pz + a*p2.pz; 
     energy = std::sqrt( sqr(p1.mass) + px*px + py*py + pz*pz);
     mass = p1.mass;
     kineticEnergy = Amax(0.,energy - mass);
     timeOfFlight  = p1.timeOfFlight;
     side          = p1.side;
     flag          = p1.flag;
     code          = p1.code;
     particleName  = p1.particleName;
     particleType  = p1.particleType; 
     baryon        = p1.baryon;
     strangeness   = p1.strangeness;
   }

G4double G4HEVector::CosAng(const G4HEVector& p) const
{
  G4double a = std::sqrt( (px*px + py*py + pz*pz)*(p.px*p.px + p.py*p.py + p.pz*p.pz) );
  if (a != 0.0) {
    a = (px*p.px + py*p.py + pz*p.pz)/a;
    if (std::fabs(a) > 1.0) {
      if (a < 0.0) a = -1.0;
      else a = 1.0;
    }   
  }
  return a;
}

G4double 
G4HEVector::Ang(const G4HEVector & p )
   {
     G4double a = std::sqrt( (px*px + py*py + pz*pz)*(p.px*p.px + p.py*p.py + p.pz*p.pz) );
     if( a != 0.0 ) 
       {
         a = (px*p.px + py*p.py + pz*p.pz)/a;
         if( std::fabs(a) > 1.0 ) 
         {
           if(a<0.0) a=-1.0;
           else a=1.0;
         } 
       }
     return std::acos(a);
   }      

G4double 
G4HEVector::Dot4( const G4HEVector & p1, const G4HEVector & p2)
   {
     return ( p1.energy*p2.energy - p1.px*p2.px - p1.py*p2.py - p1.pz*p2.pz );
   } 
  
G4double 
G4HEVector::Impu( const G4HEVector & p1, const G4HEVector & p2)
   {
     return ( - sqr( p1.energy  - p2.energy)  
              + sqr( p1.px      - p2.px)
              + sqr( p1.py      - p2.py) 
              + sqr( p1.pz      - p2.pz) );
   }

void 
G4HEVector::Add3( const G4HEVector & p1, const G4HEVector & p2)
   {
     px =  p1.px + p2.px;
     py =  p1.py + p2.py;
     pz =  p1.pz + p2.pz;    
     return;
   }

void 
G4HEVector::Sub3( const G4HEVector & p1, const G4HEVector & p2)
   {
     px =  p1.px - p2.px;
     py =  p1.py - p2.py;
     pz =  p1.pz - p2.pz;
     return;
   }

void 
G4HEVector::Cross( const G4HEVector & p1, const G4HEVector & p2)
   {
     G4double tpx = p1.py * p2.pz - p1.pz * p2.py;
     G4double tpy = p1.pz * p2.px - p1.px * p2.pz;
     G4double tpz = p1.px * p2.py - p1.py * p2.px;
     px=tpx;
     py=tpy;
     pz=tpz;
     return;
   }  
 
G4double 
G4HEVector::Dot( const G4HEVector & p1, const G4HEVector & p2)
   {
     return ( p1.px * p2.px + p1.py * p2.py + p1.pz * p2.pz );
   }

void 
G4HEVector::Smul( const G4HEVector & p, G4double h)
   {
     px =  h * p.px;
     py =  h * p.py;
     pz =  h * p.pz;
     return;
   }   

void 
G4HEVector::SmulAndUpdate( const G4HEVector & p, G4double h)
   {
     px = h * p.px;
     py = h * p.py;
     pz = h * p.pz;
     mass          = p.mass;
     energy        = std::sqrt(px*px + py*py + pz*pz + mass*mass);
     kineticEnergy = energy - mass;
     charge        = p.charge;
     timeOfFlight  = p.timeOfFlight;
     side          = p.side;
     flag          = p.flag;
     code          = p.code;
     particleName  = p.particleName;
     particleType  = p.particleType;
     baryon        = p.baryon;
     strangeness   = p.strangeness;
     return;
   }
 
void 
G4HEVector::Norz( const G4HEVector & p )
   {
     G4double a =   p.px*p.px + p.py*p.py + p.pz*p.pz;
     if (a > 0.0) a = 1./std::sqrt(a);
     px = a * p.px;
     py = a * p.py;
     pz = a * p.pz;
     mass          = p.mass;
     energy        = std::sqrt(px*px + py*py + pz*pz + mass*mass);
     kineticEnergy = energy - mass;
     charge        = p.charge;
     timeOfFlight  = p.timeOfFlight;
     side          = p.side;
     flag          = p.flag;
     code          = p.code; 
     particleName  = p.particleName;
     particleType  = p.particleType;
     baryon        = p.baryon;
     strangeness   = p.strangeness;
     return;
   }

G4double G4HEVector::Length() const
{
  return std::sqrt(px*px + py*py + pz*pz);
}

void 
G4HEVector::Exch( G4HEVector & p1)
   {
     G4HEVector mx = *this;
     *this = p1;
     p1 = mx;
     return; 
   }      

void 
G4HEVector::Defs1( const G4HEVector & p1, const G4HEVector & p2)
   {
     G4double pt2 = p2.px*p2.px + p2.py*p2.py;
     if (pt2 > 0.0)
        {
          G4double ph, qx, qy, qz;
          G4double a     = std::sqrt(p2.px*p2.px + p2.py*p2.py + p2.pz*p2.pz);
          G4double cost  = p2.pz/a; 
          G4double sint  = 0.5 * (std::sqrt(std::fabs((1.-cost)*(1.+cost))) + std::sqrt(pt2)/a);
          if(p2.py < 0.) ph = 1.5*pi;
          else ph = halfpi;
          if( p2.px != 0.0) 
             ph = std::atan2(p2.py,p2.px);             
          qx =   cost*std::cos(ph)*p1.px - std::sin(ph)*p1.py
               + sint*std::cos(ph)*p1.pz;
          qy =   cost*std::sin(ph)*p1.px + std::cos(ph)*p1.py
               + sint*std::sin(ph)*p1.pz;
          qz = - sint        *p1.px 
               + cost        *p1.pz;
          px = qx; 
          py = qy;
          pz = qz;     
        }
     else
        {
          px = p1.px;
          py = p1.py;
          pz = p1.pz;               
	} 
   }
 
void 
G4HEVector::Defs( const G4HEVector & p1, const G4HEVector & p2,
                  G4HEVector & my,       G4HEVector & mz )
   {
     my = p1;
     mz = p2;
     px = my.py*mz.pz - my.pz*mz.py;
     py = my.pz*mz.px - my.px*mz.pz;
     pz = my.px*mz.py - my.py*mz.px;
     my.px = mz.py*pz - mz.pz*py;
     my.py = mz.pz*px - mz.px*pz;
     my.pz = mz.px*py - mz.py*px;
     G4double pp;
     pp = std::sqrt(px*px + py*py + pz*pz);
     if (pp > 0.)
        {
          pp = 1./pp; 
          px = px*pp ;
          py = py*pp ;
          pz = pz*pp ;
        }
     pp = std::sqrt(my.px*my.px + my.py*my.py + my.pz*my.pz);
     if (pp > 0.)
        {
          pp = 1./pp;
          my.px = my.px*pp ;
          my.py = my.py*pp ;
          my.pz = my.pz*pp ;
        }
     pp = std::sqrt(mz.px*mz.px + mz.py*mz.py + mz.pz*mz.pz);
     if (pp > 0.)
        {
          pp = 1./pp;
          mz.px = mz.px*pp ;
          mz.py = mz.py*pp ;
          mz.pz = mz.pz*pp ;
        }
     return; 
   }  
             
void 
G4HEVector::Trac( const G4HEVector & p1, const G4HEVector & mx,
              const G4HEVector & my, const G4HEVector & mz)
   {
     G4double qx, qy, qz;
     qx =   mx.px*p1.px + mx.py*p1.py + mx.pz*p1.pz;
     qy =   my.px*p1.px + my.py*p1.py + my.pz*p1.pz;
     qz =   mz.px*p1.px + mz.py*p1.py + mz.pz*p1.pz;
     px = qx ;
     py = qy ;
     pz = qz ;
     return;
   }                 

void 
G4HEVector::setDefinition(G4String name)
   {
        if(name == "PionPlus")
          { 
            mass = 0.1395700;
            charge = 1.;
            code = 211;
            particleType = "Meson";
            particleName = name;
            baryon       = 0;
            strangeness  = 0;
	  }
        else if(name == "PionZero")
          {
             mass = 0.1349764;
             charge = 0.;
             code = 111;
             particleType = "Meson";
             particleName = name;
             baryon       = 0;
             strangeness  = 0; 
          }
        else if(name == "PionMinus")
          {
              mass = 0.1395700;
              charge = -1.;
              code = -211;
              particleType = "Meson";
              particleName = name;
              baryon       = 0;
              strangeness  = 0;
          }        
        else if(name == "KaonPlus")
          {
              mass = 0.493677;
              charge = 1.;
              code = 321;
              particleType = "Meson";
              particleName = name;
              baryon       = 0;
              strangeness  = 1;
          }
        else if(name == "KaonZero")
          {
              mass = 0.497672;
              charge = 0.;
              code = 311;
              particleType = "Meson";
              particleName = name;
              baryon       = 0;
              strangeness  = 1;
          }
        else if(name == "AntiKaonZero")
          {
              mass = 0.497672;
              charge = 0.;
              code = -311;
              particleType = "Meson";
              particleName = name;
              baryon       = 0;
              strangeness  =-1;
          }
        else if(name == "KaonMinus")
          {
              mass = 0.493677;
              charge = -1.;
              code = -321;
              particleType = "Meson";
              particleName = name;
              baryon       = 0;
              strangeness  = -1;          
          }
        else if(name == "KaonZeroShort")
          {
              mass = 0.497672;
              charge = 0.;
              code = 310;
              particleType = "Meson";
              particleName = name;
              baryon       = 0;
              strangeness  = 0;
          }
        else if(name == "KaonZeroLong")
          {
              mass = 0.497672;
              charge = 0.;
              code = 130;
              particleType = "Meson";
              particleName = name;
              baryon       = 0;
              strangeness  = 0;
          }
        else if(name == "Proton")
          {
              mass = 0.9382723;
              charge = 1.;
              code = 2212;
              particleType = "Baryon";
              particleName = name;
              baryon       = 1;
              strangeness  = 0;
          }
        else if(name == "AntiProton")
          {
              mass = 0.9382723;
              charge = -1.;
              code = -2212;
              particleType = "AntiBaryon";
              particleName = name;
              baryon       = -1;
              strangeness  = 0;
	  }
        else if(name == "Neutron")
          {
              mass = 0.93956563;
              charge = 0.;
              code = 2112;
              particleType = "Baryon";
              particleName = name;
              baryon       = 1;
              strangeness  = 0;
          }
        else if(name == "AntiNeutron")
          {
              mass = 0.93956563;
              charge = 0.;
              code = -2112;
              particleType = "AntiBaryon";
              particleName = name;
              baryon = -1;
              strangeness  = 0;
          }
        else if(name == "Lambda")
          {
              mass = 1.115684; 
              charge = 0.;
              code = 3122;
              particleType = "Baryon";
              particleName = name;
              baryon = 1;
              strangeness  = -1;
          }
        else if(name == "AntiLambda")
          {
              mass = 1.115684;
              charge = 0.;
              code = -3122;
              particleType = "AntiBaryon";
              particleName = name;
              baryon = -1;
              strangeness  = 1;
          }
        else if(name == "SigmaPlus")
          {
              mass = 1.18937;
              charge = 1.;
              code = 3222;
              particleType = "Baryon";
              particleName = name;
              baryon       = 1;
              strangeness  = -1;
          }
        else if(name == "SigmaZero") 
          {
              mass = 1.19255;
              charge = 0.;
              code = 3212;
              particleType = "Baryon";
              particleName = name;
              baryon       = 1;
              strangeness  = -1;
          }
        else if(name == "SigmaMinus")
          {
              mass = 1.19744;
              charge = -1.;
              code = 3112;
              particleType = "Baryon";
              particleName = name;
              baryon       = 1;
              strangeness  = -1;
          }
        else if(name == "AntiSigmaPlus") 
          {
              mass = 1.18937;
              charge = -1.;
              code = -3222;
              particleType = "AntiBaryon";
              particleName = name;
              baryon = -1;
              strangeness  = 1;
          }
        else if(name == "AntiSigmaZero")
          {
              mass = 1.19255;
              charge = 0.;
              code = -3212;
              particleType = "AntiBaryon";
              particleName = name;
              baryon       = -1;
              strangeness  = 1;
          }        
        else if(name == "AntiSigmaMinus")
          {
              mass = 1.19744;
              charge = 1.;
              code = -3112;
              particleType = "AntiBaryon";
              particleName = name;
              baryon       = -1;
              strangeness  = 1;
          }
        else if(name == "XiZero")
          {
              mass = 1.3149;
              charge = 0.;
              code = 3322;
              particleType = "Baryon";
              particleName = name;
              baryon       = 1;
              strangeness  = -2;
          }        
        else if(name == "XiMinus")
          {
              mass = 1.32132;
              charge = -1.;
              code = 3312;
              particleType = "Baryon";
              particleName = name;
              baryon       = 1;
              strangeness  = -2;
          }
        else if(name == "AntiXiZero")
          {
               mass = 1.3149;
               charge = 0.;
               code = -3322;
               particleType = "AntiBaryon";
               particleName = name;
               baryon = -1;
               strangeness  = 2;
          }        
        else if(name == "AntiXiMinus")
          {
               mass = 1.32132;
               charge = 1.;
               code = -3312;
               particleType = "AntiBaryon";
               particleName = name;
               baryon       = -1;
               strangeness  = 2;
          }
        else if(name == "OmegaMinus")
          {
               mass = 1.67245;
               charge = -1.;
               code = 3334;
               particleType = "Baryon";
               particleName = name;
               baryon       = 1;
               strangeness  = -3;
          }        
        else if(name == "AntiOmegaMinus")
          {
               mass = 1.67245;
               charge = 1.;
               code = -3334;
               particleType = "AntiBaryon";
               particleName = name;
               baryon       = -1;
               strangeness  = 3;
          }
        else if(name == "Deuteron")
          {
               mass = 1.875613;
               charge = 1.;
               code = 0;
               particleType = "Nucleus";
               particleName = name;
               baryon       = 2;
               strangeness  = 0;
          }        
        else if(name == "Triton")
          {
               mass = 2.80925;
               charge = 1.;
               code = 0;
               particleType = "Nucleus";
               particleName = name;
               baryon       = 3;
               strangeness  = 0;
          }
        else if(name == "Alpha")
          {
               mass = 3.727417;
               charge = 2.;
               code = 0;
               particleType = "Nucleus";
               particleName = name;
               baryon       = 4;
               strangeness  = 0;
          }
        else if(name == "Gamma")
          {
               mass = 0.;
               charge = 0.;
               code = 22;
               particleType = "Boson";
               particleName = name;
               baryon = 0;
               strangeness  = 0;
          }
        else
          {
               G4cout << "particle " << name << " not known in this generator!!" << G4endl;
               return;
          }
        px = 0.;
        py = 0.;
        pz = 0.;
        kineticEnergy = 0.;
        energy = mass;
        timeOfFlight = 0.;
        side = 0;
        flag = false;
        return;
   } 

G4int G4HEVector::FillQuarkContent()
      //  calculate quark and anti-quark contents
      //  return value is PDG encoding for this particle.
      //  It means error if the return value is differnt from
      //  this->thePDGEncoding.
{
  G4int tempPDGcode = code;
  G4double eplus = 1.;

  for (G4int flavor=0; flavor<NumberOfQuarkFlavor; flavor++){
    theQuarkContent[flavor] =0;
    theAntiQuarkContent[flavor] =0;
  }

  G4int temp = std::abs(tempPDGcode);
  G4int multiplet = temp/10000;
  temp -= G4int(multiplet*10000);
  G4int quark1 = temp/1000;
  temp -= G4int(quark1*1000);
  G4int quark2 = temp/100;
  temp -= G4int(quark2*100);
  G4int quark3 = temp/10;
  temp -= G4int(quark3*10);

  // G4int spin= (temp-1);  DHW 19 May 2011: variable set but not used

  if (particleType =="quark") {
    if (tempPDGcode>0){
      if (tempPDGcode<=NumberOfQuarkFlavor){
	theQuarkContent[tempPDGcode-1] =1;
      } else {
	//  --- thePDGEncoding is wrong 
	tempPDGcode = 0;
      }
    } else {
      G4int temp = -1*tempPDGcode; 
      if (temp<=NumberOfQuarkFlavor){
	theAntiQuarkContent[temp-1] =1;
      } else {
	//  --- thePDGEncoding is wrong 
	tempPDGcode = 0;
      }
    }

  } else if (particleType == "Meson") {
    //   -- exceptions --
    // if (tempPDGcode == 310) spin = 0;        //K0s
    // DHW 19 May 2011: variable set but not used

    if (tempPDGcode == 130) {     //K0l
      // spin = 0;   DHW 19 May 2011: variable set but not used        
      quark2 = 3;
      quark3 = 1;
    }

    if (quark1 !=0) 
     { 
       tempPDGcode = 0; 
     } 
    if ((quark2==0)||(quark3==0)){ 
      tempPDGcode = 0;
     }
    if (quark2<quark3) { 
      tempPDGcode = 0;
    }
    // check quark flavor
    if (quark2>=NumberOfQuarkFlavor){
      tempPDGcode = 0;
    }
    // check heavier quark type
    if (quark2 & 1) {
      // down type qurak
      if (tempPDGcode >0) {
        theQuarkContent[quark3-1] =1;
        theAntiQuarkContent[quark2-1] =1;
      } else {
        theQuarkContent[quark2-1] =1;
        theAntiQuarkContent[quark3-1] =1;
      }
    } else {
      // up type quark
      if (tempPDGcode >0) {
        theQuarkContent[quark2-1] =1;
        theAntiQuarkContent[quark3-1] =1;
      } else {
        theQuarkContent[quark3-1] =1;
        theAntiQuarkContent[quark2-1] =1;
      }
    }
    // check charge
    G4double totalCharge = 0.0;
    for (G4int flavor= 0; flavor<NumberOfQuarkFlavor-1; flavor+=2){
      totalCharge += (-1./3.)*eplus*theQuarkContent[flavor];
      totalCharge += 1./3.*eplus*theAntiQuarkContent[flavor];
      totalCharge += 2./3.*eplus*theQuarkContent[flavor+1];
      totalCharge += (-2./3.)*eplus*theAntiQuarkContent[flavor+1];
    }
    if (std::abs(totalCharge-charge)>0.1*eplus) { 
      tempPDGcode = 0;
    }
  } else if (particleType == "baryon"){
    // check Meson or not
    if ((quark1==0)||(quark2==0)||(quark3==0)){ 
      tempPDGcode = 0;
    }
    //exceptions
    if (std::abs(tempPDGcode) == 3122) { 
      // Lambda
      quark2 = 2;
      quark3 = 1;
      // spin = 1;  DHW 19 May 2011: variable set but not used
    } else if (std::abs(tempPDGcode) == 4122) { 
      // Lambda_c
      quark2 = 2;
      quark3 = 1;
      // spin = 1;  DHW 19 May 2011: variable set but not used
    } 
    // check quark flavor
    if ((quark1<quark2)||(quark2<quark3)||(quark1<quark3)) { 
      tempPDGcode = 0;
    }
    if (quark1>=NumberOfQuarkFlavor) {
      tempPDGcode = 0;
    }
    if (tempPDGcode >0) {
      theQuarkContent[quark1-1] ++;
      theQuarkContent[quark2-1] ++;
      theQuarkContent[quark3-1] ++;
    } else {
      theAntiQuarkContent[quark1-1] ++;
      theAntiQuarkContent[quark2-1] ++;
      theAntiQuarkContent[quark3-1] ++;
    }
    // check charge 
    G4double totalCharge = 0.0;
    for (G4int flavor= 0; flavor<NumberOfQuarkFlavor-1; flavor+=2){
      totalCharge += (-1./3.)*eplus*theQuarkContent[flavor];
      totalCharge += 1./3.*eplus*theAntiQuarkContent[flavor];
      totalCharge += 2./3.*eplus*theQuarkContent[flavor+1];
      totalCharge += (-2./3.)*eplus*theAntiQuarkContent[flavor+1];
    }
    if (std::abs(totalCharge-charge)>0.1*eplus) { 
      tempPDGcode = 0;
    }
  } else {
  }
  return tempPDGcode;
}

void G4HEVector::Print(G4int L) const
{
  G4cout << "HEV: " 
         << L << " " << px << " " <<  py << " " <<  pz << " "
         << energy << " " << mass << " " << charge << " " 
         << timeOfFlight << " " << side << " " << flag << " " 
         << code << " " << baryon << " " << particleName << G4endl;
  return;                         
}
