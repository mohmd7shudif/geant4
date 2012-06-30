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
// $Id: G4LEAlphaInelastic.hh,v 1.10 2007-02-24 06:06:38 dennis Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// Hadronic Process: Low Energy Alpha Inelastic Process
// J.L. Chuma, TRIUMF, 21-Feb-1997
 
#ifndef G4LEAlphaInelastic_h
#define G4LEAlphaInelastic_h 1
 
// Class Description
// Final state production model for Alpha inelastic scattering below 20 GeV; 
// To be used in your physics list in case you need this physics.
// In this case you want to register an object of this class with 
// the corresponding process.
// Class Description - End

#include "G4InelasticInteraction.hh"
 
class G4LEAlphaInelastic : public G4InelasticInteraction
{
  public:
    
    G4LEAlphaInelastic(const G4String& name = "G4LEAlphaInelastic");
    
    ~G4LEAlphaInelastic() { }
    
    G4HadFinalState* ApplyYourself(const G4HadProjectile& aTrack,
                                   G4Nucleus& targetNucleus);

    virtual void ModelDescription(std::ostream& outFile) const;
};

#endif
