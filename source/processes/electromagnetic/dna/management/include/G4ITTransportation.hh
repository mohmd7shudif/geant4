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
// This class is a slightly modified version of G4Transportation
// But it should use the exact same algorithm
//
// WARNING : This class is released as a prototype.
// It might strongly evolve or even disapear in the next releases.
//
// History:
// -----------
// 10 Oct 2011 M.Karamitros created
//
// -------------------------------------------------------------------

#ifndef G4ITTransportation_H
#define G4ITTransportation_H

#include "G4IT.hh"
#include "G4VITProcess.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleChangeForTransport.hh"

class G4Navigator;
class G4SafetyHelper;
class G4PropagatorInField;

class G4ITTransportation : public G4VITProcess
{
    // Concrete class that does the geometrical transport
public:  // with description

    G4ITTransportation(const G4String& aName =  "ITTransportation",G4int verbosityLevel= 1);
    virtual ~G4ITTransportation();

    G4ITTransportation(const G4ITTransportation&);

    G4IT_ADD_CLONE(G4VITProcess, G4ITTransportation)

    virtual void BuildPhysicsTable(const G4ParticleDefinition&){;}

    virtual void ComputeStep(const G4Track&,
                             const G4Step&,
                             const double timeStep,
                             double& spaceStep) ;

    virtual void StartTracking(G4Track* aTrack);
    // Give to the track a pointer to the transportation state

//________________________________________________________
public:  // without description

    virtual G4double AtRestGetPhysicalInteractionLength(
        const G4Track& ,
        G4ForceCondition*
    )
    {
        return -1.0;
    }
    // No operation in  AtRestDoIt.

    virtual G4VParticleChange* AtRestDoIt(
        const G4Track& ,
        const G4Step&
    )
    {
        return 0;
    }
    // No operation in  AtRestDoIt.

    virtual G4double AlongStepGetPhysicalInteractionLength(
        const G4Track& track,
        G4double , //   previousStepSize
        G4double currentMinimumStep,
        G4double& currentSafety,
        G4GPILSelection* selection);

    virtual G4double PostStepGetPhysicalInteractionLength(
        const G4Track&  , // track
        G4double , // previousStepSize
        G4ForceCondition* pForceCond);

    virtual G4VParticleChange* AlongStepDoIt( const G4Track& track,
        const G4Step&  stepData );

    virtual G4VParticleChange* PostStepDoIt( const G4Track& track,
        const G4Step& ) ;

//________________________________________________________
//    inline virtual G4double GetTransportationTime() ;

    G4PropagatorInField* GetPropagatorInField();
    void SetPropagatorInField( G4PropagatorInField* pFieldPropagator);
    // Access/set the assistant class that Propagate in a Field.

    inline void   SetVerboseLevel( G4int verboseLevel );
    inline G4int  GetVerboseLevel() const;
    // Level of warnings regarding eg energy conservation
    // in field integration.

    inline G4double GetThresholdWarningEnergy() const;
    inline G4double GetThresholdImportantEnergy() const;
    inline G4int GetThresholdTrials() const;

    inline void SetThresholdWarningEnergy( G4double newEnWarn );
    inline void SetThresholdImportantEnergy( G4double newEnImp );
    inline void SetThresholdTrials(G4int newMaxTrials );

    // Get/Set parameters for killing loopers:
    //   Above 'important' energy a 'looping' particle in field will
    //   *NOT* be abandoned, except after fThresholdTrials attempts.
    // Below Warning energy, no verbosity for looping particles is issued

    inline G4double GetMaxEnergyKilled() const;
    inline G4double GetSumEnergyKilled() const;
    inline void ResetKilledStatistics( G4int report = 1);
    // Statistics for tracks killed (currently due to looping in field)

    inline void EnableShortStepOptimisation(G4bool optimise=true);
    // Whether short steps < safety will avoid to call Navigator (if field=0)

protected :
    //________________________________________________________________
    // Protected methods
    G4bool               DoesGlobalFieldExist();
    // Checks whether a field exists for the "global" field manager.

    //________________________________________________________________
    // Process information
    struct G4ITTransportationState : public G4ProcessState
    {
    public :
        G4ITTransportationState();
        virtual ~G4ITTransportationState();

        G4ThreeVector        fTransportEndPosition;
        G4ThreeVector        fTransportEndMomentumDir;
        G4double             fTransportEndKineticEnergy;
        G4ThreeVector        fTransportEndSpin;
        G4bool               fMomentumChanged;
        G4bool               fEnergyChanged;
        G4bool               fEndGlobalTimeComputed;
        G4double             fCandidateEndGlobalTime;
        G4bool               fParticleIsLooping;
        // The particle's state after this Step, Store for DoIt

        G4TouchableHandle    fCurrentTouchableHandle;
        G4bool fGeometryLimitedStep;
        // Flag to determine whether a boundary was reached.

        G4ThreeVector  fPreviousSftOrigin;
        G4double       fPreviousSafety;
        // Remember last safety origin & value.

        // Counter for steps in which particle reports 'looping',
        //   if it is above 'Important' Energy
        G4int    fNoLooperTrials;

        // G4bool         fFieldExists;
        // Whether a magnetic field exists ...
        // A data member for this is problematic: it is useful only if it
        // can be initialised and updated -- and a scheme is not yet possible.

        G4double endpointDistance;
    };

    G4ITTransportationState* const & fTransportationState;

    //________________________________________________________________
    // Informations relative to the process only (meaning no information
    // relative to the treated particle)
    G4Navigator*         fLinearNavigator;
    G4PropagatorInField* fFieldPropagator;
    // The Propagators used to transport the particle

    static const G4TouchableHandle nullTouchableHandle;  // Points to (G4VTouchable*) 0

    G4ParticleChangeForTransport fParticleChange;
    // New ParticleChange

    // Thresholds for looping particles:
    //
    G4double fThreshold_Warning_Energy;     //  Warn above this energy
    G4double fThreshold_Important_Energy;   //  Hesitate above this
    G4int    fThresholdTrials;              //    for this no of trials
    // Above 'important' energy a 'looping' particle in field will
    //   *NOT* be abandoned, except after fThresholdTrials attempts.
    G4double fUnimportant_Energy;
    //  Below this energy, no verbosity for looping particles is issued


    // Statistics for tracks abandoned
    G4double fSumEnergyKilled;
    G4double fMaxEnergyKilled;

    // Whether to avoid calling G4Navigator for short step ( < safety)
    //   If using it, the safety estimate for endpoint will likely be smaller.
    G4bool   fShortStepOptimisation;

    G4SafetyHelper* fpSafetyHelper;  // To pass it the safety value obtained

    // Verbosity
    G4int    fVerboseLevel;
    // Verbosity level for warnings
    // eg about energy non-conservation in magnetic field.

    void SetInstantiateProcessState(G4bool flag)
    { fInstantiateProcessState = flag; }

    G4bool InstantiateProcessState() { return fInstantiateProcessState; }

private :
    G4bool fInstantiateProcessState;
    G4ITTransportation& operator=(const G4ITTransportation&);
};

#include "G4ITTransportation.icc"
#endif // G4ITTransportation_H
