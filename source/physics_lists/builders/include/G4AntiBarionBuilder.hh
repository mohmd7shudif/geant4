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
// $Id: G4AntiBarionBuilder.hh,v 1.1 2006-10-31 11:35:01 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4AntiBarionBuilder
//
// Author: 2011 J. Apostolakis
//
// Modified:
//----------------------------------------------------------------------------
//
#ifndef G4AntiBarionBuilder_h
#define G4AntiBarionBuilder_h 1

#include "globals.hh"

#include "G4ProtonInelasticProcess.hh"
#include "G4VAntiBarionBuilder.hh"

#include <vector>

class G4AntiBarionBuilder
{
  public: 
    G4AntiBarionBuilder();
    virtual ~G4AntiBarionBuilder();

  public: 
    void Build();
    void RegisterMe(G4VAntiBarionBuilder * aB) {theModelCollections.push_back(aB);}

  private:
    G4AntiProtonInelasticProcess*   theAntiProtonInelastic;
    G4AntiNeutronInelasticProcess*  theAntiNeutronInelastic;
    G4AntiDeuteronInelasticProcess* theAntiDeuteronInelastic;
    G4AntiTritonInelasticProcess*   theAntiTritonInelastic;
    G4AntiHe3InelasticProcess*      theAntiHe3Inelastic;
    G4AntiAlphaInelasticProcess*    theAntiAlphaInelastic;
     
    std::vector<G4VAntiBarionBuilder *> theModelCollections;

    G4bool wasActivated;
};

#endif

