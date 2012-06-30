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
// $Id: G4tgbMaterialMixture.cc,v 1.1 2008-10-23 14:43:43 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
// class G4tgbMaterialMixture

// History:
// - Created.                                 P.Arce, CIEMAT (November 2007)
// -------------------------------------------------------------------------

#include "G4tgbMaterialMixture.hh"

// -------------------------------------------------------------------------
G4tgbMaterialMixture::G4tgbMaterialMixture()
{
}


// -------------------------------------------------------------------------
G4tgbMaterialMixture::~G4tgbMaterialMixture()
{
}


// -------------------------------------------------------------------------
const G4String& G4tgbMaterialMixture::GetComponent(G4int i) const
{
   return theTgrMate->GetComponent( i );
}


// -------------------------------------------------------------------------
G4double G4tgbMaterialMixture::GetFraction(G4int i) const
{
   return theTgrMate->GetFraction( i );
} 


// -------------------------------------------------------------------------
G4tgbMaterialMixture&
G4tgbMaterialMixture::operator=(const G4tgbMaterialMixture&)
{
   return *this;
}


// -------------------------------------------------------------------------
void G4tgbMaterialMixture::TransformToFractionsByWeight()
{
}
