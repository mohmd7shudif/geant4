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
// $Id: G4QGSPProtonBuilder.cc,v 1.8 2010-11-18 14:52:22 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4QGSPProtonBuilder
//
// Author: 2002 J.P. Wellisch
//
// Modified:
// 17.11.2010 G.Folger, use G4CrossSectionPairGG for relativistic rise of cross
//             section at high energies.
// 30.03.2009 V.Ivanchenko create cross section by new
//
//----------------------------------------------------------------------------
//
#include "G4QGSPProtonBuilder.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4ProtonInelasticCrossSection.hh"
#include "G4CrossSectionPairGG.hh"

G4QGSPProtonBuilder::
G4QGSPProtonBuilder(G4bool quasiElastic, G4bool projectileDiffraction) 
 {
   theMin = 12*GeV;
   theModel = new G4TheoFSGenerator("QGSP");

   theStringModel = new G4QGSModel< G4QGSParticipants >;
   theStringDecay = new G4ExcitedStringDecay(theQGSM = new G4QGSMFragmentation);
   theStringModel->SetFragmentationModel(theStringDecay);

   theCascade = new G4GeneratorPrecompoundInterface;
   thePreEquilib = new G4PreCompoundModel(theHandler = new G4ExcitationHandler);
   theCascade->SetDeExcitation(thePreEquilib);  

   theModel->SetTransport(theCascade);
   theModel->SetHighEnergyGenerator(theStringModel);
   if (quasiElastic)
   {
      theQuasiElastic=new G4QuasiElasticChannel;
      theModel->SetQuasiElasticChannel(theQuasiElastic);
   } else 
   {  theQuasiElastic=0;}  
   if ( projectileDiffraction )
   {
      theProjectileDiffraction=new G4ProjectileDiffractiveChannel;
      theModel->SetProjectileDiffraction(theProjectileDiffraction);
   } else 
   {  theProjectileDiffraction=0;}
 }

void G4QGSPProtonBuilder::
Build(G4ProtonInelasticProcess * aP)
 {
   // G4cout << "adding inelastic Proton in QGSP" << G4endl;
   aP->AddDataSet(new G4CrossSectionPairGG(
   		new G4ProtonInelasticCrossSection(), 91*GeV));  
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(100*TeV);
   aP->RegisterMe(theModel);
 }

void G4QGSPProtonBuilder::
Build(G4HadronElasticProcess * )
 {
 }

G4QGSPProtonBuilder::~G4QGSPProtonBuilder() 
 {
   delete thePreEquilib;
   delete theCascade;
   if ( theQuasiElastic ) delete theQuasiElastic;
   if ( theProjectileDiffraction ) delete theProjectileDiffraction;
   delete theStringDecay;
   delete theStringModel;
   delete theModel;
   delete theQGSM;
   delete theHandler;
 }

 // 2002 by J.P. Wellisch
