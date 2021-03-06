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
// $Id: G4VPionBuilder.hh,v 1.1 2010-11-19 16:21:40 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4VPionBuilder
//
// Author: 2010 G.Folger
//  devired from G4VPiKBuilder
//
// Modified:
//
//----------------------------------------------------------------------------
//
#ifndef G4VPionBuilder_h
#define G4VPionBuilder_h

#include "G4HadronElasticProcess.hh"
#include "G4PionPlusInelasticProcess.hh"
#include "G4PionMinusInelasticProcess.hh"

class G4VPionBuilder
{
public:
  G4VPionBuilder();
  virtual ~G4VPionBuilder();
  virtual void Build(G4HadronElasticProcess * aP) = 0;
  virtual void Build(G4PionPlusInelasticProcess * aP) = 0;
  virtual void Build(G4PionMinusInelasticProcess * aP) = 0;
};
#endif
