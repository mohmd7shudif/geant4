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
// $Id: G4BertiniPiKBuilder.cc,v 1.3 2009-04-02 08:11:32 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4BertiniPiKBuilder
//
// Author: 2002 H.P. Wellisch
//
// Modified:
// 02.04.2009 V.Ivanchenko remove add cross section, string builderis reponsible 
//
//----------------------------------------------------------------------------
//
#include "G4BertiniPiKBuilder.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"

G4BertiniPiKBuilder::
G4BertiniPiKBuilder() 
 {
   thePiData = new G4PiNuclearCrossSection;
   theMin = 0*GeV;
   theMax = 9.9*GeV;
   theModel = new G4CascadeInterface;
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax); 
 }

G4BertiniPiKBuilder::~G4BertiniPiKBuilder() 
{
}

void G4BertiniPiKBuilder::
Build(G4PionPlusInelasticProcess * aP)
 {
   aP->RegisterMe(theModel);
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax);
 }

void G4BertiniPiKBuilder::
Build(G4PionMinusInelasticProcess * aP)
 {
   aP->RegisterMe(theModel);
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax);
 }

void G4BertiniPiKBuilder::
Build(G4HadronElasticProcess * ) {}

void G4BertiniPiKBuilder::
Build(G4KaonPlusInelasticProcess * aP)
 {
   aP->RegisterMe(theModel);
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax);
 }

void G4BertiniPiKBuilder::
Build(G4KaonMinusInelasticProcess * aP)
 {
   aP->RegisterMe(theModel);
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax);
 }

void G4BertiniPiKBuilder::
Build(G4KaonZeroLInelasticProcess * aP)
 {
   aP->RegisterMe(theModel);
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax);
 }

void G4BertiniPiKBuilder::
Build(G4KaonZeroSInelasticProcess * aP)
 {
   aP->RegisterMe(theModel);
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax);
 }

 // 2002 by J.P. Wellisch
