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
// 14.03.07 V. Grichine - first implementation
//

#include "G4HadronNucleonXsc.hh"

#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4HadTmpUtil.hh"


G4HadronNucleonXsc::G4HadronNucleonXsc() 
: fUpperLimit( 10000 * GeV ),
  fLowerLimit( 0.03 * MeV ),
  fTotalXsc(0.0), fElasticXsc(0.0), fInelasticXsc(0.0), fHadronNucleonXsc(0.0)
{
  theGamma    = G4Gamma::Gamma();
  theProton   = G4Proton::Proton();
  theNeutron  = G4Neutron::Neutron();
  theAProton  = G4AntiProton::AntiProton();
  theANeutron = G4AntiNeutron::AntiNeutron();
  thePiPlus   = G4PionPlus::PionPlus();
  thePiMinus  = G4PionMinus::PionMinus();
  thePiZero   = G4PionZero::PionZero();
  theKPlus    = G4KaonPlus::KaonPlus();
  theKMinus   = G4KaonMinus::KaonMinus();
  theK0S      = G4KaonZeroShort::KaonZeroShort();
  theK0L      = G4KaonZeroLong::KaonZeroLong();
  theL        = G4Lambda::Lambda();
  theAntiL    = G4AntiLambda::AntiLambda();
  theSPlus    = G4SigmaPlus::SigmaPlus();
  theASPlus   = G4AntiSigmaPlus::AntiSigmaPlus();
  theSMinus   = G4SigmaMinus::SigmaMinus();
  theASMinus  = G4AntiSigmaMinus::AntiSigmaMinus();
  theS0       = G4SigmaZero::SigmaZero();
  theAS0      = G4AntiSigmaZero::AntiSigmaZero();
  theXiMinus  = G4XiMinus::XiMinus();
  theXi0      = G4XiZero::XiZero();
  theAXiMinus = G4AntiXiMinus::AntiXiMinus();
  theAXi0     = G4AntiXiZero::AntiXiZero();
  theOmega    = G4OmegaMinus::OmegaMinus();
  theAOmega   = G4AntiOmegaMinus::AntiOmegaMinus();
  theD        = G4Deuteron::Deuteron();
  theT        = G4Triton::Triton();
  theA        = G4Alpha::Alpha();
  theHe3      = G4He3::He3();

  InitialiseKaonNucleonTotXsc();
}


G4HadronNucleonXsc::~G4HadronNucleonXsc()
{}

void G4HadronNucleonXsc::CrossSectionDescription(std::ostream& outFile) const
{
  outFile << "G4HadronNucleonXsc calculates the total, inelastic and elastic\n"
          << "hadron-nucleon cross sections using several different\n"
          << "parameterizations within the Glauber-Gribov approach. It is\n"
          << "valid for all incident gammas and long-lived hadrons at\n"
          << "energies above 30 keV.  This is a cross section component which\n"
          << "is to be used to build a cross section data set.\n"; 
}

G4bool 
G4HadronNucleonXsc::IsApplicable(const G4DynamicParticle* aDP, 
                                 const G4Element* anElement)
{
  G4int Z = G4lrint(anElement->GetZ());
  G4int A = G4lrint(anElement->GetN());
  return IsIsoApplicable(aDP, Z, A);
} 

////////////////////////////////////////////////////////////////////////////////////////
//

G4bool 
G4HadronNucleonXsc::IsIsoApplicable(const G4DynamicParticle* aDP, 
                                    G4int Z, G4int)
{
  G4bool applicable = false;
  // G4int baryonNumber     = aDP->GetDefinition()->GetBaryonNumber();
  G4double kineticEnergy = aDP->GetKineticEnergy();

  const G4ParticleDefinition* theParticle = aDP->GetDefinition();
 
  if ( ( kineticEnergy  >= fLowerLimit &&
         Z > 1 &&      // >=  He
       ( theParticle == theAProton   ||
         theParticle == theGamma     ||
         theParticle == theKPlus     ||
         theParticle == theKMinus    || 
         theParticle == theSMinus)      )    ||  

       ( kineticEnergy  >= 0.1*fLowerLimit &&
         Z > 1 &&      // >=  He
       ( theParticle == theProton    ||
         theParticle == theNeutron   ||   
         theParticle == thePiPlus    ||
         theParticle == thePiMinus       ) )    ) applicable = true;

  return applicable;
}


/////////////////////////////////////////////////////////////////////////////////////
//
// Returns hadron-nucleon Xsc according to differnt parametrisations:
// [2] E. Levin, hep-ph/9710546
// [3] U. Dersch, et al, hep-ex/9910052
// [4] M.J. Longo, et al, Phys.Rev.Lett. 33 (1974) 725 

G4double 
G4HadronNucleonXsc::GetHadronNucleonXscEL(const G4DynamicParticle* aParticle, 
                                          const G4ParticleDefinition* nucleon )
{
  G4double xsection;


  G4double targ_mass = 0.939*GeV;  // ~mean neutron and proton ???

  G4double proj_mass     = aParticle->GetMass();
  G4double proj_momentum = aParticle->GetMomentum().mag();
  G4double sMand = CalcMandelstamS ( proj_mass , targ_mass , proj_momentum );

  sMand /= GeV*GeV;  // in GeV for parametrisation
  proj_momentum /= GeV;

  const G4ParticleDefinition* theParticle = aParticle->GetDefinition();

  G4bool pORn = (nucleon == theProton || nucleon == theNeutron  );  
  

  if(theParticle == theGamma && pORn ) 
  {
    xsection = (0.0677*std::pow(sMand,0.0808) + 0.129*std::pow(sMand,-0.4525));
  } 
  else if(theParticle == theNeutron && pORn ) // as proton ??? 
  {
    xsection = (21.70*std::pow(sMand,0.0808) + 56.08*std::pow(sMand,-0.4525));
  } 
  else if(theParticle == theProton && pORn ) 
  {
    xsection = (21.70*std::pow(sMand,0.0808) + 56.08*std::pow(sMand,-0.4525));

    // xsection = At*( 49.51*std::pow(sMand,-0.097) + 0.314*std::log(sMand)*std::log(sMand) );
    // xsection = At*( 38.4 + 0.85*std::abs(std::pow(log(sMand),1.47)) );
  } 
  else if(theParticle == theAProton && pORn ) 
  {
    xsection = ( 21.70*std::pow(sMand,0.0808) + 98.39*std::pow(sMand,-0.4525));
  } 
  else if(theParticle == thePiPlus && pORn ) 
  {
    xsection = (13.63*std::pow(sMand,0.0808) + 27.56*std::pow(sMand,-0.4525));
  } 
  else if(theParticle == thePiMinus && pORn ) 
  {
    // xsection = At*( 55.2*std::pow(sMand,-0.255) + 0.346*std::log(sMand)*std::log(sMand) );
    xsection = (13.63*std::pow(sMand,0.0808) + 36.02*std::pow(sMand,-0.4525));
  } 
  else if(theParticle == theKPlus && pORn ) 
  {
    xsection = (11.82*std::pow(sMand,0.0808) + 8.15*std::pow(sMand,-0.4525));
  } 
  else if(theParticle == theKMinus && pORn ) 
  {
    xsection = (11.82*std::pow(sMand,0.0808) + 26.36*std::pow(sMand,-0.4525));
  }
  else  // as proton ??? 
  {
    xsection = (21.70*std::pow(sMand,0.0808) + 56.08*std::pow(sMand,-0.4525));
  }
  xsection *= millibarn;

  fTotalXsc     = xsection;
  fInelasticXsc = 0.83*xsection;
  fElasticXsc   = fTotalXsc - fInelasticXsc;
  if (fElasticXsc < 0.)fElasticXsc = 0.;
 
  return xsection;
}


