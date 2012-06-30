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
// $Id: RunAction.cc,v 1.2 2010-10-11 14:31:39 maire Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 

#include "RunAction.hh"
#include "HistoManager.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(HistoManager* histo, PrimaryGeneratorAction* kin)
:histoManager(histo), primary(kin)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run*)
{ 
  //initialize arrays
  //
  decayCount = timeCount = 0;
  for (G4int i=0; i<3; i++) EkinTot[i] = Pbalance[i] = EventTime[i] = 0. ;
  PrimaryTime = 0.;
          
  //histograms
  //
  histoManager->book();
  
  //inform the runManager to save random number seed
  //
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::ParticleCount(G4String name, G4double Ekin)
{
  particleCount[name]++;
  Emean[name] += Ekin;
  //update min max
  if (particleCount[name] == 1) Emin[name] = Emax[name] = Ekin;
  if (Ekin < Emin[name]) Emin[name] = Ekin;
  if (Ekin > Emax[name]) Emax[name] = Ekin;  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::Balance(G4double Ekin, G4double Pbal)
{
  decayCount++;
  EkinTot[0] += Ekin;
  //update min max  
  if (decayCount == 1) EkinTot[1] = EkinTot[2] = Ekin;
  if (Ekin < EkinTot[1]) EkinTot[1] = Ekin;
  if (Ekin > EkinTot[2]) EkinTot[2] = Ekin;
  
  Pbalance[0] += Pbal;
  //update min max   
  if (decayCount == 1) Pbalance[1] = Pbalance[2] = Pbal;  
  if (Pbal < Pbalance[1]) Pbalance[1] = Pbal;
  if (Pbal > Pbalance[2]) Pbalance[2] = Pbal;    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EventTiming(G4double time)
{
  timeCount++;  
  EventTime[0] += time;
  if (timeCount == 1) EventTime[1] = EventTime[2] = time;  
  if (time < EventTime[1]) EventTime[1] = time;
  if (time > EventTime[2]) EventTime[2] = time;	     
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::PrimaryTiming(G4double ptime)
{
  PrimaryTime += ptime;
}
    
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* run)
{
 G4int nbEvents = run->GetNumberOfEvent();
 if (nbEvents == 0) return;
 
 G4ParticleDefinition* particle = primary->GetParticleGun()
                                         ->GetParticleDefinition();
 G4String partName = particle->GetParticleName();
 G4double eprimary = primary->GetParticleGun()->GetParticleEnergy();
 
 G4cout << "\n ======================== run summary ======================";  
 G4cout << "\n The run was " << nbEvents << " " << partName << " of "
        << G4BestUnit(eprimary,"Energy");
 G4cout << "\n ===========================================================\n";
 G4cout << G4endl;

 G4int prec = 4, wid = prec + 2;
 G4int dfprec = G4cout.precision(prec);
      
 //particle count
 //
 G4cout << " Nb of generated particles: \n" << G4endl;
     
 std::map<G4String,G4int>::iterator it;               
 for (it = particleCount.begin(); it != particleCount.end(); it++) { 
    G4String name = it->first;
    G4int count   = it->second;
    G4double eMean = Emean[name]/count;
    G4double eMin = Emin[name], eMax = Emax[name];    
         
    G4cout << "  " << std::setw(13) << name << ": " << std::setw(7) << count
           << "  Emean = " << std::setw(wid) << G4BestUnit(eMean, "Energy")
	   << "\t( "  << G4BestUnit(eMin, "Energy")
	   << " --> " << G4BestUnit(eMax, "Energy") 
           << ")" << G4endl;	   
 }
 
 //energy momentum balance
 //
 G4double Ebmean = EkinTot[0]/decayCount;
 G4double Pbmean = Pbalance[0]/decayCount;
         
 G4cout << "\n   Ekin Total (Q): mean = "
        << std::setw(wid) << G4BestUnit(Ebmean, "Energy")
	<< "\t( "  << G4BestUnit(EkinTot[1], "Energy")
	<< " --> " << G4BestUnit(EkinTot[2], "Energy")
        << ")" << G4endl;    
	   
 G4cout << "\n   Momentum balance (excluding gamma desexcitation): mean = " 
        << std::setw(wid) << G4BestUnit(Pbmean, "Energy")
	<< "\t( "  << G4BestUnit(Pbalance[1], "Energy")
	<< " --> " << G4BestUnit(Pbalance[2], "Energy")
        << ")" << G4endl;
	    
 //total time of life
 //
 G4double Tmean = EventTime[0]/timeCount;
 G4double halfLife = Tmean*std::log(2.);
   
 G4cout << "\n   Total time of life : mean = "
            << std::setw(wid) << G4BestUnit(Tmean, "Time")
	    << "  half-life = "
	    << std::setw(wid) << G4BestUnit(halfLife, "Time")
	    << "   ( "  << G4BestUnit(EventTime[1], "Time")
	    << " --> "  << G4BestUnit(EventTime[2], "Time")
            << ")" << G4endl;
	    
 //activity of primary ion
 //
 G4double pTimeMean = PrimaryTime/nbEvents;
 G4double molMass = particle->GetAtomicMass()*g/mole;
 G4double nAtoms = Avogadro/molMass;
 G4double ActivPerAtom = 1./pTimeMean;
 G4double ActivPerMass = ActivPerAtom*nAtoms;
   
 G4cout << "\n   Activity of " << partName << " = "
            << std::setw(wid) << ActivPerMass*g/becquerel
	    << " Bq/g   ("    << ActivPerMass*g/curie
	    << " Ci/g) \n" 
	    << G4endl;
    	    	   	         
 // remove all contents in particleCount
 // 
 particleCount.clear(); 
 Emean.clear();  Emin.clear(); Emax.clear();

 // restore default precision
 // 
 G4cout.precision(dfprec);
            
 //save histograms
 //      
 histoManager->save();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
