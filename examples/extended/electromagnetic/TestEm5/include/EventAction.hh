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
// $Id: EventAction.hh,v 1.3 2006-06-29 16:54:35 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class RunAction;
class HistoManager;
class EventMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EventAction : public G4UserEventAction
{
  public:
    EventAction(RunAction* , HistoManager*);
   ~EventAction();

  public:
    void BeginOfEventAction(const G4Event*);
    void   EndOfEventAction(const G4Event*);
    
    void AddEnergy      (G4double edep)   {EnergyDeposit  += edep;};
    void AddTrakLenCharg(G4double length) {TrakLenCharged += length;};
    void AddTrakLenNeutr(G4double length) {TrakLenNeutral += length;};
    
    void CountStepsCharg ()               {nbStepsCharged++ ;};
    void CountStepsNeutr ()               {nbStepsNeutral++ ;};
    
    void SetTransmitFlag (G4int flag) 
                           {if (flag > TransmitFlag) TransmitFlag = flag;};
    void SetReflectFlag  (G4int flag) 
                           {if (flag > ReflectFlag)   ReflectFlag = flag;};
			           	  
    void SetDrawFlag(G4String val)  {drawFlag = val;};
    void SetPrintModulo(G4int val)  {printModulo = val;};
        
  private:
    RunAction*    runaction;
    HistoManager* histoManager;
    
    G4double EnergyDeposit;
    G4double TrakLenCharged, TrakLenNeutral;
    G4int    nbStepsCharged, nbStepsNeutral;
    G4int    TransmitFlag,   ReflectFlag; 
    
    G4String drawFlag;
    G4int    printModulo;
    
    EventMessenger* eventMessenger;                    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
