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
// $Id: G4VBremAngularDistribution.cc,v 1.1 2010-10-14 16:34:04 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
//
// File name:     G4VBremAngularDistribution
//
// Author:        Andreia Trindade (andreia@lip.pt)
//                Pedro Rodrigues  (psilva@lip.pt)
//                Luis Peralta     (luis@lip.pt)
//                Maria Grazia Pia (MariaGrazia.Pia@ge.infn.it)
// 
// Creation date: 21 March 2003
//
// Modifications: 
// 13 Oct 2010  V.Ivanchenko  Moved to utils  
//
// Class Description: 
//
// Abstract base class for Bremsstrahlung Angular Distribution Generation
//
// Class Description: End 

// -------------------------------------------------------------------
//
//    

#include "G4VBremAngularDistribution.hh"

//    

G4VBremAngularDistribution::G4VBremAngularDistribution(const G4String& name) 
  : G4VEmAngularDistribution(name)
{}

//    

G4VBremAngularDistribution::~G4VBremAngularDistribution() 
{}

void G4VBremAngularDistribution::PrintGeneratorInformation() const
{}

//    
