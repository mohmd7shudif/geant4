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
// $Id: G4KleinNishinaModel.hh,v 1.1 2010-09-03 14:11:58 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// -------------------------------------------------------------------
//
// GEANT4 Class header file
//
//
// File name:     G4KleinNishinaModel
//
// Author:        Vladimir Ivanchenko on base of G4KleinNishinaCompton
//
// Creation date: 13.06.2010
//
// Modifications:
//
// Class Description:
//
// Implementation of gamma Compton scattering with atomic effects 
// 

// -------------------------------------------------------------------
//

#ifndef G4KleinNishinaModel_h
#define G4KleinNishinaModel_h 1

#include "G4VEmModel.hh"
#include "G4LorentzVector.hh"
#include <vector>

class G4ParticleChangeForGamma;
class G4VAtomDeexcitation;

class G4KleinNishinaModel : public G4VEmModel
{

public:

  G4KleinNishinaModel(const G4String& nam = "KleinNishina");

  virtual ~G4KleinNishinaModel();

  virtual void Initialise(const G4ParticleDefinition*, const G4DataVector&);

  virtual G4double ComputeCrossSectionPerAtom(
                                const G4ParticleDefinition*,
                                      G4double kinEnergy, 
                                      G4double Z, 
                                      G4double A, 
                                      G4double cut,
                                      G4double emax);

  virtual void SampleSecondaries(std::vector<G4DynamicParticle*>*,
				 const G4MaterialCutsCouple*,
				 const G4DynamicParticle*,
				 G4double tmin,
				 G4double maxEnergy);

protected:

  G4ParticleDefinition*     theGamma;
  G4ParticleDefinition*     theElectron;
  G4ParticleChangeForGamma* fParticleChange;
  G4double                  lowestGammaEnergy;

private:

  // hide assignment operator
  G4KleinNishinaModel & operator=(const  G4KleinNishinaModel &right);
  G4KleinNishinaModel(const  G4KleinNishinaModel&);
 
  G4LorentzVector lv1, lv2;
  G4ThreeVector bst;

  G4VAtomDeexcitation*      fAtomDeexcitation;
  G4double                  limitFactor;
  G4bool                    isInitialized;
  std::vector<G4double>     fProbabilities;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif
