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
// $Id$
//
/// \file B4dEventActionMessenger.hh
/// \brief Definition of the B4dEventActionMessenger class

#ifndef B4dEventActionMessenger_h
#define B4dEventActionMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class B4dEventAction;
class G4UIdirectory;
class G4UIcmdWithAnInteger;

/// Messenger class that defines commands for B4dEventAction class.
///
/// It implements commands:
/// - /B4/event/setPrintModulo value

class B4dEventActionMessenger: public G4UImessenger
{
public:
  B4dEventActionMessenger(B4dEventAction* eventAction);
  virtual ~B4dEventActionMessenger();
    
  virtual void SetNewValue(G4UIcommand* command, G4String value);
    
private:
  B4dEventAction*       fEventAction;
  G4UIdirectory*        fDirectory;   
  G4UIcmdWithAnInteger* fSetPrintModuloCmd;    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
