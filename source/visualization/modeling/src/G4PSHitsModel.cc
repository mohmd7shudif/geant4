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
// $Id: G4PSHitsModel.cc,v 1.4 2009-11-04 12:44:39 allison Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
// Created:  Mar. 31, 2009  Akinori Kimura
// Model which draws the primtive scorer hits.
//

#include "G4PSHitsModel.hh"

#include "G4ModelingParameters.hh"
#include "G4VGraphicsScene.hh"
#include "G4Event.hh"
#include "G4ScoringManager.hh"

G4PSHitsModel::~G4PSHitsModel () {}

G4PSHitsModel::G4PSHitsModel (const G4String& requestedMapName):
  fRequestedMapName(requestedMapName), fpCurrentHits(0)
{
  fGlobalTag = "G4PSHitsModel for G4THitsMap<G4double> hits.";
  fGlobalDescription = fGlobalTag;
}

void G4PSHitsModel::DescribeYourselfTo (G4VGraphicsScene& sceneHandler)
{
  G4ScoringManager* scoringManager =
    G4ScoringManager::GetScoringManagerIfExist();
  if (scoringManager) {
    size_t nMeshes = scoringManager->GetNumberOfMesh();
    for (size_t i = 0; i < nMeshes; ++i) {
      G4VScoringMesh* mesh = scoringManager->GetMesh(i);
      if (mesh && mesh->IsActive()) {
	MeshScoreMap scoreMap = mesh->GetScoreMap();
	for(MeshScoreMap::const_iterator i = scoreMap.begin();
	    i != scoreMap.end(); ++i) {
	  const G4String& name = i->first;
	  if (fRequestedMapName == "all" || name == fRequestedMapName) {
	    fpCurrentHits = i->second;
	    //G4cout << name << ": " << fpCurrentHits << G4endl;
	    if (fpCurrentHits) sceneHandler.AddCompound(*fpCurrentHits);
	  }
	}
      }
    }
  }
}
