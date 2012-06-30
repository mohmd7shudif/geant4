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
// $Id: OlapRunAction.hh,v 1.3 2006-06-29 17:22:38 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// --------------------------------------------------------------
// OlapRunAction
//
// Author: Martin Liendl - Martin.Liendl@cern.ch
//
// --------------------------------------------------------------
//
#ifndef OlapRunAction_h
#define OlapRunAction_h

#include <vector>
#include <fstream>
#include "G4UserRunAction.hh"
#include "OlapEventAction.hh"
#include "OlapLogManager.hh"

class OlapRunAction : public G4UserRunAction
{

public:

   OlapRunAction();
   ~OlapRunAction();
   
   void BeginOfRunAction(const G4Run* aRun);
   void EndOfRunAction(const G4Run* aRun);
   void DrawOlaps();
   
   std::vector<OlapInfo *> theOlaps;

private:

   std::ofstream FILE;
};
#endif
