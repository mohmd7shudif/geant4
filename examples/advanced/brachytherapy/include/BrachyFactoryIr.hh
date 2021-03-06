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
// $Id: BrachyFactoryIr.hh,v 1.6 2006-06-29 15:47:36 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//    **********************************
//    *                                *
//    *      BrachyFactoryIr.hh        *
//    *                                *
//    **********************************
//
//
//
#ifndef BrachyFactoryIr_h
#define BrachyFactoryIr_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "BrachyFactory.hh"
#include "G4RunManager.hh"

class G4ParticleGun;
class G4Run;
class G4Event;
class BrachyAnalysisManager;
class BrachyFactory;
class BrachyPrimaryGeneratorActionIr;
class BrachyDetectorConstructionIr;

// This class manages the creation of iridum source used in endocavitary
// brachytherapy ...
class BrachyFactoryIr : public BrachyFactory
{
public:
  BrachyFactoryIr();
  ~BrachyFactoryIr();

  void CreatePrimaryGeneratorAction(G4Event*);
  void CreateSource(G4VPhysicalVolume*);
  void CleanSource();

private:
  BrachyDetectorConstructionIr* iridiumSource;
  BrachyPrimaryGeneratorActionIr*  iridiumPrimaryParticle;
};
#endif