/////////////////////////////////////////////////////////////////////////////////////
//
// Returns hadron-nucleon Xsc according to PDG parametrisation (2005):
// http://pdg.lbl.gov/2006/reviews/hadronicrpp.pdf
//  At = number of nucleons,  Zt = number of protons 

G4double 
G4HadronNucleonXsc::GetHadronNucleonXscPDG(const G4DynamicParticle* aParticle, 
                                           const G4ParticleDefinition* nucleon )
{
  G4double xsection(0);
  G4int Zt=1, Nt=1, At=1;

   G4double targ_mass = 0.939*GeV;  // ~mean neutron and proton ???

  G4double proj_mass     = aParticle->GetMass(); 
  G4double proj_momentum = aParticle->GetMomentum().mag();

  G4double sMand = CalcMandelstamS ( proj_mass , targ_mass , proj_momentum );

  sMand         /= GeV*GeV;  // in GeV for parametrisation

  // General PDG fit constants

  G4double s0   = 5.38*5.38; // in Gev^2
  G4double eta1 = 0.458;
  G4double eta2 = 0.458;
  G4double B    = 0.308;

  const G4ParticleDefinition* theParticle = aParticle->GetDefinition();

  G4bool pORn = (nucleon == theProton || nucleon == theNeutron  );  
  G4bool proton = (nucleon == theProton);
  G4bool neutron = (nucleon == theNeutron);
  
  if(theParticle == theNeutron) // proton-neutron fit 
  {
    if ( proton )
    {
      xsection = Zt*( 35.80 + B*std::pow(std::log(sMand/s0),2.) 
		 + 40.15*std::pow(sMand,-eta1) - 30.*std::pow(sMand,-eta2));// on p
    }
    if ( neutron )
    {
      xsection  = Nt*( 35.45 + B*std::pow(std::log(sMand/s0),2.) 
		      + 42.53*std::pow(sMand,-eta1) - 33.34*std::pow(sMand,-eta2)); // on n pp for nn
    }
  } 
  else if(theParticle == theProton) 
  {
    if ( proton )
    {      
      xsection  = Zt*( 35.45 + B*std::pow(std::log(sMand/s0),2.) 
                          + 42.53*std::pow(sMand,-eta1) - 33.34*std::pow(sMand,-eta2));
    }
    if ( neutron )
    {
      xsection = Nt*( 35.80 + B*std::pow(std::log(sMand/s0),2.) 
                          + 40.15*std::pow(sMand,-eta1) - 30.*std::pow(sMand,-eta2));
    }
  } 
  else if(theParticle == theAProton) 
  {
    if ( proton )
    {      
      xsection  = Zt*( 35.45 + B*std::pow(std::log(sMand/s0),2.) 
                          + 42.53*std::pow(sMand,-eta1) + 33.34*std::pow(sMand,-eta2));
    }
    if ( neutron )
    {
      xsection = Nt*( 35.80 + B*std::pow(std::log(sMand/s0),2.) 
                          + 40.15*std::pow(sMand,-eta1) + 30.*std::pow(sMand,-eta2));
    }
  } 
  else if(theParticle == thePiPlus && pORn ) 
  {
    xsection  = At*( 20.86 + B*std::pow(std::log(sMand/s0),2.) 
                          + 19.24*std::pow(sMand,-eta1) - 6.03*std::pow(sMand,-eta2));
  } 
  else if(theParticle == thePiMinus && pORn ) 
  {
    xsection  = At*( 20.86 + B*std::pow(std::log(sMand/s0),2.) 
                          + 19.24*std::pow(sMand,-eta1) + 6.03*std::pow(sMand,-eta2));
  } 
  else if(theParticle == theKPlus) 
  {
    if ( proton )
    {      
      xsection  = Zt*( 17.91 + B*std::pow(std::log(sMand/s0),2.) 
                          + 7.14*std::pow(sMand,-eta1) - 13.45*std::pow(sMand,-eta2));
    }
    if ( neutron )
    {
      xsection = Nt*( 17.87 + B*std::pow(std::log(sMand/s0),2.) 
                          + 5.17*std::pow(sMand,-eta1) - 7.23*std::pow(sMand,-eta2));
    }
  } 
  else if(theParticle == theKMinus) 
  {
    if ( proton )
    {      
      xsection  = Zt*( 17.91 + B*std::pow(std::log(sMand/s0),2.) 
                          + 7.14*std::pow(sMand,-eta1) + 13.45*std::pow(sMand,-eta2));
    }
    if ( neutron )
    {
      xsection = Nt*( 17.87 + B*std::pow(std::log(sMand/s0),2.) 
                          + 5.17*std::pow(sMand,-eta1) + 7.23*std::pow(sMand,-eta2) );
    }
  }
  else if(theParticle == theSMinus && pORn ) 
  {
    xsection  = At*( 35.20 + B*std::pow(std::log(sMand/s0),2.) 
                          - 199.*std::pow(sMand,-eta1) + 264.*std::pow(sMand,-eta2) );
  } 
  else if(theParticle == theGamma && pORn ) // modify later on
  {
    xsection  = At*( 0.0 + B*std::pow(std::log(sMand/s0),2.) 
                          + 0.032*std::pow(sMand,-eta1) - 0.0*std::pow(sMand,-eta2) );
   
  } 
  else  // as proton ??? 
  {
    if ( proton )
    {      
      xsection  = Zt*( 35.45 + B*std::pow(std::log(sMand/s0),2.) 
                       + 42.53*std::pow(sMand,-eta1) - 33.34*std::pow(sMand,-eta2) );
    }
    if ( neutron )
    {
      xsection = Nt*( 35.80 + B*std::pow(std::log(sMand/s0),2.) 
                      + 40.15*std::pow(sMand,-eta1) - 30.*std::pow(sMand,-eta2));
    }
  } 
  xsection *= millibarn; // parametrised in mb

  fTotalXsc     = xsection;
  fInelasticXsc = 0.75*xsection;
  fElasticXsc   = fTotalXsc - fInelasticXsc;
  if (fElasticXsc < 0.) fElasticXsc = 0.;

  return xsection;
}


/////////////////////////////////////////////////////////////////////////////////////
//
// Returns hadron-nucleon cross-section based on N. Starkov parametrisation of
// data from mainly http://wwwppds.ihep.su:8001/c5-6A.html database

