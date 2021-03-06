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
// $Id: G4Na23GEMChannel.hh,v 1.4 2009-09-15 12:54:16 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// Hadronic Process: Nuclear De-excitations
// by V. Lara (Sept. 2001)
//


#ifndef G4Na23GEMChannel_h
#define G4Na23GEMChannel_h 1

#include "G4GEMChannel.hh"
#include "G4Na23GEMCoulombBarrier.hh"
#include "G4Na23GEMProbability.hh"

class G4Na23GEMChannel : public G4GEMChannel
{
public:
  // only available constructor
  G4Na23GEMChannel() : G4GEMChannel(23,11,"Na23",
				   &theEvaporationProbability,
				   &theCoulombBarrier)
  {
    theEvaporationProbability.SetCoulomBarrier(&theCoulombBarrier);
  }
  
  // destructor
  ~G4Na23GEMChannel() {};
  
private:
  const G4Na23GEMChannel & operator=(const G4Na23GEMChannel & right);  
    
  G4Na23GEMChannel(const G4Na23GEMChannel & right);
  
public:
  G4bool operator==(const G4Na23GEMChannel & right) const;
  G4bool operator!=(const G4Na23GEMChannel & right) const;
    
private:
  
  G4Na23GEMCoulombBarrier theCoulombBarrier;
	
  G4Na23GEMProbability theEvaporationProbability;
  
};
#endif
