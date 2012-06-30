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
// $Id: G4VisCommandsDefault.hh,v 1.3 2006-06-29 21:28:34 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $

// /vis/default commands - John Allison  30th October 2011

#ifndef G4VISCOMMANDSDEFAULT_HH
#define G4VISCOMMANDSDEFAULT_HH

#include "G4VVisCommand.hh"

class G4UIcmdWithAString;
class G4UIcmdWithABool;

class G4VisCommandDefaultStyle: public G4VVisCommand {
public:
  G4VisCommandDefaultStyle();
  virtual ~G4VisCommandDefaultStyle();
  G4String GetCurrentValue(G4UIcommand* command);
  void SetNewValue(G4UIcommand* command, G4String newValue);
private:
  G4VisCommandDefaultStyle(const G4VisCommandDefaultStyle&);
  G4VisCommandDefaultStyle& operator=(const G4VisCommandDefaultStyle&);
  G4UIcmdWithAString* fpCommand;
};

class G4VisCommandDefaultHiddenEdge: public G4VVisCommand {
public:
  G4VisCommandDefaultHiddenEdge();
  virtual ~G4VisCommandDefaultHiddenEdge();
  G4String GetCurrentValue(G4UIcommand* command);
  void SetNewValue(G4UIcommand* command, G4String newValue);
private:
  G4VisCommandDefaultHiddenEdge(const G4VisCommandDefaultHiddenEdge&);
  G4VisCommandDefaultHiddenEdge& operator=(const G4VisCommandDefaultHiddenEdge&);
  G4UIcmdWithABool* fpCommand;
};

#endif