G4double 
G4HadronNucleonXsc::GetHadronNucleonXscNS(const G4DynamicParticle* aParticle, 
                                          const G4ParticleDefinition* nucleon  )
{
  G4double xsection(0);
  // G4double Delta;  DHW 19 May 2011: variable set but not used
  G4double A0, B0;
  G4int Zt=1, Nt=1, At=1;
  G4double hpXsc(0);
  G4double hnXsc(0);


  G4double targ_mass = 0.939*GeV;  // ~mean neutron and proton ???

  G4double proj_mass     = aParticle->GetMass();
  G4double proj_energy   = aParticle->GetTotalEnergy(); 
  G4double proj_momentum = aParticle->GetMomentum().mag();

  G4double sMand = CalcMandelstamS ( proj_mass , targ_mass , proj_momentum );

  sMand         /= GeV*GeV;  // in GeV for parametrisation
  proj_momentum /= GeV;
  proj_energy   /= GeV;
  proj_mass     /= GeV;

  // General PDG fit constants

  G4double s0   = 5.38*5.38; // in Gev^2
  G4double eta1 = 0.458;
  G4double eta2 = 0.458;
  G4double B    = 0.308;


  const G4ParticleDefinition* theParticle = aParticle->GetDefinition();

  G4bool pORn = (nucleon == theProton || nucleon == theNeutron  );  
  G4bool proton = (nucleon == theProton);
  G4bool neutron = (nucleon == theNeutron);

  if( theParticle == theNeutron && pORn) 
  {
    if( proj_momentum >= 373.)
    {
      return GetHadronNucleonXscPDG(aParticle,nucleon);
    }
    else if( proj_momentum >= 10.)
    // if( proj_momentum >= 2.)
    {
      // Delta = 1.;   DHW 19 May 2011: variable set but not used
      // if( proj_energy < 40. ) Delta = 0.916+0.0021*proj_energy;

      if(proj_momentum >= 10.)
      {
        B0 = 7.5;
        A0 = 100. - B0*std::log(3.0e7);

        xsection = A0 + B0*std::log(proj_energy) - 11
                  + 103*std::pow(2*0.93827*proj_energy + proj_mass*proj_mass+
                     0.93827*0.93827,-0.165);        //  mb
      }
      fTotalXsc = xsection;
    }
    else
    {
      // nn to be pp

      if(neutron)
      {
        if( proj_momentum < 0.73 )
        {
          hnXsc = 23 + 50*( std::pow( std::log(0.73/proj_momentum), 3.5 ) );
        }
        else if( proj_momentum < 1.05  )
        {
          hnXsc = 23 + 40*(std::log(proj_momentum/0.73))*
                         (std::log(proj_momentum/0.73));
        }
        else  // if( proj_momentum < 10.  )
        {
          hnXsc = 39.0+
              75*(proj_momentum - 1.2)/(std::pow(proj_momentum,3.0) + 0.15);
        }
        fTotalXsc = hnXsc;
      }
      // pn to be np

      if(proton)
      {
        if( proj_momentum < 0.8 )
        {
          hpXsc = 33+30*std::pow(std::log(proj_momentum/1.3),4.0);
        }      
        else if( proj_momentum < 1.4 )
        {
          hpXsc = 33+30*std::pow(std::log(proj_momentum/0.95),2.0);
        }
        else    // if( proj_momentum < 10.  )
        {
          hpXsc = 33.3+
              20.8*(std::pow(proj_momentum,2.0)-1.35)/
                 (std::pow(proj_momentum,2.50)+0.95);
        }
        fTotalXsc = hpXsc;
      }
      // xsection = hpXsc*Zt + hnXsc*Nt;
    }
  } 
  else if(theParticle == theProton && pORn) 
  {
    if( proj_momentum >= 373.)
    {
      return GetHadronNucleonXscPDG(aParticle,nucleon);
    }
    else if( proj_momentum >= 10.)
    // if( proj_momentum >= 2.)
    {
      // Delta = 1.;  DHW 19 May 2011: variable set but not used
      // if( proj_energy < 40. ) Delta = 0.916+0.0021*proj_energy;

      if(proj_momentum >= 10.)
      {
        B0 = 7.5;
        A0 = 100. - B0*std::log(3.0e7);

        xsection = A0 + B0*std::log(proj_energy) - 11
                  + 103*std::pow(2*0.93827*proj_energy + proj_mass*proj_mass+
                     0.93827*0.93827,-0.165);        //  mb
      }
      fTotalXsc = xsection;
    }
    else
    {
      // pp

      if(proton)
      {
        if( proj_momentum < 0.73 )
        {
          hpXsc = 23 + 50*( std::pow( std::log(0.73/proj_momentum), 3.5 ) );
        }
        else if( proj_momentum < 1.05  )
        {
          hpXsc = 23 + 40*(std::log(proj_momentum/0.73))*
                         (std::log(proj_momentum/0.73));
        }
        else    // if( proj_momentum < 10.  )
        {
           hpXsc = 39.0+
              75*(proj_momentum - 1.2)/(std::pow(proj_momentum,3.0) + 0.15);
        }
        fTotalXsc = hpXsc;
      }
      // pn to be np

      if(neutron)
      {
        if( proj_momentum < 0.8 )
        {
          hnXsc = 33+30*std::pow(std::log(proj_momentum/1.3),4.0);
        }      
        else if( proj_momentum < 1.4 )
        {
          hnXsc = 33+30*std::pow(std::log(proj_momentum/0.95),2.0);
        }
        else   // if( proj_momentum < 10.  )
        {
          hnXsc = 33.3+
              20.8*(std::pow(proj_momentum,2.0)-1.35)/
                 (std::pow(proj_momentum,2.50)+0.95);
        }
        fTotalXsc = hnXsc;
      }
      // xsection = hpXsc*Zt + hnXsc*Nt;
      // xsection = hpXsc*(Zt + Nt);
      // xsection = hnXsc*(Zt + Nt);
    }    
    // xsection *= 0.95;
  } 
  else if(theParticle == theAProton && pORn) 
  {
    if(proton)
    {
      xsection  = Zt*( 35.45 + B*std::pow(std::log(sMand/s0),2.) 
                          + 42.53*std::pow(sMand,-eta1) + 33.34*std::pow(sMand,-eta2));
    }
    if(proton)
    {
      xsection = Nt*( 35.80 + B*std::pow(std::log(sMand/s0),2.) 
                          + 40.15*std::pow(sMand,-eta1) + 30.*std::pow(sMand,-eta2));
    }
    fTotalXsc = xsection;
  } 
  else if(theParticle == thePiPlus && pORn) 
  {
    if(proton)
    {
      if(proj_momentum < 0.4)
      {
        G4double Ex3 = 180*std::exp(-(proj_momentum-0.29)*(proj_momentum-0.29)/0.085/0.085);
        hpXsc      = Ex3+20.0;
      }
      else if(proj_momentum < 1.15)
      {
        G4double Ex4 = 88*(std::log(proj_momentum/0.75))*(std::log(proj_momentum/0.75));
        hpXsc = Ex4+14.0;
      }
      else if(proj_momentum < 3.5)
      {
        G4double Ex1 = 3.2*std::exp(-(proj_momentum-2.55)*(proj_momentum-2.55)/0.55/0.55);
        G4double Ex2 = 12*std::exp(-(proj_momentum-1.47)*(proj_momentum-1.47)/0.225/0.225);
        hpXsc = Ex1+Ex2+27.5;
      }
      else //  if(proj_momentum > 3.5) // mb
      {
        hpXsc = 10.6+2.*std::log(proj_energy)+25*std::pow(proj_energy,-0.43);
      }
      fTotalXsc = hpXsc;
    }    

// pi+n = pi-p??

    if(neutron)
    {
      if(proj_momentum < 0.37)
      {
        hnXsc = 28.0 + 40*std::exp(-(proj_momentum-0.29)*(proj_momentum-0.29)/0.07/0.07);
      }
      else if(proj_momentum<0.65)
      {
        hnXsc = 26+110*(std::log(proj_momentum/0.48))*(std::log(proj_momentum/0.48));
      }
      else if(proj_momentum<1.3)
      {
        hnXsc = 36.1+
                10*std::exp(-(proj_momentum-0.72)*(proj_momentum-0.72)/0.06/0.06)+
                24*std::exp(-(proj_momentum-1.015)*(proj_momentum-1.015)/0.075/0.075);
      }
      else if(proj_momentum<3.0)
      {
        hnXsc = 36.1+0.079-4.313*std::log(proj_momentum)+
                3*std::exp(-(proj_momentum-2.1)*(proj_momentum-2.1)/0.4/0.4)+
                1.5*std::exp(-(proj_momentum-1.4)*(proj_momentum-1.4)/0.12/0.12);
      }
      else   // mb
      {
        hnXsc = 10.6+2*std::log(proj_energy)+30*std::pow(proj_energy,-0.43); 
      }
      fTotalXsc = hnXsc;
    }
    // xsection = hpXsc*Zt + hnXsc*Nt;
  } 
  else if(theParticle == thePiMinus && pORn) 
  {
    // pi-n = pi+p??

    if(neutron)
    {
      if(proj_momentum < 0.4)
      {
        G4double Ex3 = 180*std::exp(-(proj_momentum-0.29)*(proj_momentum-0.29)/0.085/0.085);
        hnXsc      = Ex3+20.0;
      }
      else if(proj_momentum < 1.15)
      {
        G4double Ex4 = 88*(std::log(proj_momentum/0.75))*(std::log(proj_momentum/0.75));
        hnXsc = Ex4+14.0;
      }
      else if(proj_momentum < 3.5)
      {
        G4double Ex1 = 3.2*std::exp(-(proj_momentum-2.55)*(proj_momentum-2.55)/0.55/0.55);
        G4double Ex2 = 12*std::exp(-(proj_momentum-1.47)*(proj_momentum-1.47)/0.225/0.225);
        hnXsc = Ex1+Ex2+27.5;
      }
      else //  if(proj_momentum > 3.5) // mb
      {
      hnXsc = 10.6+2.*std::log(proj_energy)+25*std::pow(proj_energy,-0.43);
      }
      fTotalXsc = hnXsc;
    }
    // pi-p

    if(proton)
    {
      if(proj_momentum < 0.37)
      {
        hpXsc = 28.0 + 40*std::exp(-(proj_momentum-0.29)*(proj_momentum-0.29)/0.07/0.07);
      }
      else if(proj_momentum<0.65)
      {
        hpXsc = 26+110*(std::log(proj_momentum/0.48))*(std::log(proj_momentum/0.48));
      }
      else if(proj_momentum<1.3)
      {
        hpXsc = 36.1+
                10*std::exp(-(proj_momentum-0.72)*(proj_momentum-0.72)/0.06/0.06)+
                24*std::exp(-(proj_momentum-1.015)*(proj_momentum-1.015)/0.075/0.075);
      }
      else if(proj_momentum<3.0)
      {
        hpXsc = 36.1+0.079-4.313*std::log(proj_momentum)+
                3*std::exp(-(proj_momentum-2.1)*(proj_momentum-2.1)/0.4/0.4)+
                1.5*std::exp(-(proj_momentum-1.4)*(proj_momentum-1.4)/0.12/0.12);
      }
      else   // mb
      {
        hpXsc = 10.6+2*std::log(proj_energy)+30*std::pow(proj_energy,-0.43); 
      }
      fTotalXsc = hpXsc;
    }
    // xsection = hpXsc*Zt + hnXsc*Nt;
  } 
  else if(theParticle == theKPlus && pORn) 
  {
    if(proton)
    {
      xsection  = Zt*( 17.91 + B*std::pow(std::log(sMand/s0),2.) 
                          + 7.14*std::pow(sMand,-eta1) - 13.45*std::pow(sMand,-eta2));
    }
    if(neutron)
    {
      xsection = Nt*( 17.87 + B*std::pow(std::log(sMand/s0),2.) 
                          + 5.17*std::pow(sMand,-eta1) - 7.23*std::pow(sMand,-eta2));
    }
    fTotalXsc = xsection;
  } 
  else if(theParticle == theKMinus && pORn) 
  {
    if(proton)
    {
      xsection  = Zt*( 17.91 + B*std::pow(std::log(sMand/s0),2.) 
                          + 7.14*std::pow(sMand,-eta1) + 13.45*std::pow(sMand,-eta2));
    }
    if(neutron)
    {
      xsection = Nt*( 17.87 + B*std::pow(std::log(sMand/s0),2.) 
                          + 5.17*std::pow(sMand,-eta1) + 7.23*std::pow(sMand,-eta2));
    }
    fTotalXsc = xsection;
  }
  else if(theParticle == theSMinus && pORn) 
  {
    xsection  = At*( 35.20 + B*std::pow(std::log(sMand/s0),2.) 
                          - 199.*std::pow(sMand,-eta1) + 264.*std::pow(sMand,-eta2));
  } 
  else if(theParticle == theGamma && pORn) // modify later on
  {
    xsection  = At*( 0.0 + B*std::pow(std::log(sMand/s0),2.) 
                          + 0.032*std::pow(sMand,-eta1) - 0.0*std::pow(sMand,-eta2));
    fTotalXsc = xsection;   
  } 
  else  // as proton ??? 
  {
    if(proton)
    {
      xsection  = Zt*( 35.45 + B*std::pow(std::log(sMand/s0),2.) 
                          + 42.53*std::pow(sMand,-eta1) - 33.34*std::pow(sMand,-eta2));
    }
    if(neutron)
    {
      xsection += Nt*( 35.80 + B*std::pow(std::log(sMand/s0),2.) 
                          + 40.15*std::pow(sMand,-eta1) - 30.*std::pow(sMand,-eta2));
    }
    fTotalXsc = xsection;
  } 
  fTotalXsc *= millibarn; // parametrised in mb
  // xsection  *= millibarn; // parametrised in mb

  fInelasticXsc = 0.83*fTotalXsc;
  fElasticXsc   = fTotalXsc - fInelasticXsc;
  if (fElasticXsc < 0.)fElasticXsc = 0.;

  return fTotalXsc;
}

