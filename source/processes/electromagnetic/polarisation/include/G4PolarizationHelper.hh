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
// $Id: G4PolarizationHelper.hh,v 1.2 2007-11-01 17:30:25 schaelic Exp $
// tag $Name: not supported by cvs2svn $
//
// GEANT4 Class header file
//
//
// File name:     G4PolarizationHelper
//
// Author:        Andreas Schaelicke
//
// Creation date: 12.08.2006
//
// Class Description:
//
//   Provides some basic polarization transformation routines.

#ifndef G4PolarizationHelper_h
#define G4PolarizationHelper_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "Randomize.hh"


class G4PolarizationHelper {
public:
  static G4ThreeVector GetFrame(const G4ThreeVector &, const G4ThreeVector &);
  static G4ThreeVector GetParticleFrameX(const G4ThreeVector &);
  static G4ThreeVector GetParticleFrameY(const G4ThreeVector &);
  static G4ThreeVector GetRandomFrame(const G4ThreeVector &);

  static G4ThreeVector GetSpinInPRF(const G4ThreeVector &uZ,const G4ThreeVector &spin);

  static void TestPolarizationTransformations();
  static void TestInteractionFrame();
};

#endif
