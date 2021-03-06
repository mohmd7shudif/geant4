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
// $Id: G4OpenGLViewerMessenger.cc,v 1.22 2011-01-07 09:47:23 allison Exp $
// GEANT4 tag $Name: not supported by cvs2svn $

#ifdef G4VIS_BUILD_OPENGL_DRIVER

#include "G4OpenGLViewerMessenger.hh"

#include "G4OpenGLViewer.hh"
#include "G4OpenGLStoredViewer.hh"
#include "G4UImanager.hh"
#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4VisManager.hh"
#include <sstream>

G4OpenGLViewerMessenger*
G4OpenGLViewerMessenger::fpInstance = 0;

G4OpenGLViewerMessenger*
G4OpenGLViewerMessenger::GetInstance()
{
  if (!fpInstance) fpInstance = new G4OpenGLViewerMessenger;
  return fpInstance;
}

G4OpenGLViewerMessenger::G4OpenGLViewerMessenger()
{
  G4bool omitable;

  fpDirectory = new G4UIdirectory("/vis/ogl/");
  fpDirectory->SetGuidance("G4OpenGLViewer commands.");

  fpDirectorySet = new G4UIdirectory ("/vis/ogl/set/");
  fpDirectorySet->SetGuidance("G4OpenGLViewer set commands.");

  G4UIparameter* parameter;

  fpCommandDisplayHeadTime =
    new G4UIcommand("/vis/ogl/set/displayHeadTime", this);
  fpCommandDisplayHeadTime->SetGuidance
    ("Display head time of range in 2D text.");
  parameter = new G4UIparameter ("displayHeadTime", 'b', omitable = false);
  parameter->SetDefaultValue(false);
  fpCommandDisplayHeadTime->SetParameter(parameter);
  parameter = new G4UIparameter ("screenX", 'd', omitable = true);
  parameter->SetGuidance("-1 < screenX < 1");
  parameter->SetParameterRange("screenX >= -1. && screenX <= 1.");
  parameter->SetDefaultValue(-0.9);
  fpCommandDisplayHeadTime->SetParameter(parameter);
  parameter = new G4UIparameter ("screenY", 'd', omitable = true);
  parameter->SetGuidance("-1 < screenY < 1");
  parameter->SetParameterRange("screenY >= -1. && screenY <= 1.");
  parameter->SetDefaultValue(-0.9);
  fpCommandDisplayHeadTime->SetParameter(parameter);
  parameter = new G4UIparameter ("screenSize", 'd', omitable = true);
  parameter->SetDefaultValue(24.);
  fpCommandDisplayHeadTime->SetParameter(parameter);
  parameter = new G4UIparameter ("red", 'd', omitable = true);
  parameter->SetParameterRange("red >= 0. && red <= 1.");
  parameter->SetDefaultValue(0.);
  fpCommandDisplayHeadTime->SetParameter(parameter);
  parameter = new G4UIparameter ("green", 'd', omitable = true);
  parameter->SetParameterRange("green >= 0. && green <= 1.");
  parameter->SetDefaultValue(1.);
  fpCommandDisplayHeadTime->SetParameter(parameter);
  parameter = new G4UIparameter ("blue", 'd', omitable = true);
  parameter->SetParameterRange("blue >= 0. && blue <= 1.");
  parameter->SetDefaultValue(1.);
  fpCommandDisplayHeadTime->SetParameter(parameter);

  fpCommandDisplayLightFront =
    new G4UIcommand("/vis/ogl/set/displayLightFront", this);
  fpCommandDisplayLightFront->SetGuidance
    ("Display the light front at head time.");
  fpCommandDisplayLightFront->SetGuidance
    ("Tip: The trajectories can appear of jump ahead of the light front"
     "\nbecause their time range overlaps the viewer's time range.  To"
     "\naverage out this discrete time effect, advance the light front by"
     "\nhalf the trajectories interval. E.g., if the trajectory time slice"
     "\ninterval is 0.01 ns:"
     "\n  /vis/ogl/set/displayLightFront true -90 0 0 mm -0.005 ns"
     "\nTo prevent them beating the light front at all:"
     "\n  /vis/ogl/set/displayLightFront true -90 0 0 mm -0.01 ns");
  parameter = new G4UIparameter ("displayLightFront", 'b', omitable = false);
  parameter->SetDefaultValue(false);
  fpCommandDisplayLightFront->SetParameter(parameter);
  parameter = new G4UIparameter ("originX", 'd', omitable = true);
  parameter->SetDefaultValue(0.);
  fpCommandDisplayLightFront->SetParameter(parameter);
  parameter = new G4UIparameter ("originY", 'd', omitable = true);
  parameter->SetDefaultValue(0.);
  fpCommandDisplayLightFront->SetParameter(parameter);
  parameter = new G4UIparameter ("originZ", 'd', omitable = true);
  parameter->SetDefaultValue(0.);
  fpCommandDisplayLightFront->SetParameter(parameter);
  parameter = new G4UIparameter ("space_unit", 's', omitable = true);
  parameter->SetDefaultValue("m");
  fpCommandDisplayLightFront->SetParameter(parameter);
  parameter = new G4UIparameter ("originT", 'd', omitable = true);
  parameter->SetDefaultValue(0.);
  fpCommandDisplayLightFront->SetParameter(parameter);
  parameter = new G4UIparameter ("time_unit", 's', omitable = true);
  parameter->SetDefaultValue("s");
  fpCommandDisplayLightFront->SetParameter(parameter);
  parameter = new G4UIparameter ("red", 'd', omitable = true);
  parameter->SetParameterRange("red >= 0. && red <= 1.");
  parameter->SetDefaultValue(0.);
  fpCommandDisplayLightFront->SetParameter(parameter);
  parameter = new G4UIparameter ("green", 'd', omitable = true);
  parameter->SetParameterRange("green >= 0. && green <= 1.");
  parameter->SetDefaultValue(1.);
  fpCommandDisplayLightFront->SetParameter(parameter);
  parameter = new G4UIparameter ("blue", 'd', omitable = true);
  parameter->SetParameterRange("blue >= 0. && blue <= 1.");
  parameter->SetDefaultValue(0.);
  fpCommandDisplayLightFront->SetParameter(parameter);

  fpCommandDisplayListLimit =
    new G4UIcmdWithAnInteger("/vis/ogl/set/displayListLimit", this);
  fpCommandDisplayListLimit->SetGuidance
    ("Set/reset display list limit (to avoid memory exhaustion).");
  fpCommandDisplayListLimit->SetParameterName("limit", omitable = true);
  fpCommandDisplayListLimit->SetDefaultValue(50000);
  fpCommandDisplayListLimit->SetRange("limit>=10000");

  fpCommandEndTime =
    new G4UIcommand("/vis/ogl/set/endTime", this);
  fpCommandEndTime->SetGuidance("Set end and range of track time.");
  parameter = new G4UIparameter ("end-time", 'd', omitable = false);
  parameter->SetDefaultValue(DBL_MAX);
  fpCommandEndTime->SetParameter(parameter);
  parameter = new G4UIparameter ("end-time-unit", 's', omitable = false);
  parameter->SetDefaultValue("ns");
  fpCommandEndTime->SetParameter(parameter);
  parameter = new G4UIparameter ("time-range", 'd', omitable = true);
  parameter->SetDefaultValue(-1.);
  fpCommandEndTime->SetParameter(parameter);
  parameter = new G4UIparameter ("time-range-unit", 's', omitable = true);
  parameter->SetDefaultValue("ns");
  fpCommandEndTime->SetParameter(parameter);

  fpCommandFade = new G4UIcmdWithADouble("/vis/ogl/set/fade", this);
  fpCommandFade->SetGuidance
    ("0: no fade; 1: maximum fade with time within range.");
  fpCommandFade->SetParameterName("fadefactor", omitable = false);
  fpCommandFade->SetRange("fadefactor>=0.&&fadefactor<=1.");
  fpCommandFade->SetDefaultValue(0.);

  fpCommandPrintEPS =
    new G4UIcmdWithoutParameter("/vis/ogl/printEPS", this);
  fpCommandPrintEPS->SetGuidance("Print Encapsulated PostScript file.");
  fpCommandPrintEPS->SetGuidance
    ("Generates files with names G4OpenGL_n.eps, where n is a sequence"
     "\nnumber, starting at 0."
     "\nCan be \"vectored\" or \"pixmap\" - see \"/vis/ogl/set/printMode\".");

  fpCommandPrintFilename =
    new G4UIcommand("/vis/ogl/set/printFilename", this);
  fpCommandPrintFilename->SetGuidance ("Set print filename");
  fpCommandPrintFilename->SetGuidance ("Setting 'incremental' will increment filename by one at each new print, starting at 0");
  G4UIparameter* parameterPrintFilename;
  parameterPrintFilename = new G4UIparameter ("name", 's', omitable = true);
  parameterPrintFilename->SetDefaultValue("G4OpenGL");
  fpCommandPrintFilename->SetParameter(parameterPrintFilename);
  parameterPrintFilename = new G4UIparameter ("incremental", 'b', omitable = true);
  parameterPrintFilename->SetDefaultValue(1);
  fpCommandPrintFilename->SetParameter(parameterPrintFilename);

  fpCommandPrintMode = new G4UIcmdWithAString
    ("/vis/ogl/set/printMode",this);
  fpCommandPrintMode->SetGuidance("Set print mode");
  fpCommandPrintMode->SetParameterName("print_mode",omitable = true);
  fpCommandPrintMode->SetCandidates("vectored pixmap");
  fpCommandPrintMode->SetDefaultValue("vectored");

  fpCommandPrintSize =
    new G4UIcommand("/vis/ogl/set/printSize", this);
  fpCommandPrintSize->SetGuidance ("Set print size");
  fpCommandPrintSize->SetGuidance ("Tip : -1 will mean 'print size' = 'window size'");
  fpCommandPrintSize->SetGuidance ("       Setting size greatter than your maximum graphic card capacity , will set the size to maximum  size.");
  G4UIparameter* parameterPrintSize;
  parameterPrintSize = new G4UIparameter ("width", 'd', omitable = false);
  parameterPrintSize->SetDefaultValue(-1);
  fpCommandPrintSize->SetParameter(parameterPrintSize);
  parameterPrintSize = new G4UIparameter ("height", 'd', omitable = false);
  parameterPrintSize->SetDefaultValue(-1);
  fpCommandPrintSize->SetParameter(parameterPrintSize);

  fpCommandStartTime =
    new G4UIcommand("/vis/ogl/set/startTime", this);
  fpCommandStartTime->SetGuidance("Set start and range of track time.");
  parameter = new G4UIparameter ("start-time", 'd', omitable = false);
  parameter->SetDefaultValue(-DBL_MAX);
  fpCommandStartTime->SetParameter(parameter);
  parameter = new G4UIparameter ("start-time-unit", 's', omitable = false);
  parameter->SetDefaultValue("ns");
  fpCommandStartTime->SetParameter(parameter);
  parameter = new G4UIparameter ("time-range", 'd', omitable = true);
  parameter->SetDefaultValue(-1.);
  fpCommandStartTime->SetParameter(parameter);
  parameter = new G4UIparameter ("time-range-unit", 's', omitable = true);
  parameter->SetDefaultValue("ns");
  fpCommandStartTime->SetParameter(parameter);

  fpCommandTransparency =
    new G4UIcmdWithABool("/vis/ogl/set/transparency", this);
  fpCommandTransparency->SetGuidance
    ("True/false to enable/disable rendering of transparent objects.");
  fpCommandTransparency->SetParameterName
    ("transparency-enabled", omitable = true);
  fpCommandTransparency->SetDefaultValue(true);
}