/////////////////////////////////////////////////////////////////////////////////////
//
// Returns hadron-nucleon cross-section based on V. Uzjinsky parametrisation of
// data from G4FTFCrossSection class

G4double 
G4HadronNucleonXsc::GetHadronNucleonXscVU(const G4DynamicParticle* aParticle, 
                                          const G4ParticleDefinition* nucleon  )
{
  G4int PDGcode = aParticle->GetDefinition()->GetPDGEncoding();
  G4int absPDGcode = std::abs(PDGcode);
  G4double Elab = aParticle->GetTotalEnergy();              
                          // (s - 2*0.88*GeV*GeV)/(2*0.939*GeV)/GeV;
  G4double Plab = aParticle->GetMomentum().mag();            
                          // std::sqrt(Elab * Elab - 0.88);

  Elab /= GeV;
  Plab /= GeV;

  G4double LogPlab = std::log( Plab );
  G4double sqrLogPlab = LogPlab * LogPlab;

  G4bool pORn = (nucleon == theProton || nucleon == theNeutron  );  
  G4bool proton = (nucleon == theProton);
  G4bool neutron = (nucleon == theNeutron);

   
  if( absPDGcode > 1000 && pORn )  //------Projectile is baryon -
  {
    if(proton)
    {
      fTotalXsc   = 48.0 +  0. *std::pow(Plab, 0.  ) + 0.522*sqrLogPlab - 4.51*LogPlab;
      fElasticXsc = 11.9 + 26.9*std::pow(Plab,-1.21) + 0.169*sqrLogPlab - 1.85*LogPlab;
    }
    if(neutron)
    {    
      fTotalXsc   = 47.3 +  0. *std::pow(Plab, 0.  ) + 0.513*sqrLogPlab - 4.27*LogPlab;
      fElasticXsc = 11.9 + 26.9*std::pow(Plab,-1.21) + 0.169*sqrLogPlab - 1.85*LogPlab;
    }
  }
  else if( PDGcode ==  211  && pORn )  //------Projectile is PionPlus ----
  {
    if(proton)
    {
      fTotalXsc  = 16.4 + 19.3 *std::pow(Plab,-0.42) + 0.19 *sqrLogPlab - 0.0 *LogPlab;
      fElasticXsc =  0.0 + 11.4*std::pow(Plab,-0.40) + 0.079*sqrLogPlab - 0.0 *LogPlab;
    }
    if(neutron)
    {    
      fTotalXsc   =  33.0 + 14.0 *std::pow(Plab,-1.36) + 0.456*sqrLogPlab - 4.03*LogPlab;
      fElasticXsc = 1.76 + 11.2*std::pow(Plab,-0.64) + 0.043*sqrLogPlab - 0.0 *LogPlab;
    }
  }
  else if( PDGcode == -211  && pORn )  //------Projectile is PionMinus ----
  {
    if(proton)
    {
      fTotalXsc   = 33.0 + 14.0 *std::pow(Plab,-1.36) + 0.456*sqrLogPlab - 4.03*LogPlab;
      fElasticXsc = 1.76 + 11.2*std::pow(Plab,-0.64) + 0.043*sqrLogPlab - 0.0 *LogPlab;
    }
    if(neutron)
    {    
      fTotalXsc   = 16.4 + 19.3 *std::pow(Plab,-0.42) + 0.19 *sqrLogPlab - 0.0 *LogPlab;
      fElasticXsc =  0.0 + 11.4*std::pow(Plab,-0.40) + 0.079*sqrLogPlab - 0.0 *LogPlab;
    }
  }
  else if( PDGcode ==  111  && pORn )  //------Projectile is PionZero  --
  {
    if(proton)
    {
      fTotalXsc   = (16.4 + 19.3 *std::pow(Plab,-0.42) + 0.19 *sqrLogPlab - 0.0 *LogPlab +   //Pi+
                        33.0 + 14.0 *std::pow(Plab,-1.36) + 0.456*sqrLogPlab - 4.03*LogPlab)/2; //Pi-

      fElasticXsc = ( 0.0 + 11.4*std::pow(Plab,-0.40) + 0.079*sqrLogPlab - 0.0 *LogPlab +    //Pi+
                         1.76 + 11.2*std::pow(Plab,-0.64) + 0.043*sqrLogPlab - 0.0 *LogPlab)/2; //Pi-

    }
    if(neutron)
    {    
      fTotalXsc   = (33.0 + 14.0 *std::pow(Plab,-1.36) + 0.456*sqrLogPlab - 4.03*LogPlab +   //Pi+
                        16.4 + 19.3 *std::pow(Plab,-0.42) + 0.19 *sqrLogPlab - 0.0 *LogPlab)/2; //Pi-
      fElasticXsc = ( 1.76 + 11.2*std::pow(Plab,-0.64) + 0.043*sqrLogPlab - 0.0 *LogPlab +   //Pi+
                         0.0  + 11.4*std::pow(Plab,-0.40) + 0.079*sqrLogPlab - 0.0 *LogPlab)/2; //Pi-
    }
  }
  else if( PDGcode == 321  && pORn )    //------Projectile is KaonPlus --
  {
    if(proton)
    {
      fTotalXsc   = 18.1 +  0. *std::pow(Plab, 0.  ) + 0.26 *sqrLogPlab - 1.0 *LogPlab;
      fElasticXsc =  5.0 +  8.1*std::pow(Plab,-1.8 ) + 0.16 *sqrLogPlab - 1.3 *LogPlab;
    }
    if(neutron)
    {    
      fTotalXsc   = 18.7 +  0. *std::pow(Plab, 0.  ) + 0.21 *sqrLogPlab - 0.89*LogPlab;
      fElasticXsc =  7.3 +  0. *std::pow(Plab,-0.  ) + 0.29 *sqrLogPlab - 2.4 *LogPlab;
    }
  }
  else if( PDGcode ==-321  && pORn )  //------Projectile is KaonMinus ----
  {
    if(proton)
    {
      fTotalXsc   = 32.1 +  0. *std::pow(Plab, 0.  ) + 0.66*sqrLogPlab - 5.6*LogPlab;
      fElasticXsc =  7.3 +  0. *std::pow(Plab,-0.  ) + 0.29*sqrLogPlab - 2.4*LogPlab;
    }
    if(neutron)
    {    
      fTotalXsc   = 25.2 +  0. *std::pow(Plab, 0.  ) + 0.38*sqrLogPlab - 2.9*LogPlab;
      fElasticXsc =  5.0 +  8.1*std::pow(Plab,-1.8 ) + 0.16*sqrLogPlab - 1.3*LogPlab;
    }
  }
  else if( PDGcode == 311  && pORn )  //------Projectile is KaonZero -----
  {
    if(proton)
    {
      fTotalXsc   = ( 18.1 +  0. *std::pow(Plab, 0.  ) + 0.26 *sqrLogPlab - 1.0 *LogPlab +   //K+
                        32.1 +  0. *std::pow(Plab, 0.  ) + 0.66 *sqrLogPlab - 5.6 *LogPlab)/2; //K-
      fElasticXsc = (  5.0 +  8.1*std::pow(Plab,-1.8 ) + 0.16 *sqrLogPlab - 1.3 *LogPlab +   //K+
                         7.3 +  0. *std::pow(Plab,-0.  ) + 0.29 *sqrLogPlab - 2.4 *LogPlab)/2; //K-
    }
    if(neutron)
    {    
      fTotalXsc   = ( 18.7 +  0. *std::pow(Plab, 0.  ) + 0.21 *sqrLogPlab - 0.89*LogPlab +   //K+
                         25.2 +  0. *std::pow(Plab, 0.  ) + 0.38 *sqrLogPlab - 2.9 *LogPlab)/2; //K-
      fElasticXsc = (  7.3 +  0. *std::pow(Plab,-0.  ) + 0.29 *sqrLogPlab - 2.4 *LogPlab +   //K+
                         5.0 +  8.1*std::pow(Plab,-1.8 ) + 0.16 *sqrLogPlab - 1.3 *LogPlab)/2; //K-
    }
  }
  else  //------Projectile is undefined, Nucleon assumed
  {
    if(proton)
    {
      fTotalXsc   = 48.0 +  0. *std::pow(Plab, 0.  ) + 0.522*sqrLogPlab - 4.51*LogPlab;
      fElasticXsc = 11.9 + 26.9*std::pow(Plab,-1.21) + 0.169*sqrLogPlab - 1.85*LogPlab;
    }
    if(neutron)
    {    
      fTotalXsc   = 47.3 +  0. *std::pow(Plab, 0.  ) + 0.513*sqrLogPlab - 4.27*LogPlab;
      fElasticXsc = 11.9 + 26.9*std::pow(Plab,-1.21) + 0.169*sqrLogPlab - 1.85*LogPlab;
    }
  }
  fTotalXsc   *= millibarn;
  fElasticXsc *= millibarn;
  fInelasticXsc   = fTotalXsc - fElasticXsc;
  if (fInelasticXsc < 0.) fInelasticXsc = 0.;

  return fTotalXsc;    
}

