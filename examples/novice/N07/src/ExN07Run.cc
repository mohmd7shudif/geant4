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
// $Id: ExN07Run.cc,v 1.6 2007-05-04 01:49:28 asaim Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//

#include "ExN07Run.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"

ExN07Run::ExN07Run()
{
  G4String detName[6] = {"Calor-A_abs","Calor-A_gap","Calor-B_abs","Calor-B_gap","Calor-C_abs","Calor-C_gap"};
  G4String primNameSum[6] = {"eDep","nGamma","nElectron","nPositron","trackLength","nStep"};
  G4String primNameMin[3] = {"minEkinGamma","minEkinElectron","minEkinPositron"};
  G4String paraName[3] = {"Calor-AP_para","Calor-BP_para","Calor-CP_para"};

  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
  G4String fullName;
  size_t i,j;
  for(i=0;i<6;i++)
  {
    for(j=0;j<6;j++)
    {
      fullName = detName[i]+"/"+primNameSum[j];
      colIDSum[i][j] = SDMan->GetCollectionID(fullName);
    }
    for(j=0;j<3;j++)
    {
      fullName = detName[i]+"/"+primNameMin[j];
      colIDMin[i][j] = SDMan->GetCollectionID(fullName);
    }
  }
  for(i=0;i<3;i++)
  {
    for(j=0;j<6;j++)
    {
      fullName = paraName[i]+"/"+primNameSum[j];
      colIDPara[i][j] = SDMan->GetCollectionID(fullName);
    }
  }
}

ExN07Run::~ExN07Run()
{;}

void ExN07Run::RecordEvent(const G4Event* evt)
{
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  if(!HCE) return;
  numberOfEvent++;
  size_t i,j;
  for(i=0;i<6;i++)
  {
    for(j=0;j<6;j++)
    {
      G4THitsMap<G4double>* evtMap = (G4THitsMap<G4double>*)(HCE->GetHC(colIDSum[i][j]));
      mapSum[i][j] += *evtMap;
    }
    for(j=0;j<3;j++)
    {
      G4THitsMap<G4double>* evtMap = (G4THitsMap<G4double>*)(HCE->GetHC(colIDMin[i][j]));
      std::map<G4int,G4double*>::iterator itr = evtMap->GetMap()->begin();
      for(; itr != evtMap->GetMap()->end(); itr++)
      {
        G4int key = (itr->first);
        G4double val = *(itr->second);
        G4double* mapP = mapMin[i][j][key];
        if( mapP && (val>*mapP) ) continue;
        mapMin[i][j].set(key,val);
      }
    }
  }
  for(i=0;i<3;i++)
  {
    for(j=0;j<6;j++)
    {
      G4THitsMap<G4double>* evtMap = (G4THitsMap<G4double>*)(HCE->GetHC(colIDPara[i][j]));
      mapPara[i][j] += *evtMap;
    }
  }
}

G4double ExN07Run::GetTotal(const G4THitsMap<G4double> &map) const
{
  G4double tot = 0.;
  std::map<G4int,G4double*>::iterator itr = map.GetMap()->begin();
  for(; itr != map.GetMap()->end(); itr++) 
  { tot += *(itr->second); }
  return tot;
}

G4double ExN07Run::FindMinimum(const G4THitsMap<G4double> &map) const
{
  G4double val = DBL_MAX;
  std::map<G4int,G4double*>::iterator itr = map.GetMap()->begin();
  for(; itr != map.GetMap()->end(); itr++) 
  { if(val>*(itr->second)) val = *(itr->second); }
  return val;
}


  
