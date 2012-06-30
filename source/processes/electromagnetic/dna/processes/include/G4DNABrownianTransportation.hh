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

#ifndef G4ITBROWNIANTRANSPORTATION_H
#define G4ITBROWNIANTRANSPORTATION_H

#include "G4ITTransportation.hh"

class G4SafetyHelper;

class G4DNABrownianTransportation : public G4ITTransportation
{
public:
    G4DNABrownianTransportation(const G4String& aName =  "DNABrownianTransportation", G4int verbosityLevel= 1);
    G4IT_ADD_CLONE(G4VITProcess,G4DNABrownianTransportation)
    virtual ~G4DNABrownianTransportation();
    G4DNABrownianTransportation(const G4DNABrownianTransportation& other);
    G4DNABrownianTransportation& operator=(const G4DNABrownianTransportation& other);

    void BuildPhysicsTable(const G4ParticleDefinition&);

    virtual void ComputeStep(const G4Track&,
                             const G4Step&,
                             const double,
                             double&) ;

    virtual G4double AlongStepGetPhysicalInteractionLength( const G4Track& /*track*/,
                                                            G4double /*previousStepSize*/,
                                                            G4double /*currentMinimumStep*/,
                                                            G4double& /*currentSafety*/,
                                                            G4GPILSelection* /*selection*/);
    virtual G4VParticleChange* PostStepDoIt( const G4Track& track, const G4Step& ) ;

    G4VParticleChange* AlongStepDoIt(const G4Track& track, const G4Step&);

protected:
    void Diffusion(const G4Track& track);
    G4SafetyHelper* fpSafetyHelper;
    G4Material* fNistWater ;
};

#endif // G4ITBROWNIANTRANSPORTATION_H