////////////////////////////////////////////////////////////////////////////////////
//
//

G4double G4HadronNucleonXsc::CalculateEcmValue( const G4double mp , 
                                                const G4double mt , 
                                                const G4double Plab )
{
  G4double Elab = std::sqrt ( mp * mp + Plab * Plab );
  G4double Ecm  = std::sqrt ( mp * mp + mt * mt + 2 * Elab * mt );
  // G4double Pcm  = Plab * mt / Ecm;
  // G4double KEcm = std::sqrt ( Pcm * Pcm + mp * mp ) - mp;

  return Ecm ; // KEcm;
}


////////////////////////////////////////////////////////////////////////////////////
//
//

G4double G4HadronNucleonXsc::CalcMandelstamS( const G4double mp , 
                                                       const G4double mt , 
                                                       const G4double Plab )
{
  G4double Elab = std::sqrt ( mp * mp + Plab * Plab );
  G4double sMand  = mp*mp + mt*mt + 2*Elab*mt ;

  return sMand;
}

////////////////////////////////////////////////////////////////////////////////////
//
// Initialaise K(p,m)-(p,n) total cross section vectors


void G4HadronNucleonXsc::InitialiseKaonNucleonTotXsc()
{
  G4int i = 0, iMax;
  G4double tmpxsc[106];

  // Kp-proton tot xsc

  iMax = 66;
  fKpProtonTotXscVector = G4LPhysicsFreeVector(iMax, fKpProtonTotTkin[0], fKpProtonTotTkin[iMax-1]);
  fKpProtonTotXscVector.SetSpline(true);

  for( i = 0; i < iMax; i++)
  {
    tmpxsc[i] = 0.;

    if( i == 0 || i == iMax-1 ) tmpxsc[i] = fKpProtonTotXsc[i];
    else                        tmpxsc[i] = 0.5*(fKpProtonTotXsc[i-1]+fKpProtonTotXsc[i+1]);

    fKpProtonTotXscVector.PutValues(size_t(i), fKpProtonTotTkin[i], tmpxsc[i]*millibarn);
  }

  // Kp-neutron tot xsc

  iMax = 75;
  fKpNeutronTotXscVector = G4LPhysicsFreeVector(iMax, fKpNeutronTotTkin[0], fKpNeutronTotTkin[iMax-1]);
  fKpNeutronTotXscVector.SetSpline(true);

  for( i = 0; i < iMax; i++)
  {
    tmpxsc[i] = 0.;
    if( i == 0 || i == iMax-1 ) tmpxsc[i] = fKpNeutronTotXsc[i];
    else                        tmpxsc[i] = 0.5*(fKpNeutronTotXsc[i-1]+fKpNeutronTotXsc[i+1]);

    fKpNeutronTotXscVector.PutValues(size_t(i), fKpNeutronTotTkin[i], tmpxsc[i]*millibarn);
  }

  // Km-proton tot xsc

  iMax = 106;
  fKmProtonTotXscVector = G4LPhysicsFreeVector(iMax, fKmProtonTotTkin[0], fKmProtonTotTkin[iMax-1]);
  fKmProtonTotXscVector.SetSpline(true);

  for( i = 0; i < iMax; i++)
  {
    tmpxsc[i] = 0.;

    if( i == 0 || i == iMax-1 ) tmpxsc[i] = fKmProtonTotXsc[i];
    else                        tmpxsc[i] = 0.5*(fKmProtonTotXsc[i-1]+fKmProtonTotXsc[i+1]);

    fKmProtonTotXscVector.PutValues(size_t(i), fKmProtonTotTkin[i], tmpxsc[i]*millibarn);
  }

  // Km-neutron tot xsc

  iMax = 68;
  fKmNeutronTotXscVector = G4LPhysicsFreeVector(iMax, fKmNeutronTotTkin[0], fKmNeutronTotTkin[iMax-1]);
  fKmNeutronTotXscVector.SetSpline(true);

  for( i = 0; i < iMax; i++)
  {
    tmpxsc[i] = 0.;
    if( i == 0 || i == iMax-1 ) tmpxsc[i] = fKmNeutronTotXsc[i];
    else                        tmpxsc[i] = 0.5*(fKmNeutronTotXsc[i-1]+fKmNeutronTotXsc[i+1]);

    fKmNeutronTotXscVector.PutValues(size_t(i), fKmNeutronTotTkin[i], tmpxsc[i]*millibarn);
  }
}

