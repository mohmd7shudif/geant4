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
// $Id: G4PenelopeBremsstrahlungModel.cc,v 1.1 2010-12-20 14:11:37 pandola Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// Author: Luciano Pandola
//
// History:
// --------
// 23 Nov 2010   L Pandola    First complete implementation, Penelope v2008
// 24 May 2011   L. Pandola   Renamed (make default Penelope)
//

#include "G4PenelopeBremsstrahlungModel.hh"
#include "G4PenelopeBremsstrahlungFS.hh"
#include "G4PenelopeBremsstrahlungAngular.hh"
#include "G4ParticleDefinition.hh"
#include "G4MaterialCutsCouple.hh"
#include "G4ProductionCutsTable.hh"
#include "G4DynamicParticle.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4PenelopeOscillatorManager.hh"
#include "G4PenelopeCrossSection.hh"
#include "G4PhysicsFreeVector.hh"
#include "G4PhysicsLogVector.hh" 
#include "G4PhysicsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
 
G4PenelopeBremsstrahlungModel::G4PenelopeBremsstrahlungModel(const G4ParticleDefinition*,
							     const G4String& nam)
  :G4VEmModel(nam),fParticleChange(0),isInitialised(false),energyGrid(0),  
   XSTableElectron(0),XSTablePositron(0)
  
