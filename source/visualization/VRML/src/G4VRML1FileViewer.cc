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
// $Id: G4VRML1FileViewer.cc,v 1.12 2010-11-11 00:14:50 akimura Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// G4VRMLView.cc
// Satoshi Tanaka & Yasuhide Sawada


//#define DEBUG_FR_VIEW

#include "G4VisManager.hh"
#include "G4Scene.hh"
#include "G4VRML1FileViewer.hh"
#include "G4VRML1FileSceneHandler.hh"
#include "G4VRML1File.hh"
#include "G4ios.hh"

G4VRML1FileViewer::G4VRML1FileViewer(G4VRML1FileSceneHandler& sceneHandler,
				 const G4String& name) :
  G4VViewer(sceneHandler,
	    sceneHandler.IncrementViewCount(),
	    name),
  fSceneHandler(sceneHandler)
{}

G4VRML1FileViewer::~G4VRML1FileViewer()
{}

void G4VRML1FileViewer::SetView()
{
#if defined DEBUG_FR_VIEW
  if (G4VisManager::GetVerbosity() >= G4VisManager::errors)
        G4cout << "***** G4VRML1FileViewer::SetView(): No effects" << G4endl;
#endif
}

void G4VRML1FileViewer::DrawView()
{
#if defined DEBUG_FR_VIEW
  if (G4VisManager::GetVerbosity() >= G4VisManager::errors)
	G4cout << "***** G4VRML1FileViewer::DrawView()" << G4endl;
#endif

	fSceneHandler.VRMLBeginModeling();

	// Here is a minimal DrawView() function.
	NeedKernelVisit();
	ProcessView();
	FinishView();
}

void G4VRML1FileViewer::ClearView(void)
{
  //#if defined DEBUG_FR_VIEW
  if (G4VisManager::GetVerbosity() >= G4VisManager::errors)
        G4cout << "***** G4VRML1File1View::ClearView()" << G4endl;
  //#endif
  if(fSceneHandler.fFlagDestOpen) {
    fSceneHandler.fDest.close();
    // Re-open with same filename...
    fSceneHandler.fDest.open(fSceneHandler.fVRMLFileName);
    fSceneHandler.fDest << "#VRML V1.0 ascii" << "\n";
    fSceneHandler.fDest << "# Generated by VRML 1.0 driver of GEANT4\n" << "\n";
  }
}

void G4VRML1FileViewer::ShowView(void)
{
#if defined DEBUG_FR_VIEW
  if (G4VisManager::GetVerbosity() >= G4VisManager::errors)
        G4cout << "***** G4VRML1FileViewer::ShowView()" << G4endl;
#endif
	fSceneHandler.VRMLEndModeling();
}

void G4VRML1FileViewer::FinishView(void)
{
#if defined DEBUG_FR_VIEW
  if (G4VisManager::GetVerbosity() >= G4VisManager::errors)
        G4cout << "***** G4VRML1FileViewer::FinishView(): No effects" << G4endl;
#endif
}