///////////////////////////////////////////////////////
//
// K-nucleon tot xsc (mb) fit data, std::log(Tkin(MeV))

const G4double G4HadronNucleonXsc::fKpProtonTotXsc[66] = {
0.000000e+00, 1.592400e-01, 3.184700e-01, 7.961800e-01, 1.433120e+00, 2.070060e+00, 
2.866240e+00, 3.582800e+00, 4.378980e+00, 5.015920e+00, 5.573250e+00, 6.449040e+00, 
7.404460e+00, 8.200640e+00, 8.837580e+00, 9.713380e+00, 1.027070e+01, 1.090764e+01, 
1.130573e+01, 1.170382e+01, 1.242038e+01, 1.281847e+01, 1.321656e+01, 1.337580e+01, 
1.345541e+01, 1.329618e+01, 1.265924e+01, 1.242038e+01, 1.250000e+01, 1.305732e+01, 
1.369427e+01, 1.425159e+01, 1.544586e+01, 1.648089e+01, 1.751592e+01, 1.791401e+01, 
1.791401e+01, 1.775478e+01, 1.751592e+01, 1.735669e+01, 1.719745e+01, 1.711783e+01, 
1.703822e+01, 1.695860e+01, 1.695860e+01, 1.695860e+01, 1.695860e+01, 1.687898e+01, 
1.687898e+01, 1.703822e+01, 1.719745e+01, 1.735669e+01, 1.751592e+01, 1.767516e+01, 
1.783439e+01, 1.799363e+01, 1.815287e+01, 1.839172e+01, 1.855095e+01, 1.871019e+01, 
1.886943e+01, 1.918790e+01, 1.942675e+01, 1.966561e+01, 2.006369e+01, 2.054140e+01 
}; // 66


