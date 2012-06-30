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
// $Id: G4OpenGLStoredSceneHandler.hh,v 1.32 2010-11-10 17:10:49 allison Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// Laurent Garnier  27th October 2011

#ifdef G4VIS_BUILD_OPENGLQT_DRIVER

#ifndef G4OPENGLSTOREDQTSCENEHANDLER_HH
#define G4OPENGLSTOREDQTSCENEHANDLER_HH

#include "G4OpenGLStoredSceneHandler.hh"

class G4OpenGLStoredQtSceneHandler: public G4OpenGLStoredSceneHandler {

public:

  G4OpenGLStoredQtSceneHandler (G4VGraphicsSystem& system, const G4String& name = "");
  virtual ~G4OpenGLStoredQtSceneHandler ();

  // Two virtual functions for extra processing in a sub-class, for
  // example, to make a display tree.
  void ExtraPOProcessing(size_t currentPOListIndex);
  void ExtraTOProcessing(size_t currentTOListIndex);

  void ClearStore ();
  void ClearTransientStore ();
};

#endif

#endif
