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

#ifndef BrachyUserScoreWriter_h
#define BrachyUserScoreWriter_h 1

#include "globals.hh"
#include "G4VScoreWriter.hh"
/*
// Code developed by:
// S.Guatelli, susanna@uow.edu.au
//
Original code from geant4/examples/extended/runAndEvent/RE03, by M. Asai
*/

// class description:
//
//  This class represents storing the scored quantity into a file.
//
class BrachyUserScoreWriter : public G4VScoreWriter {

public:
  BrachyUserScoreWriter();
  virtual ~BrachyUserScoreWriter();

public:
  // store a quantity into a file
  void DumpQuantityToFile(G4String & psName, G4String & fileName, G4String & option);
};

#endif