{
  fIntrinsicLowEnergyLimit = 100.0*eV;
  fIntrinsicHighEnergyLimit = 100.0*GeV;
  nBins = 200;

  SetHighEnergyLimit(fIntrinsicHighEnergyLimit);
  //
  oscManager = G4PenelopeOscillatorManager::GetOscillatorManager();
  //
  verboseLevel= 0;
   
  // Verbosity scale:
  // 0 = nothing
  // 1 = warning for energy non-conservation
  // 2 = details of energy budget
  // 3 = calculation of cross sections, file openings, sampling of atoms
  // 4 = entering in methods

  fPenelopeFSHelper = new G4PenelopeBremsstrahlungFS();
  fPenelopeAngular = new G4PenelopeBremsstrahlungAngular();

  // Atomic deexcitation model activated by default
  SetDeexcitationFlag(true);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
 
G4PenelopeBremsstrahlungModel::~G4PenelopeBremsstrahlungModel()
{
  ClearTables();
  if (fPenelopeFSHelper)
    delete fPenelopeFSHelper;
  if (fPenelopeAngular)
    delete fPenelopeAngular;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4PenelopeBremsstrahlungModel::Initialise(const G4ParticleDefinition*,
                                             const G4DataVector&)
{
  if (verboseLevel > 3)
    G4cout << "Calling G4PenelopeBremsstrahlungModel::Initialise()" << G4endl;
 
  //Clear and re-build the tables 
  ClearTables();

  //Set the number of bins for the tables. 20 points per decade
  nBins = (size_t) (20*std::log10(HighEnergyLimit()/LowEnergyLimit()));
  nBins = std::max(nBins,(size_t)100);
  energyGrid = new G4PhysicsLogVector(LowEnergyLimit(),
                                      HighEnergyLimit(), 
                                      nBins-1); //one hidden bin is added
 

  XSTableElectron = new 
    std::map< std::pair<const G4Material*,G4double>, G4PenelopeCrossSection*>;
  XSTablePositron = new 
    std::map< std::pair<const G4Material*,G4double>, G4PenelopeCrossSection*>;

  if (verboseLevel > 2) {
    G4cout << "Penelope Bremsstrahlung model v2008 is initialized " << G4endl
           << "Energy range: "
           << LowEnergyLimit() / keV << " keV - "
           << HighEnergyLimit() / GeV << " GeV." 
           << G4endl;
  }
 
  if(isInitialised) return;
  fParticleChange = GetParticleChangeForLoss();
  isInitialised = true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double G4PenelopeBremsstrahlungModel::CrossSectionPerVolume(const G4Material* material,
                                           const G4ParticleDefinition* theParticle,
                                           G4double energy,
                                           G4double cutEnergy,
                                           G4double)
{
  //
  if (verboseLevel > 3)
    G4cout << "Calling CrossSectionPerVolume() of G4PenelopeBremsstrahlungModel" << G4endl;
 
  SetupForMaterial(theParticle, material, energy);

  G4double crossPerMolecule = 0.;

  G4PenelopeCrossSection* theXS = GetCrossSectionTableForCouple(theParticle,material,
                                                                cutEnergy);

  if (theXS)
    crossPerMolecule = theXS->GetHardCrossSection(energy);

  G4double atomDensity = material->GetTotNbOfAtomsPerVolume();
  G4double atPerMol =  oscManager->GetAtomsPerMolecule(material);
 
  if (verboseLevel > 3)
    G4cout << "Material " << material->GetName() << " has " << atPerMol <<
      "atoms per molecule" << G4endl;

  G4double moleculeDensity = 0.; 
  if (atPerMol)
    moleculeDensity = atomDensity/atPerMol;
 
  G4double crossPerVolume = crossPerMolecule*moleculeDensity;

  if (verboseLevel > 2)
  {
    G4cout << "G4PenelopeBremsstrahlungModel " << G4endl;
    G4cout << "Mean free path for gamma emission > " << cutEnergy/keV << " keV at " <<
      energy/keV << " keV = " << (1./crossPerVolume)/mm << " mm" << G4endl;
  }
  
  return crossPerVolume;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
                                                                                                     
//This is a dummy method. Never inkoved by the tracking, it just issues
//a warning if one tries to get Cross Sections per Atom via the
//G4EmCalculator.
G4double G4PenelopeBremsstrahlungModel::ComputeCrossSectionPerAtom(const G4ParticleDefinition*,
								     G4double,
								     G4double,
								     G4double,
								     G4double,
								     G4double)
{
  G4cout << "*** G4PenelopeBremsstrahlungModel -- WARNING ***" << G4endl;
  G4cout << "Penelope Bremsstrahlung model v2008 does not calculate cross section _per atom_ " << G4endl;
  G4cout << "so the result is always zero. For physics values, please invoke " << G4endl;
  G4cout << "GetCrossSectionPerVolume() or GetMeanFreePath() via the G4EmCalculator" << G4endl;
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double G4PenelopeBremsstrahlungModel::ComputeDEDXPerVolume(const G4Material* material,
							     const G4ParticleDefinition* theParticle,
							     G4double kineticEnergy,
							     G4double cutEnergy)
{
  if (verboseLevel > 3)
    G4cout << "Calling ComputeDEDX() of G4PenelopeBremsstrahlungModel" << G4endl;
 
  G4PenelopeCrossSection* theXS = GetCrossSectionTableForCouple(theParticle,material,
                                                                cutEnergy);
  G4double sPowerPerMolecule = 0.0;
  if (theXS)
    sPowerPerMolecule = theXS->GetSoftStoppingPower(kineticEnergy);

  
  G4double atomDensity = material->GetTotNbOfAtomsPerVolume();
  G4double atPerMol =  oscManager->GetAtomsPerMolecule(material);
                                                                                        
  G4double moleculeDensity = 0.; 
  if (atPerMol)
    moleculeDensity = atomDensity/atPerMol;
 
  G4double sPowerPerVolume = sPowerPerMolecule*moleculeDensity;

  if (verboseLevel > 2)
    {
      G4cout << "G4PenelopeBremsstrahlungModel " << G4endl;
      G4cout << "Stopping power < " << cutEnergy/keV << " keV at " <<
        kineticEnergy/keV << " keV = " << 
        sPowerPerVolume/(keV/mm) << " keV/mm" << G4endl;
    }
  return sPowerPerVolume;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4PenelopeBremsstrahlungModel::SampleSecondaries(std::vector<G4DynamicParticle*>*fvect,
						      const G4MaterialCutsCouple* couple,
						      const G4DynamicParticle* aDynamicParticle,
						      G4double cutG,
						      G4double)
{
  if (verboseLevel > 3)
    G4cout << "Calling SampleSecondaries() of G4PenelopeBremsstrahlungModel" << G4endl;

  G4double kineticEnergy = aDynamicParticle->GetKineticEnergy();
  const G4Material* material = couple->GetMaterial();

  if (kineticEnergy <= fIntrinsicLowEnergyLimit)
   {
     fParticleChange->SetProposedKineticEnergy(0.);
     fParticleChange->ProposeLocalEnergyDeposit(kineticEnergy);
     return ;
   }

  G4ParticleMomentum particleDirection0 = aDynamicParticle->GetMomentumDirection();
  //This is the momentum
  G4ThreeVector initialMomentum =  aDynamicParticle->GetMomentum();

  //Not enough energy to produce a secondary! Return with nothing happened
  if (kineticEnergy < cutG)
    return;

  if (verboseLevel > 3)
    G4cout << "Going to sample gamma energy for: " <<material->GetName() << " " << 
      "energy = " << kineticEnergy/keV << ", cut = " << cutG/keV << G4endl;

   //Sample gamma's energy according to the spectrum
  G4double gammaEnergy = 
    fPenelopeFSHelper->SampleGammaEnergy(kineticEnergy,material,cutG);

  if (verboseLevel > 3)
    G4cout << "Sampled gamma energy: " << gammaEnergy/keV << " keV" << G4endl;

  G4double Zeff = std::sqrt(fPenelopeFSHelper->GetEffectiveZSquared(material));
  if (verboseLevel > 3)
    {
      G4cout << "Sampling in " << material->GetName() << " with Zeff= " << 
	Zeff << G4endl;
    }
  
  //Now sample cosTheta for the Gamma
  G4double cosThetaPrimary = 
    fPenelopeAngular->SampleCosTheta(Zeff,kineticEnergy,gammaEnergy);
   
  if (verboseLevel > 3)
    G4cout << "Sampled cosTheta for e-: " << cosThetaPrimary << G4endl;

  G4double residualPrimaryEnergy = kineticEnergy-gammaEnergy;
  if (residualPrimaryEnergy < 0)
    {
      //Ok we have a problem, all energy goes with the gamma
      gammaEnergy += residualPrimaryEnergy;
      residualPrimaryEnergy = 0.0;
    }
  
  //Get primary kinematics
  G4double sinTheta = std::sqrt(1. - cosThetaPrimary*cosThetaPrimary);
  G4double phi  = twopi * G4UniformRand(); 
  G4ThreeVector gammaDirection1(sinTheta* std::cos(phi),
				sinTheta* std::sin(phi),
				cosThetaPrimary);
  
  gammaDirection1.rotateUz(particleDirection0);
  
  //Produce final state according to momentum conservation
  G4ThreeVector particleDirection1 = initialMomentum - gammaEnergy*gammaDirection1;
  particleDirection1 = particleDirection1.unit(); //normalize

  //Update the primary particle
  if (residualPrimaryEnergy > 0.)
    {
      fParticleChange->ProposeMomentumDirection(particleDirection1);
      fParticleChange->SetProposedKineticEnergy(residualPrimaryEnergy);
    }
  else
    {
      fParticleChange->SetProposedKineticEnergy(0.);
    }
  
  //Now produce the photon
  G4DynamicParticle* theGamma = new G4DynamicParticle(G4Gamma::Gamma(),
                                                      gammaDirection1,
                                                      gammaEnergy);
  fvect->push_back(theGamma);
  
  if (verboseLevel > 1)
    {
      G4cout << "-----------------------------------------------------------" << G4endl;
      G4cout << "Energy balance from G4PenelopeBremsstrahlung" << G4endl;
      G4cout << "Incoming primary energy: " << kineticEnergy/keV << " keV" << G4endl;
      G4cout << "-----------------------------------------------------------" << G4endl;
      G4cout << "Outgoing primary energy: " << residualPrimaryEnergy/keV << " keV" << G4endl;
      G4cout << "Bremsstrahlung photon " << gammaEnergy/keV << " keV" << G4endl;
      G4cout << "Total final state: " << (residualPrimaryEnergy+gammaEnergy)/keV 
             << " keV" << G4endl;
      G4cout << "-----------------------------------------------------------" << G4endl;
    }

  if (verboseLevel > 0)
    {
      G4double energyDiff = std::fabs(residualPrimaryEnergy+gammaEnergy-kineticEnergy);
      if (energyDiff > 0.05*keV)
        G4cout << "Warning from G4PenelopeBremsstrahlung: problem with energy conservation: " 
	       <<
          (residualPrimaryEnergy+gammaEnergy)/keV <<
          " keV (final) vs. " <<
          kineticEnergy/keV << " keV (initial)" << G4endl;
    }  
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4PenelopeBremsstrahlungModel::ClearTables()
{  
  std::map< std::pair<const G4Material*,G4double>, G4PenelopeCrossSection*>::iterator i;
  if (XSTableElectron)
    {
      for (i=XSTableElectron->begin(); i != XSTableElectron->end(); i++)
        {
          G4PenelopeCrossSection* tab = i->second;
          delete tab;
        }
      delete XSTableElectron;
      XSTableElectron = 0;
    }
  if (XSTablePositron)
    {
      for (i=XSTablePositron->begin(); i != XSTablePositron->end(); i++)
        {
          G4PenelopeCrossSection* tab = i->second;
          delete tab;
        }
      delete XSTablePositron;
      XSTablePositron = 0;
    } 

  if (energyGrid)
    delete energyGrid;

  if (fPenelopeFSHelper)
    fPenelopeFSHelper->ClearTables();
  
  if (fPenelopeAngular)
    fPenelopeAngular->ClearTables();

  if (verboseLevel > 2)
    G4cout << "G4PenelopeBremsstrahlungModel: cleared tables" << G4endl;
 return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double G4PenelopeBremsstrahlungModel::MinEnergyCut(const G4ParticleDefinition*,
						       const G4MaterialCutsCouple*)
{
  return fIntrinsicLowEnergyLimit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4PenelopeBremsstrahlungModel::BuildXSTable(const G4Material* mat,G4double cut)
{
//
  //This method fills the G4PenelopeCrossSection containers for electrons or positrons
  //and for the given material/cut couple. 
  //Equivalent of subroutines EBRaT and PINaT of Penelope
  //
  if (verboseLevel > 2)
    {
      G4cout << "G4PenelopeBremsstrahlungModel: going to build cross section table " << G4endl;
      G4cout << "for e+/e- in " << mat->GetName() << G4endl;
    }

  //Tables have been already created (checked by GetCrossSectionTableForCouple)
  if (energyGrid->GetVectorLength() != nBins) 
    {
      G4ExceptionDescription ed;
      ed << "Energy Grid looks not initialized" << G4endl;
      ed << nBins << " " << energyGrid->GetVectorLength() << G4endl;
      G4Exception("G4PenelopeBremsstrahlungModel::BuildXSTable()",
		  "em2016",FatalException,ed);
    }

  G4PenelopeCrossSection* XSEntry = new G4PenelopeCrossSection(nBins);
  G4PenelopeCrossSection* XSEntry2 = new G4PenelopeCrossSection(nBins);

  G4PhysicsTable* table = fPenelopeFSHelper->GetScaledXSTable(mat,cut);


  //loop on the energy grid  
  for (size_t bin=0;bin<nBins;bin++)
    {
       G4double energy = energyGrid->GetLowEdgeEnergy(bin);
       G4double XH0=0, XH1=0, XH2=0;
       G4double XS0=0, XS1=0, XS2=0;
    
       //Global xs factor
       G4double fact = fPenelopeFSHelper->GetEffectiveZSquared(mat)*
	 ((energy+electron_mass_c2)*(energy+electron_mass_c2)/
	  (energy*(energy+2.0*electron_mass_c2))); 
       
       G4double restrictedCut = cut/energy;
   
       //Now I need the dSigma/dX profile - interpolated on energy - for 
       //the 32-point x grid. Interpolation is log-log
       size_t nBinsX = fPenelopeFSHelper->GetNBinsX();       
       G4double* tempData = new G4double[nBinsX];     
       G4double logene = std::log(energy);
       for (size_t ix=0;ix<nBinsX;ix++)	
	 {
	   //find dSigma/dx for the given E. X belongs to the 32-point grid.
	   G4double val = (*table)[ix]->Value(logene);	   
	   tempData[ix] = std::exp(val); //back to the real value!
	 } 
       
       G4double XH0A = 0.;
       if (restrictedCut <= 1) //calculate only if we are above threshold!
	 XH0A = fPenelopeFSHelper->GetMomentumIntegral(tempData,1.0,-1) -
	   fPenelopeFSHelper->GetMomentumIntegral(tempData,restrictedCut,-1);
       G4double XS1A = fPenelopeFSHelper->GetMomentumIntegral(tempData,
							      restrictedCut,0);       
       G4double XS2A = fPenelopeFSHelper->GetMomentumIntegral(tempData,
							      restrictedCut,1);
       G4double XH1A=0, XH2A=0;
       if (restrictedCut <=1)
	 {
	   XH1A = fPenelopeFSHelper->GetMomentumIntegral(tempData,1.0,0) -
	     XS1A;
	   XH2A = fPenelopeFSHelper->GetMomentumIntegral(tempData,1.0,1) -
	     XS2A;
	 }
       delete[] tempData;

       XH0 = XH0A*fact;
       XS1 = XS1A*fact*energy;
       XH1 = XH1A*fact*energy;
       XS2 = XS2A*fact*energy*energy;
       XH2 = XH2A*fact*energy*energy;

       XSEntry->AddCrossSectionPoint(bin,energy,XH0,XH1,XH2,XS0,XS1,XS2);

       //take care of positrons
       G4double posCorrection = GetPositronXSCorrection(mat,energy);
       XSEntry2->AddCrossSectionPoint(bin,energy,XH0*posCorrection,
				      XH1*posCorrection,
				      XH2*posCorrection,
				      XS0,
				      XS1*posCorrection,
				      XS2*posCorrection);
    }
 
  //Insert in the appropriate table
  std::pair<const G4Material*,G4double> theKey = std::make_pair(mat,cut);  
  XSTableElectron->insert(std::make_pair(theKey,XSEntry));
  XSTablePositron->insert(std::make_pair(theKey,XSEntry2));
  
  return;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4PenelopeCrossSection* 
G4PenelopeBremsstrahlungModel::GetCrossSectionTableForCouple(const G4ParticleDefinition* part,
							     const G4Material* mat,
							     G4double cut)
{
  if (part != G4Electron::Electron() && part != G4Positron::Positron())
    {
      G4ExceptionDescription ed;
      ed << "Invalid particle: " << part->GetParticleName() << G4endl;
      G4Exception("G4PenelopeBremsstrahlungModel::GetCrossSectionTableForCouple()",
		  "em0001",FatalException,ed);
      return NULL;
    }

  if (part == G4Electron::Electron())
    {
      if (!XSTableElectron)
        {	  
          G4String excep = "The Cross Section Table for e- was not initialized correctly!";
          G4Exception("G4PenelopeBremsstrahlungModel::GetCrossSectionTableForCouple()",
		      "em2013",FatalException,excep);          
	  return NULL;
        }
      std::pair<const G4Material*,G4double> theKey = std::make_pair(mat,cut);
      if (XSTableElectron->count(theKey)) //table already built
        return XSTableElectron->find(theKey)->second;
      else
        {
          BuildXSTable(mat,cut);
          if (XSTableElectron->count(theKey)) //now it should be ok!
            return XSTableElectron->find(theKey)->second;
          else
            {
	      G4ExceptionDescription ed;
              ed << "Unable to build e- table for " << mat->GetName() << G4endl;
              G4Exception("G4PenelopeBremsstrahlungModel::GetCrossSectionTableForCouple()",
			  "em2009",FatalException,ed);
            }
        }
    }
  if (part == G4Positron::Positron())
    {
      if (!XSTablePositron)
        {
	  G4String excep = "The Cross Section Table for e+ was not initialized correctly!";
          G4Exception("G4PenelopeBremsstrahlungModel::GetCrossSectionTableForCouple()",
		      "em2013",FatalException,excep); 
	  return NULL;
        }
      std::pair<const G4Material*,G4double> theKey = std::make_pair(mat,cut);
      if (XSTablePositron->count(theKey)) //table already built
        return XSTablePositron->find(theKey)->second;
      else
        {
          BuildXSTable(mat,cut);
          if (XSTablePositron->count(theKey)) //now it should be ok!
            return XSTablePositron->find(theKey)->second;
          else
            {
	      G4ExceptionDescription ed;
              ed << "Unable to build e+ table for " << mat->GetName() << G4endl;
              G4Exception("G4PenelopeBremsstrahlungModel::GetCrossSectionTableForCouple()",
			  "em2009",FatalException,ed);
            }
        }
    }
  return NULL;
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double G4PenelopeBremsstrahlungModel::GetPositronXSCorrection(const G4Material* mat,
								  G4double energy)
{
  //The electron-to-positron correction factor is set equal to the ratio of the 
  //radiative stopping powers for positrons and electrons, which has been calculated 
  //by Kim et al. (1986) (cf. Berger and Seltzer, 1982). Here, it is used an 
  //analytical approximation which reproduces the tabulated values with 0.5% 
  //accuracy
  G4double t=std::log(1.0+1e6*energy/
		      (electron_mass_c2*fPenelopeFSHelper->GetEffectiveZSquared(mat)));
  G4double corr = 1.0-std::exp(-t*(1.2359e-1-t*(6.1274e-2-t*
					   (3.1516e-2-t*(7.7446e-3-t*(1.0595e-3-t*
								      (7.0568e-5-t*
								       1.8080e-6)))))));
  return corr;
}
