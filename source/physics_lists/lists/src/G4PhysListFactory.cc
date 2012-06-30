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
// $Id: G4PhysListFactory.cc,v 1.16 2010-11-19 19:50:15 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---------------------------------------------------------------------------
//
// ClassName:  G4PhysListFactory
//
// Author: 21 April 2008 V. Ivanchenko
//
// Modified:
//
//----------------------------------------------------------------------------
//

#include "G4PhysListFactory.hh"
#include "CHIPS.hh"
#include "FTFP_BERT.hh"
#include "FTFP_BERT_TRV.hh"
#include "FTF_BIC.hh"
#include "LBE.hh"
#include "LHEP.hh"
#include "QBBC.hh"
#include "QGSC_BERT.hh"
#include "QGSP_BERT.hh"
#include "QGSP_BERT_CHIPS.hh"
#include "QGSP_BERT_HP.hh"
#include "QGSP_BIC.hh"
#include "QGSP_BIC_HP.hh"
#include "QGSP_FTFP_BERT.hh"
#include "QGS_BIC.hh"
#include "QGSP_INCLXX.hh"
#include "QGSP_INCL_ABLA.hh"
#include "Shielding.hh"

#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"

G4PhysListFactory::G4PhysListFactory() 
  : defName("FTFP_BERT"),verbose(1)
{
  nlists_hadr = 19;
  G4String s[19] = {
    "CHIPS",
    "FTFP_BERT","FTFP_BERT_TRV","FTF_BIC",
    "LBE","LHEP","QBBC",
    "QGSC_BERT","QGSP","QGSP_BERT","QGSP_BERT_CHIPS","QGSP_BERT_HP",
    "QGSP_BIC","QGSP_BIC_HP",
    "QGSP_FTFP_BERT","QGS_BIC","QGSP_INCLXX","QGSP_INCL_ABLA",
    "Shielding"};
  for(size_t i=0; i<nlists_hadr; ++i) {
    listnames_hadr.push_back(s[i]);
  }

  nlists_em = 6;
  G4String s1[6] = {"","_EMV","_EMX","_EMY","_LIV","_PEN"};
  for(size_t i=0; i<nlists_em; ++i) {
    listnames_em.push_back(s1[i]);
  }
}

G4PhysListFactory::~G4PhysListFactory()
{}

G4VModularPhysicsList* 
G4PhysListFactory::ReferencePhysList()
{
  // instantiate PhysList by environment variable "PHYSLIST"
  G4String name = "";
  char* path = getenv("PHYSLIST");
  if (path) {
    name = G4String(path);
  } else {
    name = defName;
    G4cout << "### G4PhysListFactory WARNING: "
	   << " environment variable PHYSLIST is not defined"
	   << G4endl
	   << "    Default Physics Lists " << name 
	   << " is instantiated" 
	   << G4endl;
  }
  return GetReferencePhysList(name);
}

G4VModularPhysicsList* 
G4PhysListFactory::GetReferencePhysList(const G4String& name)
{
  // analysis on the string 
  size_t n = name.size();

  // last characters in the string
  size_t em_opt = 0;
  G4String em_name = "";

  // check EM options
  if(n > 4) {
    em_name = name.substr(n - 4, 4);
    for(size_t i=1; i<nlists_em; ++i) { 
      if(listnames_em[i] == em_name) { 
	em_opt = i;
        n -= 4;
        break; 
      }
    }
    if(0 == em_opt) { em_name = ""; }
  }

  // hadronic pHysics List
  G4String had_name = name.substr(0, n);

  if(0 < verbose) {
    G4cout << "G4PhysListFactory::GetReferencePhysList <" << had_name
	   << em_name << ">  EMoption= " << em_opt << G4endl;
  }
  G4VModularPhysicsList* p = 0;
  if(had_name == "CHIPS")               {p = new CHIPS(verbose);}
  else if(had_name == "FTFP_BERT")      {p = new FTFP_BERT(verbose);}
  else if(had_name == "FTFP_BERT_TRV")  {p = new FTFP_BERT_TRV(verbose);}
  else if(had_name == "FTF_BIC")        {p = new FTF_BIC(verbose);}
  else if(had_name == "LBE")            {p = new LBE();}
  else if(had_name == "LHEP")           {p = new LHEP(verbose);}
  else if(had_name == "QBBC")           {p = new QBBC(verbose);}
  else if(had_name == "QGSC_BERT")      {p = new QGSC_BERT(verbose);}
  else if(had_name == "QGSP_BERT")      {p = new QGSP_BERT(verbose);}
  else if(had_name == "QGSP_BERT_CHIPS"){p = new QGSP_BERT_CHIPS(verbose);}
  else if(had_name == "QGSP_BERT_HP")   {p = new QGSP_BERT_HP(verbose);}
  else if(had_name == "QGSP_BIC")       {p = new QGSP_BIC(verbose);}
  else if(had_name == "QGSP_BIC_HP")    {p = new QGSP_BIC_HP(verbose);}
  else if(had_name == "QGSP_FTFP_BERT") {p = new QGSP_FTFP_BERT(verbose);}
  else if(had_name == "QGS_BIC")        {p = new QGS_BIC(verbose);}
  else if(had_name == "QGSP_INCLXX")    {p = new QGSP_INCLXX(verbose);}
  else if(had_name == "QGSP_INCL_ABLA") {p = new QGSP_INCL_ABLA(verbose);}
  else if(had_name == "Shielding")      {p = new Shielding(verbose);}
  else if(had_name == "ShieldingLEND")  {p = new Shielding(verbose,"LEND");}
  else {
    G4cout << "### G4PhysListFactory WARNING: "
	   << "PhysicsList " << had_name << " is not known"
	   << G4endl;
  }
  if(p) {
    G4cout << "<<< Reference Physics List " << had_name
	   << em_name << " is built" << G4endl;
    G4int ver = p->GetVerboseLevel();
    p->SetVerboseLevel(0);
    if(0 < em_opt) {
      if(1 == em_opt) { 
	p->ReplacePhysics(new G4EmStandardPhysics_option1(verbose)); 
      } else if(2 == em_opt) {
	p->ReplacePhysics(new G4EmStandardPhysics_option2(verbose)); 
      } else if(3 == em_opt) {
	p->ReplacePhysics(new G4EmStandardPhysics_option3(verbose)); 
      } else if(4 == em_opt) {
	p->ReplacePhysics(new G4EmLivermorePhysics(verbose)); 
      } else if(5 == em_opt) {
	p->ReplacePhysics(new G4EmPenelopePhysics(verbose)); 
      }
    }
    p->SetVerboseLevel(ver);
  }
  G4cout << G4endl;
  return p;
}
  
G4bool G4PhysListFactory::IsReferencePhysList(const G4String& name)
{
  G4bool res = false;
  size_t n = name.size();
  if(n > 4) {
    G4String em_name = name.substr(n - 4, 4);
    for(size_t i=1; i<nlists_em; ++i) { 
      if(listnames_em[i] == em_name) { 
        n -= 4;
        break; 
      }
    }
  }
  G4String had_name = name.substr(0, n);
  for(size_t i=0; i<nlists_hadr; ++i) {
    if(had_name == listnames_hadr[i]) {
      res = true;
      break;
    }
  }
  return res;
}

const std::vector<G4String>& 
G4PhysListFactory::AvailablePhysLists() const
{
  return listnames_hadr;
}

const std::vector<G4String>& 
G4PhysListFactory::AvailablePhysListsEM() const
{
  return listnames_em;
}

