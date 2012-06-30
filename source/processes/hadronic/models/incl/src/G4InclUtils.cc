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
#include "G4InclUtils.hh"
#include "G4NucleiProperties.hh"

G4double G4InclUtils::calculate4MomentumScaling(G4int A, G4int Z, G4double excitationE, G4double kineticE,
						G4double px, G4double py, G4double pz)
{
  G4double nuclearMass = (G4NucleiProperties::GetNuclearMass(A, Z) / MeV + excitationE) * MeV;
  G4double p2 = px*px + py*py + pz*pz;
  if(p2 > 0.0)
    return std::sqrt(kineticE*kineticE + 2.0 * kineticE * nuclearMass)/std::sqrt(p2);
  else // if p2 <= 0.0 we have incorrect input. Returning 1.0 allows us to attempt to recover from the error.
    return 1.0;
}