const G4double G4HadronNucleonXsc::fKpProtonTotTkin[66] = {
2.100000e+00, 2.180770e+00, 2.261540e+00, 2.396150e+00, 2.476920e+00, 2.557690e+00, 
2.557690e+00, 2.584620e+00, 2.638460e+00, 2.665380e+00, 2.719230e+00, 2.746150e+00, 
2.800000e+00, 2.853850e+00, 2.934620e+00, 3.042310e+00, 3.150000e+00, 3.311540e+00, 
3.446150e+00, 3.607690e+00, 3.930770e+00, 4.226920e+00, 4.361540e+00, 4.846150e+00, 
4.980770e+00, 5.088460e+00, 5.465380e+00, 5.653850e+00, 5.950000e+00, 6.084620e+00, 
6.246150e+00, 6.300000e+00, 6.380770e+00, 6.515380e+00, 6.730770e+00, 6.838460e+00, 
7.000000e+00, 7.161540e+00, 7.323080e+00, 7.457690e+00, 7.619230e+00, 7.780770e+00, 
7.915380e+00, 8.130770e+00, 8.265380e+00, 8.453850e+00, 8.642310e+00, 8.803850e+00, 
9.019230e+00, 9.234620e+00, 9.530770e+00, 9.773080e+00, 1.001538e+01, 1.017692e+01, 
1.033846e+01, 1.058077e+01, 1.082308e+01, 1.098462e+01, 1.114615e+01, 1.138846e+01, 
1.160385e+01, 1.173846e+01, 1.192692e+01, 1.216923e+01, 1.238461e+01, 1.257308e+01 
}; // 66

const G4double G4HadronNucleonXsc::fKpNeutronTotXsc[75] = {
3.980900e-01, 3.184700e-01, 3.184700e-01, 3.980900e-01, 3.980900e-01, 3.980900e-01, 
3.980900e-01, 3.980900e-01, 3.980900e-01, 4.777100e-01, 3.980900e-01, 3.980900e-01, 
4.777100e-01, 5.573200e-01, 1.035030e+00, 1.512740e+00, 2.149680e+00, 2.786620e+00, 
3.503180e+00, 4.219750e+00, 5.015920e+00, 5.652870e+00, 6.289810e+00, 7.245220e+00, 
8.121020e+00, 8.837580e+00, 9.633760e+00, 1.042994e+01, 1.114650e+01, 1.194268e+01, 
1.265924e+01, 1.329618e+01, 1.393312e+01, 1.449045e+01, 1.496815e+01, 1.552548e+01, 
1.592357e+01, 1.664013e+01, 1.727707e+01, 1.783439e+01, 1.831210e+01, 1.902866e+01, 
1.902866e+01, 1.878981e+01, 1.847134e+01, 1.831210e+01, 1.807325e+01, 1.791401e+01, 
1.783439e+01, 1.767516e+01, 1.759554e+01, 1.743631e+01, 1.743631e+01, 1.751592e+01, 
1.743631e+01, 1.735669e+01, 1.751592e+01, 1.759554e+01, 1.767516e+01, 1.783439e+01, 
1.783439e+01, 1.791401e+01, 1.815287e+01, 1.823248e+01, 1.847134e+01, 1.878981e+01, 
1.894905e+01, 1.902866e+01, 1.934713e+01, 1.966561e+01, 1.990446e+01, 2.014331e+01, 
2.030255e+01, 2.046178e+01, 2.085987e+01 
}; // 75

const G4double G4HadronNucleonXsc::fKpNeutronTotTkin[75] = {
2.692000e-02, 1.615400e-01, 2.961500e-01, 4.576900e-01, 6.461500e-01, 7.538500e-01, 
8.884600e-01, 1.103850e+00, 1.211540e+00, 1.400000e+00, 1.561540e+00, 1.776920e+00, 
1.992310e+00, 2.126920e+00, 2.342310e+00, 2.423080e+00, 2.557690e+00, 2.692310e+00, 
2.800000e+00, 2.988460e+00, 3.203850e+00, 3.365380e+00, 3.500000e+00, 3.688460e+00, 
3.850000e+00, 4.011540e+00, 4.173080e+00, 4.415380e+00, 4.630770e+00, 4.873080e+00, 
5.061540e+00, 5.276920e+00, 5.492310e+00, 5.707690e+00, 5.896150e+00, 6.030770e+00, 
6.138460e+00, 6.219230e+00, 6.273080e+00, 6.326920e+00, 6.407690e+00, 6.650000e+00, 
6.784620e+00, 7.026920e+00, 7.242310e+00, 7.350000e+00, 7.484620e+00, 7.619230e+00, 
7.807690e+00, 7.915380e+00, 8.050000e+00, 8.211540e+00, 8.453850e+00, 8.588460e+00, 
8.830770e+00, 9.073080e+00, 9.288460e+00, 9.476920e+00, 9.665380e+00, 9.826920e+00, 
1.004231e+01, 1.031154e+01, 1.052692e+01, 1.071538e+01, 1.095769e+01, 1.120000e+01, 
1.138846e+01, 1.155000e+01, 1.176538e+01, 1.190000e+01, 1.214231e+01, 1.222308e+01, 
1.238461e+01, 1.246538e+01, 1.265385e+01 
}; // 75

const G4double G4HadronNucleonXsc::fKmProtonTotXsc[106] = {
1.136585e+02, 9.749129e+01, 9.275262e+01, 8.885017e+01, 8.334146e+01, 7.955401e+01, 
7.504530e+01, 7.153658e+01, 6.858537e+01, 6.674913e+01, 6.525784e+01, 6.448781e+01, 
6.360279e+01, 6.255401e+01, 6.127526e+01, 6.032404e+01, 5.997910e+01, 5.443554e+01, 
5.376307e+01, 5.236934e+01, 5.113937e+01, 5.090941e+01, 4.967944e+01, 4.844948e+01, 
4.705575e+01, 4.638327e+01, 4.571080e+01, 4.475958e+01, 4.397213e+01, 4.257840e+01, 
4.102090e+01, 4.090592e+01, 3.906969e+01, 3.839721e+01, 3.756097e+01, 3.644599e+01, 
3.560976e+01, 3.533101e+01, 3.533101e+01, 3.644599e+01, 3.811847e+01, 3.839721e+01, 
3.979094e+01, 4.090592e+01, 4.257840e+01, 4.341463e+01, 4.425087e+01, 4.564460e+01, 
4.759582e+01, 4.703833e+01, 4.843206e+01, 4.787457e+01, 4.452962e+01, 4.202090e+01, 
4.034843e+01, 3.839721e+01, 3.616725e+01, 3.365854e+01, 3.170732e+01, 3.087108e+01, 
3.170732e+01, 3.254355e+01, 3.310104e+01, 3.254355e+01, 3.142857e+01, 3.059233e+01, 
2.947735e+01, 2.891986e+01, 2.836237e+01, 2.752613e+01, 2.696864e+01, 2.641115e+01, 
2.501742e+01, 2.473868e+01, 2.418118e+01, 2.362369e+01, 2.334495e+01, 2.278746e+01, 
2.250871e+01, 2.222997e+01, 2.167247e+01, 2.139373e+01, 2.139373e+01, 2.139373e+01, 
2.111498e+01, 2.083624e+01, 2.055749e+01, 2.083624e+01, 2.055749e+01, 2.083624e+01, 
2.083624e+01, 2.055749e+01, 2.055749e+01, 2.055749e+01, 2.027875e+01, 2.000000e+01, 
2.055749e+01, 2.027875e+01, 2.083624e+01, 2.083624e+01, 2.055749e+01, 2.083624e+01, 
2.083624e+01, 2.083624e+01, 2.139373e+01, 2.139373e+01
}; // 106

