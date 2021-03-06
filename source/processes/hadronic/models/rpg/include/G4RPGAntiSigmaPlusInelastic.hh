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
// $Id: G4RPGAntiSigmaPlusInelastic.hh,v 1.1 2007-07-18 20:51:37 dennis Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// Author: D. H. Wright
// Date:   18 June 2007
//

// Class Description:
// Final state production model for anti-sigma+ inelastic scattering
// using the re-parameterized Gheisha model.

#ifndef G4RPGAntiSigmaPlusInelastic_h
#define G4RPGAntiSigmaPlusInelastic_h 1
 

#include "G4RPGInelastic.hh"
 
 class G4RPGAntiSigmaPlusInelastic : public G4RPGInelastic
 {
 public:
    
    G4RPGAntiSigmaPlusInelastic() : G4RPGInelastic("G4RPGAntiSigmaPlusInelastic")
    {
      SetMinEnergy( 0.0 );
      SetMaxEnergy( 25.*GeV );
    }
    
    ~G4RPGAntiSigmaPlusInelastic() { }
    
    G4HadFinalState* ApplyYourself(const G4HadProjectile &aTrack,
                                   G4Nucleus &targetNucleus );
    
 private:
    
    void Cascade(                         // derived from CASASP
      G4FastVector<G4ReactionProduct,GHADLISTSIZE> &vec,
      G4int& vecLen,
      const G4HadProjectile *originalIncident,
      G4ReactionProduct &currentParticle,
      G4ReactionProduct &targetParticle,
      G4bool &incidentHasChanged,
      G4bool &targetHasChanged,
      G4bool &quasiElastic );
    
 };
 
#endif
 
