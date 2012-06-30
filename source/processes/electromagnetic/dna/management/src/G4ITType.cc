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
// Author: Mathieu Karamitros (kara (AT) cenbg . in2p3 . fr) 
//
// History:
// -----------
// 10 Oct 2011 M.Karamitros created
//
// -------------------------------------------------------------------

#include "G4ITType.hh"

G4ITTypeManager* G4ITTypeManager::fInstance = 0;

// static method
size_t G4ITType::size()
{
    return  G4ITTypeManager::Instance()->size();
}

G4ITType& G4ITType::operator=(const G4ITType & rhs)
{
    if (this == &rhs) return *this;
    fValue = rhs.fValue;
    return *this;
}

G4ITTypeManager*  G4ITTypeManager::Instance()
{
    if(fInstance == 0)
    {
        fInstance =  new G4ITTypeManager();
    }
    return fInstance;
}

void G4ITTypeManager::DeleteInstance()
{
    delete fInstance ;
}

G4ITTypeManager::G4ITTypeManager()
{ fLastType = 0;}

G4ITTypeManager::~G4ITTypeManager()
{;}

size_t G4ITTypeManager::size() const
{
    return fLastType;
}

G4ITType G4ITTypeManager::NewType()
{
    G4ITType newType = fLastType;
    fLastType++;
    return newType;
}
