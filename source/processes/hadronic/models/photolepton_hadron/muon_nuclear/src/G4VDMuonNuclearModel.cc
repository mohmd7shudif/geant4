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
// $Id: $
//
// Author:      D.H. Wright
// Date:        2 February 2011
//
// Description: simple vector dominance model of muon nuclear interaction 
//              in which a gamma from the virtual photon spectrum 
//              interacts either as a pi+ or a pi- in the nucleus
//              Kokoulin's parameterized gamma spectrum is used.
//

#include "G4VDMuonNuclearModel.hh"

#include "Randomize.hh"
#include "G4CascadeInterface.hh"
#include "G4TheoFSGenerator.hh" 
#include "G4GeneratorPrecompoundInterface.hh"
#include "G4ExcitationHandler.hh"
#include "G4PreCompoundModel.hh"
#include "G4LundStringFragmentation.hh"
#include "G4ExcitedStringDecay.hh"
#include "G4FTFModel.hh"

G4VDMuonNuclearModel::G4VDMuonNuclearModel()
 : G4InelasticInteraction("G4VDMuonNuclearModel")
{
  SetMinEnergy(0.0);
  SetMaxEnergy(1*PeV);
  CutFixed = 0.2*GeV;
  NBIN = 1000;

  for (G4int k = 0; k < 5; k++) {
    for (G4int j = 0; j < 8; j++) {
      for (G4int i = 0; i < 1001; i++) {
        proba[k][j][i] = 0.0;
        ya[i] = 0.0;
      }
    }
  }

  MakeSamplingTable();

  // Build FTFP model
  ftfp = new G4TheoFSGenerator();
  precoInterface = new G4GeneratorPrecompoundInterface();
  theHandler = new G4ExcitationHandler();
  preEquilib = new G4PreCompoundModel(theHandler);
  precoInterface->SetDeExcitation(preEquilib);
  ftfp->SetTransport(precoInterface);
  theFragmentation = new G4LundStringFragmentation();
  theStringDecay = new G4ExcitedStringDecay(theFragmentation);    
  theStringModel = new G4FTFModel;
  theStringModel->SetFragmentationModel(theStringDecay);
  ftfp->SetHighEnergyGenerator(theStringModel);

  // Build Bertini cascade
  bert = new G4CascadeInterface();
}


G4VDMuonNuclearModel::~G4VDMuonNuclearModel()
{
  delete ftfp;
  delete theHandler;
  delete preEquilib;
  delete theFragmentation;
  delete theStringDecay;
  delete theStringModel;
  delete bert;
}

  
G4HadFinalState*
G4VDMuonNuclearModel::ApplyYourself(const G4HadProjectile& aTrack,
                                    G4Nucleus& targetNucleus)
{
  theParticleChange.Clear();

  // For very low energy, return initial track
  G4double epmax = aTrack.GetTotalEnergy() - 0.5*proton_mass_c2;
  if (epmax <= CutFixed) {
    theParticleChange.SetStatusChange(isAlive);
    theParticleChange.SetEnergyChange(aTrack.GetKineticEnergy());
    theParticleChange.SetMomentumChange(aTrack.Get4Momentum().vect().unit());
    return &theParticleChange;
  }

  // Produce recoil muon and transferred photon
  G4DynamicParticle* transferredPhoton = CalculateEMVertex(aTrack, targetNucleus);

  // Interact the gamma with the nucleus
  CalculateHadronicVertex(transferredPhoton, targetNucleus);
  return &theParticleChange;
}


