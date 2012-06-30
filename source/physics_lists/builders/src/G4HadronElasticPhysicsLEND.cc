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
// $Id: G4HadronElasticPhysicsLEND.cc,v 1.3 2010-09-23 18:53:20 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4HadronElasticPhysicsLEND
//
// Author: 02 June 2011 Tatsumi Koi
//
// Modified:
// 03.06.2011 V.Ivanchenko change design - now first default constructor 
//            is called, LEND model and cross section are added on top
//
//----------------------------------------------------------------------------
//
// LEND model for n with E < 20 MeV
// LEND cross sections for n n with E < 20 MeV

#include "G4HadronElasticPhysicsLEND.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4Neutron.hh"
#include "G4HadronicProcess.hh"
#include "G4HadronElastic.hh"

#include "G4LENDElastic.hh"
#include "G4LENDElasticCrossSection.hh"

G4HadronElasticPhysicsLEND::G4HadronElasticPhysicsLEND(G4int ver,G4String eva)
  : G4VPhysicsConstructor("hElasticWEL_CHIPS_LEND"), verbose(ver), 
    wasActivated(false),
    evaluation(eva)
{
  if(verbose > 1) { 
    G4cout << "### G4HadronElasticPhysicsLEND: " << GetPhysicsName() 
	   << G4endl; 
  }
  mainElasticBuilder = new G4HadronElasticPhysics(verbose);
}

G4HadronElasticPhysicsLEND::~G4HadronElasticPhysicsLEND()
{
  delete mainElasticBuilder;
}

void G4HadronElasticPhysicsLEND::ConstructParticle()
{
  // G4cout << "G4HadronElasticPhysics::ConstructParticle" << G4endl;
  mainElasticBuilder->ConstructParticle();
}

void G4HadronElasticPhysicsLEND::ConstructProcess()
{
  if(wasActivated) return;
  wasActivated = true;

  mainElasticBuilder->ConstructProcess();

  mainElasticBuilder->GetNeutronModel()->SetMinEnergy(19.5*MeV);

  G4HadronicProcess* hel = mainElasticBuilder->GetNeutronProcess();

  G4LENDElastic* lend = new G4LENDElastic( G4Neutron::Neutron() );
  if ( evaluation.size() > 0 ) lend->ChangeDefaultEvaluation( evaluation );
  //lend->AllowNaturalAbundanceTarget();
  lend->AllowAnyCandidateTarget();
  hel->RegisterMe(lend);
  G4LENDElasticCrossSection* lend_XS = new G4LENDElasticCrossSection( G4Neutron::Neutron() );
  if ( evaluation.size() > 0 ) lend_XS->ChangeDefaultEvaluation( evaluation );
  //lend_XS->AllowNaturalAbundanceTarget();
  lend_XS->AllowAnyCandidateTarget();
  hel->AddDataSet( lend_XS );

  if(verbose > 1) {
    G4cout << "### HadronElasticPhysicsLEND is constructed" 
	   << G4endl;
  }
}


