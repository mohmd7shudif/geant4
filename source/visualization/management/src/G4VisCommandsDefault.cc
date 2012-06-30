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
// $Id: G4VisCommandsDefault.cc,v 1.4 2007-01-05 16:29:38 allison Exp $
// GEANT4 tag $Name: not supported by cvs2svn $

// /vis/default commands - John Allison  30th October 2011

#include "G4VisCommandsDefault.hh"

#include "G4VisManager.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"

////////////// /vis/default/hiddenEdge ///////////////////////////////////////

G4VisCommandDefaultHiddenEdge::G4VisCommandDefaultHiddenEdge()
{
  G4bool omitable;
  fpCommand = new G4UIcmdWithABool("/vis/default/hiddenEdge", this);
  fpCommand->SetGuidance("Default hiddenEdge drawing for future viewers.");
  fpCommand->SetGuidance
    ("Edges become hidden/seen in wireframe or surface mode.");
  fpCommand->SetParameterName("hidden-edge",omitable = true);
  fpCommand->SetDefaultValue(true);
}

G4VisCommandDefaultHiddenEdge::~G4VisCommandDefaultHiddenEdge()
{
  delete fpCommand;
}

G4String G4VisCommandDefaultHiddenEdge::GetCurrentValue(G4UIcommand*)
{
  return "";
}

void G4VisCommandDefaultHiddenEdge::SetNewValue(G4UIcommand*, G4String newValue)
{
  // Follows /vis/viewer/set/hiddenEdge

  G4VisManager::Verbosity verbosity = fpVisManager->GetVerbosity();
  G4ViewParameters vp = fpVisManager->GetDefaultViewParameters();
  G4ViewParameters::DrawingStyle existingStyle = vp.GetDrawingStyle();

  if (G4UIcommand::ConvertToBool(newValue)) {
    switch (existingStyle) {
    case G4ViewParameters::wireframe:
      vp.SetDrawingStyle(G4ViewParameters::hlr);
      break;
    case G4ViewParameters::hlr:
      break;
    case G4ViewParameters::hsr:
      vp.SetDrawingStyle(G4ViewParameters::hlhsr);
      break;
    case G4ViewParameters::hlhsr:
      break;
    }
  }
  else {
    switch (existingStyle) {
    case G4ViewParameters::wireframe:
      break;
    case G4ViewParameters::hlr:
      vp.SetDrawingStyle(G4ViewParameters::wireframe);
      break;
    case G4ViewParameters::hsr:
      break;
    case G4ViewParameters::hlhsr:
      vp.SetDrawingStyle(G4ViewParameters::hsr);
      break;
    }
  }

  fpVisManager->SetDefaultViewParameters(vp);

  if (verbosity >= G4VisManager::confirmations) {
    G4cout << "Default drawing style set to " << vp.GetDrawingStyle()
	   << G4endl;
  }
}

////////////// /vis/default/style ///////////////////////////////////////

G4VisCommandDefaultStyle::G4VisCommandDefaultStyle()
{
  G4bool omitable;
  fpCommand = new G4UIcmdWithAString("/vis/default/style", this);
  fpCommand->SetGuidance("Default drawing style for future viewers.");
  fpCommand->SetGuidance
    ("Set style of drawing - w[ireframe] or s[urface].");
  fpCommand->SetGuidance 
    ("(Default hidden line drawing is controlled by \"/vis/default/hiddenEdge\".)");
  fpCommand->SetParameterName ("style",omitable = false);
}

G4VisCommandDefaultStyle::~G4VisCommandDefaultStyle()
{
  delete fpCommand;
}

G4String G4VisCommandDefaultStyle::GetCurrentValue(G4UIcommand*)
{
  return "";
}

void G4VisCommandDefaultStyle::SetNewValue(G4UIcommand*, G4String newValue)
{
  // Follows /vis/viewer/set/style

  G4VisManager::Verbosity verbosity = fpVisManager->GetVerbosity();
  G4ViewParameters vp = fpVisManager->GetDefaultViewParameters();
  G4ViewParameters::DrawingStyle existingStyle = vp.GetDrawingStyle();

  const size_t iPos0 = 0;
  if (newValue[iPos0] == 'w') {  // "wireframe"
    switch (existingStyle) {
    case G4ViewParameters::wireframe:
      break;
    case G4ViewParameters::hlr:
      break;
    case G4ViewParameters::hsr:
      vp.SetDrawingStyle(G4ViewParameters::wireframe);
      break;
    case G4ViewParameters::hlhsr:
      vp.SetDrawingStyle(G4ViewParameters::hlr);
      break;
    }
  }
  else if (newValue[iPos0] == 's') {  // "surface"
    switch (existingStyle) {
    case G4ViewParameters::wireframe:
      vp.SetDrawingStyle(G4ViewParameters::hsr);
      break;
    case G4ViewParameters::hlr:
      vp.SetDrawingStyle(G4ViewParameters::hlhsr);
      break;
    case G4ViewParameters::hsr:
      break;
    case G4ViewParameters::hlhsr:
      break;
    }
  }
  else {
    if (verbosity >= G4VisManager::errors) {
      G4cout << "ERROR: \"" << newValue << "\" not recognised."
	"  Looking for 'w' or 's' first character." << G4endl;
    }
    return;
  }

  fpVisManager->SetDefaultViewParameters(vp);

  if (verbosity >= G4VisManager::confirmations) {
    G4cout << "Default drawing style set to " << vp.GetDrawingStyle()
	   << G4endl;
  }
}