G4DynamicParticle*
G4VDMuonNuclearModel::CalculateEMVertex(const G4HadProjectile& aTrack,
                                        G4Nucleus& targetNucleus)
{
  // Select sampling table
  G4double KineticEnergy = aTrack.GetKineticEnergy();
  G4double TotalEnergy = aTrack.GetTotalEnergy();
  G4double Mass = G4MuonMinus::MuonMinus()->GetPDGMass();
  G4double lnZ = std::log(G4double(targetNucleus.GetZ_asInt() ) );

  G4double epmin = CutFixed;
  G4double epmax = TotalEnergy - 0.5*proton_mass_c2;
  G4double m0 = 0.2*GeV;

  G4double delmin = 1.e10;
  G4double del;
  G4int izz = 0;
  G4int itt = 0;
  G4int NBINminus1 = NBIN - 1;

  G4int nzdat = 5;
  G4double zdat[] = {1.,4.,13.,29.,92.};
  for (G4int iz = 0; iz < nzdat; iz++) {
    del = std::abs(lnZ-std::log(zdat[iz]));
    if (del < delmin) {
      delmin = del;
      izz = iz;
    }
  }
 
  G4int ntdat = 8;
  G4double tdat[] = {1.e3,1.e4,1.e5,1.e6,1.e7,1.e8,1.e9,1.e10};
  delmin = 1.e10;
  for (G4int it = 0; it < ntdat; it++) {
    del = std::abs(std::log(KineticEnergy)-std::log(tdat[it]) );
    if (del < delmin) {
      delmin = del;
      itt = it;
    }
  }

  // Sample the energy transfer according to the probability table
  G4double r = G4UniformRand();

  G4int iy = -1;
  do {
       iy += 1 ;
     } while (((proba[izz][itt][iy]) < r)&&(iy < NBINminus1)) ;

  // Sampling is done uniformly in y in the bin

  G4double y; 
  if (iy < NBIN)
    y = ya[iy] + G4UniformRand() * (ya[iy+1] - ya[iy]);
  else
    y = ya[iy];

  G4double x = std::exp(y);
  G4double ep = epmin*std::exp(x*std::log(epmax/epmin) );

  // Sample scattering angle of mu, but first t should be sampled.
  G4double yy = ep/TotalEnergy;
  G4double tmin = Mass*Mass*yy*yy/(1.-yy);
  G4double tmax = 2.*proton_mass_c2*ep;
  G4double t1;
  G4double t2;
  if (m0 < ep) {
    t1 = m0*m0;
    t2 = ep*ep;
  } else {
    t1 = ep*ep;
    t2 = m0*m0;
  }

  G4double w1 = tmax*t1;
  G4double w2 = tmax+t1;
  G4double w3 = tmax*(tmin+t1)/(tmin*w2);
  G4double y1 = 1.-yy;
  G4double y2 = 0.5*yy*yy;
  G4double y3 = y1+y2;

  G4double t;
  G4double rej;

  // Now sample t
  G4int ntry = 0;
  do
  {
    ntry += 1;
    t = w1/(w2*std::exp(G4UniformRand()*std::log(w3))-tmax);
    rej = (1.-t/tmax)*(y1*(1.-tmin/t)+y2)/(y3*(1.-t/t2)); 
  } while (G4UniformRand() > rej) ;

  // compute angle from t
  G4double sinth2 =
             0.5*(t-tmin)/(2.*(TotalEnergy*(TotalEnergy-ep)-Mass*Mass)-tmin);
  G4double theta = std::acos(1. - 2.*sinth2);

  G4double phi = twopi*G4UniformRand();
  G4double sinth = std::sin(theta);
  G4double dirx = sinth*std::cos(phi);
  G4double diry = sinth*std::sin(phi);
  G4double dirz=std::cos(theta);
  G4ThreeVector finalDirection(dirx,diry,dirz);
  G4ThreeVector ParticleDirection(aTrack.Get4Momentum().vect().unit() );
  finalDirection.rotateUz(ParticleDirection);

  G4double NewKinEnergy = KineticEnergy - ep;
  G4double finalMomentum = std::sqrt(NewKinEnergy*(NewKinEnergy+2.*Mass) );
  G4double Ef = NewKinEnergy + Mass;
  G4double initMomentum = std::sqrt(KineticEnergy*(TotalEnergy+Mass) );

  // Set energy and direction of scattered primary in theParticleChange
  theParticleChange.SetStatusChange(isAlive);
  theParticleChange.SetEnergyChange(NewKinEnergy);
  theParticleChange.SetMomentumChange(finalDirection);

  // Now create the emitted gamma 
  G4LorentzVector primaryMomentum(initMomentum*ParticleDirection, TotalEnergy);
  G4LorentzVector fsMomentum(finalMomentum*finalDirection, Ef);
  G4LorentzVector momentumTransfer = primaryMomentum - fsMomentum;

  G4DynamicParticle* gamma = 
           new G4DynamicParticle(G4Gamma::Gamma(), momentumTransfer);
 
  //  G4ThreeVector mom(0.0, 0.0, 50.*GeV);
  //  G4DynamicParticle* gamma = new G4DynamicParticle();
  //  gamma->SetDefinition(G4Gamma::Gamma() );
  //  gamma->SetMomentum(mom); 
  return gamma;
}

