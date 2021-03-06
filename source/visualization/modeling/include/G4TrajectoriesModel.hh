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
// $Id: G4TrajectoriesModel.hh,v 1.11 2010-05-11 11:21:52 allison Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// John Allison  26th August 1998.
//
// Class Description:
//
// Model which knows how to draw GEANT4 trajectories.
//
// For access to base class information, e.g., modeling parameters,
// use GetModelingParameters() inherited from G4VModel.  See Class
// Description of the base class G4VModel.

#ifndef G4TRAJECTORIESMODEL_HH
#define G4TRAJECTORIESMODEL_HH

#include "G4VModel.hh"

class G4VTrajectory;

class G4TrajectoriesModel: public G4VModel {

public: // With description

  G4TrajectoriesModel ();
  G4TrajectoriesModel (G4int drawingMode);

  virtual ~G4TrajectoriesModel ();

  virtual void DescribeYourselfTo (G4VGraphicsScene&);
  // The main task of a model is to describe itself to the graphics scene.

  G4bool IsDrawingModeSet() const;
  G4int GetDrawingMode() const;
  void SetDrawingMode(G4int drawingMode);

  const G4VTrajectory* GetCurrentTrajectory() const
  {return fpCurrentTrajectory;}

private:

  G4bool fDrawingModeSet;
  G4int fDrawingMode;
  const G4VTrajectory* fpCurrentTrajectory;

};

#endif
