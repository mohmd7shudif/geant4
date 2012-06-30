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
// WARNING : This class is released as a prototype.
// It might strongly evolve or even disapear in the next releases.
//
// History:
// -----------
// 10 Oct 2011 M.Karamitros created
//
// -------------------------------------------------------------------

#ifndef G4DNATransformElectronModel_h
#define G4DNATransformElectronModel_h 1

#include "G4VEmModel.hh"


/**
  * When an electron reaches the highest energy domain of G4DNATransformElectronModel,
  * it is then automatically converted into a solvated electron without thermalization
  * displacement (assumed to be already thermalized).
  */

class G4DNATransformElectronModel: public G4VEmModel
{
public :
    G4DNATransformElectronModel(const G4ParticleDefinition* p = 0,
                                const G4String& nam = "DNATransformElectronModel");
    virtual ~G4DNATransformElectronModel();

    virtual void Initialise(const G4ParticleDefinition*, const G4DataVector&);

    virtual G4double CrossSectionPerVolume(  const G4Material* material,
            const G4ParticleDefinition* p,
            G4double ekin,
            G4double emin,
            G4double emax);

    virtual void SampleSecondaries(std::vector<G4DynamicParticle*>*,
                                   const G4MaterialCutsCouple*,
                                   const G4DynamicParticle*,
                                   G4double tmin,
                                   G4double maxEnergy);

    inline void SetVerbose(int);

protected:

    G4ParticleChangeForGamma* fParticleChangeForGamma;

private:

    G4Material* fNistWater;
    G4bool fIsInitialised;
    G4int fVerboseLevel;

    G4DNATransformElectronModel & operator=(const  G4DNATransformElectronModel &right);
    G4DNATransformElectronModel(const  G4DNATransformElectronModel&);

};

inline void G4DNATransformElectronModel::SetVerbose(int flag)
{
    fVerboseLevel = flag ;
}

#endif