G4OpenGLViewerMessenger::~G4OpenGLViewerMessenger ()
{
  delete fpCommandTransparency;
  delete fpCommandStartTime;
  delete fpCommandPrintSize;
  delete fpCommandPrintMode;
  delete fpCommandPrintFilename;
  delete fpCommandPrintEPS;
  delete fpCommandFade;
  delete fpCommandEndTime;
  delete fpCommandDisplayListLimit;
  delete fpCommandDisplayLightFront;
  delete fpCommandDisplayHeadTime;
  delete fpDirectorySet;
  delete fpDirectory;

  delete fpInstance;
}

void G4OpenGLViewerMessenger::SetNewValue
(G4UIcommand* command, G4String newValue)
{
  G4VisManager* pVisManager = G4VisManager::GetInstance();

  G4VViewer* pViewer = pVisManager->GetCurrentViewer();

  if (!pViewer) {
    G4cout <<
      "G4OpenGLViewerMessenger::SetNewValue: No current viewer."
      "\n  \"/vis/open\", or similar, to get one."
           << G4endl;
    return;
  }

  G4OpenGLViewer* pOGLViewer = dynamic_cast<G4OpenGLViewer*>(pViewer);

  if (!pOGLViewer) {
    G4cout <<
      "G4OpenGLViewerMessenger::SetNewValue: Current viewer is not of type"
      "\n  OGL.  (It is \""
	   << pViewer->GetName() <<
      "\".)\n  Use \"/vis/viewer/select\" or \"/vis/open\"."
           << G4endl;
    return;
  }

  if (command == fpCommandPrintEPS) 
    {
      pOGLViewer->printEPS();
      if (pOGLViewer->fVP.IsAutoRefresh())
	G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/refresh");
      return;
    }

  if (command == fpCommandPrintSize) 
    {
      G4int width,height;
      std::istringstream iss(newValue);
      iss >> width
	  >> height;
      pOGLViewer->setPrintSize(width,height);
      return;
    }

  if (command == fpCommandPrintFilename) 
    {
      G4String name;
      G4bool inc;
      std::istringstream iss(newValue);
      iss >> name
	  >> inc;
      pOGLViewer->setPrintFilename(name,inc);
      return;
    }

  if (command == fpCommandPrintMode)
    {
      if (newValue == "vectored") pOGLViewer->fVectoredPs = true;
      if (newValue == "pixmap") pOGLViewer->fVectoredPs = false;
      return;
    }

  if (command == fpCommandTransparency)
    {
      pOGLViewer->transparency_enabled = command->ConvertToBool(newValue);
      if (pOGLViewer->fVP.IsAutoRefresh())
	G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/refresh");
      return;
    }

  G4OpenGLStoredViewer* pOGLSViewer =
    dynamic_cast<G4OpenGLStoredViewer*>(pViewer);

  if (!pOGLSViewer)
    {
      G4cout <<
  "G4OpenGLViewerMessenger::SetNewValue: Current viewer is not of type OGLS."
  "\n  (It is \"" << pViewer->GetName() << "\".)"
  "\n  This feature is only implemented for OGL Stored viewers."
  "\n  Use \"/vis/viewer/select\" or \"/vis/open OGLS...\"."
	     << G4endl;
      return;
    }

  if (command == fpCommandDisplayHeadTime)
    {
      G4String display;
      G4double screenX, screenY, screenSize, red, green, blue;
      std::istringstream iss(newValue);
      iss >> display >> screenX >> screenY
	  >> screenSize >> red >> green >> blue;
      pOGLSViewer->fDisplayHeadTime = command->ConvertToBool(display);
      pOGLSViewer->fDisplayHeadTimeX = screenX;
      pOGLSViewer->fDisplayHeadTimeY = screenY;
      pOGLSViewer->fDisplayHeadTimeSize = screenSize;
      pOGLSViewer->fDisplayHeadTimeRed = red;
      pOGLSViewer->fDisplayHeadTimeGreen = green;
      pOGLSViewer->fDisplayHeadTimeBlue = blue;
      return;
    }

  if (command == fpCommandDisplayLightFront)
    {
      G4String display, originX, originY, originZ, unitS, originT, unitT;
      G4double red, green, blue;
      std::istringstream iss(newValue);
      iss >> display
	  >> originX >> originY >> originZ >> unitS
	  >> originT >> unitT
	  >> red >> green >> blue;
      pOGLSViewer->fDisplayLightFront = command->ConvertToBool(display);
      pOGLSViewer->fDisplayLightFrontX =
	command->ConvertToDimensionedDouble(G4String(originX + ' ' + unitS));
      pOGLSViewer->fDisplayLightFrontY =
	command->ConvertToDimensionedDouble(G4String(originY + ' ' + unitS));
      pOGLSViewer->fDisplayLightFrontZ =
	command->ConvertToDimensionedDouble(G4String(originZ + ' ' + unitS));
      pOGLSViewer->fDisplayLightFrontT =
	command->ConvertToDimensionedDouble(G4String(originT + ' ' + unitT));
      pOGLSViewer->fDisplayLightFrontRed = red;
      pOGLSViewer->fDisplayLightFrontGreen = green;
      pOGLSViewer->fDisplayLightFrontBlue = blue;
      return;
    }

  if (command == fpCommandEndTime)
    {
      G4String end_time_string, end_time_unit,
	time_range_string, time_range_unit;
      std::istringstream iss(newValue);
      iss >> end_time_string >> end_time_unit
	  >> time_range_string >> time_range_unit;
      pOGLSViewer->fEndTime = command->ConvertToDimensionedDouble
	(G4String(end_time_string + ' ' + end_time_unit));
      G4double timeRange = command->ConvertToDimensionedDouble
	(G4String(time_range_string + ' ' + time_range_unit));
      if (timeRange > 0.) {
	pOGLSViewer->fStartTime = pOGLSViewer->fEndTime - timeRange;
      }
      if (pOGLSViewer->fVP.IsAutoRefresh())
	G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/refresh");
      return;
    }

  if (command == fpCommandFade)
    {
      pOGLSViewer->fFadeFactor = command->ConvertToDouble(newValue);
      if (pOGLSViewer->fVP.IsAutoRefresh())
	G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/refresh");
      return;
    }

  if (command == fpCommandStartTime)
    {
      G4String start_time_string, start_time_unit,
	time_range_string, time_range_unit;
      std::istringstream iss(newValue);
      iss >> start_time_string >> start_time_unit
	  >> time_range_string >> time_range_unit;
      pOGLSViewer->fStartTime = command->ConvertToDimensionedDouble
	(G4String(start_time_string + ' ' + start_time_unit));
      G4double timeRange = command->ConvertToDimensionedDouble
	(G4String(time_range_string + ' ' + time_range_unit));
      if (timeRange > 0.) {
	pOGLSViewer->fEndTime = pOGLSViewer->fStartTime + timeRange;
      }
      if (pOGLSViewer->fVP.IsAutoRefresh())
	G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/refresh");
      return;
    }

  G4VSceneHandler* pSceneHandler = pViewer->GetSceneHandler();

  if (!pSceneHandler) {
    G4cout <<
  "G4OpenGLViewerMessenger::SetNewValue: This viewer has no scene handler."
  "\n  Shouldn't happen - please report circumstances."
  "\n  (Viewer is \"" << pViewer->GetName() << "\".)"
  "\n  Try \"/vis/open\", or similar, to get one."
           << G4endl;
    return;
  }

  G4OpenGLSceneHandler* pOGLSceneHandler =
    dynamic_cast<G4OpenGLSceneHandler*>(pSceneHandler);

  if (!pOGLSceneHandler) {
    G4cout <<
  "G4OpenGLViewerMessenger::SetNewValue: Current scene handler is not of type"
  "\n  OGL.  (Viewer is \"" << pViewer->GetName() << "\".)"
  "\n  (Scene handler is \"" << pSceneHandler->GetName() << "\".)"
  "\n  Use \"/vis/sceneHandler/list\" and \"/vis/sceneHandler/select\""
  "\n  or \"/vis/open\"."
           << G4endl;
    return;
  }

  G4OpenGLStoredSceneHandler* pOGLSSceneHandler =
    dynamic_cast<G4OpenGLStoredSceneHandler*>(pViewer->GetSceneHandler());

  if (!pOGLSSceneHandler) {
    G4cout <<
  "G4OpenGLViewerMessenger::SetNewValue: Current scene handler is not of type"
  "\n  OGLS (Stored).  (Viewer is \"" << pViewer->GetName() << "\".)"
  "\n  (Scene handler is \"" << pSceneHandler->GetName() << "\".)"
  "\n  This feature is only implemented for OGL Stored"
  "\n  scene handlers.  Use \"/vis/viewer/select\" or \"/vis/open OGLS...\"."
           << G4endl;
    return;
  }

  if (command == fpCommandDisplayListLimit)
    {
      G4int displayListLimit =
	fpCommandDisplayListLimit->GetNewIntValue(newValue);
      pOGLSSceneHandler->SetDisplayListLimit(displayListLimit);
      return;
    }
}

#endif