void
G4VDMuonNuclearModel::CalculateHadronicVertex(G4DynamicParticle* incident,
                                              G4Nucleus& target)
{
  G4HadFinalState* hfs = 0;

  // Convert incident particle to a pion
  G4double piMass = G4PionMinus::PionMinus()->GetPDGMass();
  G4double piKE = incident->GetTotalEnergy() - piMass;
  G4double piMom = std::sqrt(piKE*(piKE + 2*piMass) );
  G4ThreeVector piMomentum(incident->GetMomentumDirection() );
  piMomentum *= piMom;
  delete incident;

  // Choose pi+ or pi- randomly and interact them with GHEISHA-style models
  if (G4UniformRand() > 0.5) {
    G4DynamicParticle theHadron(G4PionMinus::PionMinus(), piMomentum);
    G4HadProjectile projectile(theHadron);
    if (piKE < 10*GeV) {
      hfs = bert->ApplyYourself(projectile, target);
    } else {
      hfs = ftfp->ApplyYourself(projectile, target);
    }

  } else {
    G4DynamicParticle theHadron(G4PionPlus::PionPlus(), piMomentum);
    G4HadProjectile projectile(theHadron);
    if (piKE < 10*GeV) {
      hfs = bert->ApplyYourself(projectile, target);
    } else {
      hfs = ftfp->ApplyYourself(projectile, target);
    }
  }

  // Copy secondaries from sub-model to model
  G4int nsec = hfs->GetNumberOfSecondaries();
  G4DynamicParticle* dynSec;
  for (G4int i = 0; i < nsec; i++) {
    dynSec = hfs->GetSecondary(i)->GetParticle();
    theParticleChange.AddSecondary(dynSec);
  }

} 

void G4VDMuonNuclearModel::MakeSamplingTable()
{
  G4double adat[] = {1.01,9.01,26.98,63.55,238.03};
  G4double zdat[] = {1.,4.,13.,29.,92.};
  G4int nzdat = 5;

  G4double tdat[] = {1.e3,1.e4,1.e5,1.e6,1.e7,1.e8,1.e9,1.e10};
  G4int ntdat = 8;

  G4int nbin;
  G4double KineticEnergy;
  G4double TotalEnergy;
  G4double Maxep;
  G4double CrossSection;

  G4double c;
  G4double y;
  G4double ymin,ymax;
  G4double dy,yy;
  G4double dx,x;
  G4double ep;

  G4double AtomicNumber;
  G4double AtomicWeight;

  for (G4int iz = 0; iz < nzdat; iz++) {
    AtomicNumber = zdat[iz];
    AtomicWeight = adat[iz]*(g/mole);  

    for (G4int it = 0; it < ntdat; it++) {
      KineticEnergy = tdat[it];
      TotalEnergy = KineticEnergy + G4MuonMinus::MuonMinus()->GetPDGMass();
      Maxep = TotalEnergy - 0.5*proton_mass_c2;

      CrossSection = 0.0;

      // Calculate the differential cross section
      // numerical integration in log .........
      c = std::log(Maxep/CutFixed);
      ymin = -5.0;
      ymax = 0.0;
      dy = (ymax-ymin)/NBIN; 

      nbin=-1;              

      y = ymin - 0.5*dy;
      yy = ymin - dy;
      for (G4int i = 0; i < NBIN; i++) {
        y += dy;
        x = std::exp(y);
        yy += dy;
        dx = std::exp(yy+dy)-std::exp(yy);
      
        ep = CutFixed*std::exp(c*x);

        CrossSection +=
           ep*dx*muNucXS.ComputeDDMicroscopicCrossSection(KineticEnergy,
                                                          AtomicNumber,
                                                          AtomicWeight, ep);
        if (nbin < NBIN) {
          nbin += 1;
          ya[nbin] = y;
          proba[iz][it][nbin] = CrossSection;
        }
      }
      ya[NBIN] = 0.;
 
      if (CrossSection > 0.0) {
        for (G4int ib = 0; ib <= nbin; ib++) proba[iz][it][ib] /= CrossSection;
      }
    } // loop on it
  } // loop on iz

  // G4cout << " Kokoulin XS = "
  //       << muNucXS.ComputeDDMicroscopicCrossSection(1*GeV, 20.0, 40.0*g/mole, 0.3*GeV)/millibarn
  //       << G4endl; 
}