const G4double G4HadronNucleonXsc::fKmProtonTotTkin[106] = {
4.017980e+00, 4.125840e+00, 4.179780e+00, 4.251690e+00, 4.287640e+00, 4.341570e+00, 
4.395510e+00, 4.467420e+00, 4.503370e+00, 4.575280e+00, 4.683150e+00, 4.737080e+00, 
4.773030e+00, 4.826970e+00, 4.880900e+00, 4.916850e+00, 4.952810e+00, 4.988760e+00, 
4.988760e+00, 5.006740e+00, 5.006740e+00, 5.042700e+00, 5.078650e+00, 5.114610e+00, 
5.132580e+00, 5.150560e+00, 5.186520e+00, 5.204490e+00, 5.276400e+00, 5.348310e+00, 
5.366290e+00, 5.384270e+00, 5.456180e+00, 5.564040e+00, 5.600000e+00, 5.671910e+00, 
5.743820e+00, 5.833710e+00, 5.905620e+00, 5.977530e+00, 6.085390e+00, 6.085390e+00, 
6.157300e+00, 6.175280e+00, 6.211240e+00, 6.229210e+00, 6.247190e+00, 6.337080e+00, 
6.391010e+00, 6.516850e+00, 6.462920e+00, 6.498880e+00, 6.570790e+00, 6.606740e+00, 
6.660670e+00, 6.678650e+00, 6.696630e+00, 6.732580e+00, 6.804490e+00, 6.876400e+00, 
6.948310e+00, 7.020220e+00, 7.074160e+00, 7.182020e+00, 7.235960e+00, 7.289890e+00, 
7.397750e+00, 7.523600e+00, 7.631460e+00, 7.757300e+00, 7.901120e+00, 8.062920e+00, 
8.260670e+00, 8.386520e+00, 8.530340e+00, 8.674160e+00, 8.817980e+00, 8.943820e+00, 
9.087640e+00, 9.267420e+00, 9.429210e+00, 9.573030e+00, 9.698880e+00, 9.896630e+00, 
1.002247e+01, 1.016629e+01, 1.031011e+01, 1.048989e+01, 1.063371e+01, 1.077753e+01, 
1.095730e+01, 1.108315e+01, 1.120899e+01, 1.135281e+01, 1.149663e+01, 1.162247e+01, 
1.174831e+01, 1.187416e+01, 1.200000e+01, 1.212584e+01, 1.221573e+01, 1.234157e+01, 
1.239551e+01, 1.250337e+01, 1.261124e+01, 1.273708e+01 
}; // 106

const G4double G4HadronNucleonXsc::fKmNeutronTotXsc[68] = {
2.621810e+01, 2.741123e+01, 2.868413e+01, 2.963889e+01, 3.067343e+01, 3.178759e+01, 
3.282148e+01, 3.417466e+01, 3.536778e+01, 3.552620e+01, 3.544576e+01, 3.496756e+01, 
3.433030e+01, 3.401166e+01, 3.313537e+01, 3.257772e+01, 3.178105e+01, 3.138264e+01, 
3.074553e+01, 2.970952e+01, 2.891301e+01, 2.827542e+01, 2.787700e+01, 2.715978e+01, 
2.660181e+01, 2.612394e+01, 2.564574e+01, 2.516721e+01, 2.421098e+01, 2.365235e+01, 
2.317366e+01, 2.261437e+01, 2.237389e+01, 2.205427e+01, 2.181395e+01, 2.165357e+01, 
2.149335e+01, 2.133297e+01, 2.109232e+01, 2.093128e+01, 2.069030e+01, 2.052992e+01, 
2.028927e+01, 2.012824e+01, 1.996737e+01, 1.996590e+01, 1.988530e+01, 1.964432e+01, 
1.948361e+01, 1.940236e+01, 1.940040e+01, 1.931882e+01, 1.947593e+01, 1.947429e+01, 
1.939320e+01, 1.939157e+01, 1.946922e+01, 1.962715e+01, 1.970481e+01, 1.970301e+01, 
1.993958e+01, 2.009669e+01, 2.025380e+01, 2.033178e+01, 2.049003e+01, 2.064747e+01, 
2.080540e+01, 2.096333e+01 
}; // 68

const G4double G4HadronNucleonXsc::fKmNeutronTotTkin[68] = {
5.708500e+00, 5.809560e+00, 5.896270e+00, 5.954120e+00, 5.997630e+00, 6.041160e+00, 
6.142160e+00, 6.171410e+00, 6.272470e+00, 6.344390e+00, 6.416230e+00, 6.459180e+00, 
6.487690e+00, 6.501940e+00, 6.544740e+00, 6.573280e+00, 6.616110e+00, 6.644710e+00, 
6.658840e+00, 6.744700e+00, 6.773150e+00, 6.830410e+00, 6.859010e+00, 6.916240e+00, 
6.973530e+00, 6.987730e+00, 7.030670e+00, 7.102360e+00, 7.173880e+00, 7.288660e+00, 
7.374720e+00, 7.547000e+00, 7.690650e+00, 7.791150e+00, 7.920420e+00, 8.020980e+00, 
8.107160e+00, 8.207720e+00, 8.365740e+00, 8.523790e+00, 8.710560e+00, 8.811110e+00, 
8.969140e+00, 9.127190e+00, 9.270860e+00, 9.400230e+00, 9.486440e+00, 9.673210e+00, 
9.802510e+00, 9.946220e+00, 1.011870e+01, 1.029116e+01, 1.047808e+01, 1.062181e+01, 
1.075114e+01, 1.089488e+01, 1.106739e+01, 1.118244e+01, 1.135496e+01, 1.151307e+01, 
1.171439e+01, 1.190130e+01, 1.208822e+01, 1.223199e+01, 1.231829e+01, 1.247646e+01, 
1.259150e+01, 1.270655e+01 
}; // 68





//
//
///////////////////////////////////////////////////////////////////////////////////////
