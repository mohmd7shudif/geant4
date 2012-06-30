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
// Author: Mathieu Karamitros (kara@cenbg.in2p3.fr)
//
// WARNING : This class is released as a prototype.
// It might strongly evolve or even disapear in the next releases.
//
// History:
// -----------
// 10 Oct 2011 M.Karamitros created
//
// -------------------------------------------------------------------

#include "G4DNAChemistryManager.hh"
#include "G4Molecule.hh"
#include "G4ITStepManager.hh"
#include "G4H2O.hh"
#include "G4DNAMolecularReactionTable.hh"
#include "G4WaterExcitationStructure.hh"
#include "G4WaterIonisationStructure.hh"
#include "G4Electron_aq.hh"
#include "G4ITManager.hh"
#include "G4MolecularConfiguration.hh"

using namespace std;

auto_ptr<G4DNAChemistryManager> G4DNAChemistryManager::fInstance(0);

G4DNAChemistryManager::G4DNAChemistryManager() :
    fActiveChemistry(false)
{
    fExcitationLevel = 0;
    fIonisationLevel = 0;
    fWriteFile       = false;
}

G4DNAChemistryManager* G4DNAChemistryManager::Instance()
{
    if(!fInstance.get()) fInstance = auto_ptr<G4DNAChemistryManager>(new G4DNAChemistryManager());
    return fInstance.get();
}

G4DNAChemistryManager::~G4DNAChemistryManager()
{
    if (fOutput.is_open())
    {
        fOutput.close();
    }
    if(fIonisationLevel) delete fIonisationLevel;
    if(fExcitationLevel) delete fExcitationLevel;
    G4DNAMolecularReactionTable::DeleteInstance();
    G4MoleculeHandleManager::DeleteInstance();
    G4MolecularConfiguration::DeleteManager();
    fInstance.release();
}

void G4DNAChemistryManager::DeleteInstance()
{
    if(fInstance.get())
        fInstance.reset();
}

void G4DNAChemistryManager::WriteInto(const G4String& output,
                                      ios_base::openmode mode)
{
    fOutput.open(output.data(), mode);
    fOutput << std::setprecision(6) << std::scientific;

    fOutput << setw(11) << left << "#Parent ID"
            << setw(10) << "Molecule"
            << setw(14) << "Elec Modif"
            << setw(13) << "Energy (eV)"
            << setw(22) << "X pos of parent [nm]"
            << setw(22) << "Y pos of parent [nm]"
            << setw(22) << "Z pos of parent [nm]"
            << setw(14) << "X pos [nm]"
            << setw(14) << "Y pos [nm]"
            << setw(14) << "Z pos [nm]"
            << G4endl
            << setw(21) << "#"
            << setw(13) << "1)io/ex=0/1"
            << G4endl
            << setw(21) << "#"
            << setw(13) << "2)level=0...5"
            << G4endl;
    fWriteFile = true;
}

void G4DNAChemistryManager::CloseFile()
{
    if (fOutput.is_open())
    {
        fOutput.close();
    }
    fWriteFile = false;
}

G4WaterExcitationStructure* G4DNAChemistryManager::GetExcitationLevel()
{
    if(!fExcitationLevel)
    {
        fExcitationLevel = new G4WaterExcitationStructure;
    }
    return fExcitationLevel;
}

G4WaterIonisationStructure* G4DNAChemistryManager::GetIonisationLevel()
{
    if(!fIonisationLevel)
    {
        fIonisationLevel = new G4WaterIonisationStructure;
    }
    return fIonisationLevel;
}

void G4DNAChemistryManager::CreateWaterMolecule(ElectronicModification modification,
                                                G4int electronicLevel,
                                                const G4Track* theIncomingTrack)
{
    if(fWriteFile)
    {
        G4double energy = -1.;

        switch (modification)
        {
        case fExcitedMolecule :
            energy = GetExcitationLevel()->ExcitationEnergy(electronicLevel);
            break;
        case fIonizedMolecule :
            energy = GetIonisationLevel()->IonisationEnergy(electronicLevel);
            break;
        }

        fOutput << setw(11) << left << theIncomingTrack->GetTrackID()
                << setw(10) << "H2O"
                << left << modification
                << internal <<":"
                << right <<electronicLevel
                << left
                << setw(11) << ""
                << std::setprecision(2) << std::fixed
                << setw(13) << energy/eV
                << std::setprecision(6) << std::scientific
                << setw(22) << (theIncomingTrack->GetPosition().x())/nanometer
                << setw(22) << (theIncomingTrack->GetPosition().y())/nanometer
                << setw(22) << (theIncomingTrack->GetPosition().z())/nanometer
                << G4endl;
    }

    if(fActiveChemistry)
    {
        G4Molecule * H2O = new G4Molecule (G4H2O::Definition());

        switch (modification)
        {
        case fExcitedMolecule :
            H2O -> ExciteMolecule(electronicLevel);
            break;
        case fIonizedMolecule :
            H2O -> IonizeMolecule(electronicLevel);
            break;
        }

        G4Track * H2OTrack = H2O->BuildTrack(1*picosecond,
                                             theIncomingTrack->GetPosition());

        H2OTrack -> SetParentID(theIncomingTrack->GetTrackID());
        H2OTrack -> SetTrackStatus(fStopButAlive);
        H2OTrack -> SetKineticEnergy(0.);

        G4ITStepManager::Instance()->PushTrack(H2OTrack);
    }
}

void G4DNAChemistryManager::CreateSolvatedElectron(const G4Track* theIncomingTrack,
                                                   G4ThreeVector* finalPosition)
{
    if(fWriteFile)
    {
        fOutput << setw(11)<< theIncomingTrack->GetTrackID()
                << setw(10)<< "e_aq"
                << setw(14)<< -1
                << std::setprecision(2) << std::fixed
                << setw(13)<< theIncomingTrack->GetKineticEnergy()/eV
                << std::setprecision(6) << std::scientific
                << setw(22)<< (theIncomingTrack->GetPosition().x())/nanometer
                << setw(22)<< (theIncomingTrack->GetPosition().y())/nanometer
                << setw(22)<< (theIncomingTrack->GetPosition().z())/nanometer  ;

        if(finalPosition != 0)
        {
            fOutput<< setw(14)<< (finalPosition->x())/nanometer
                   << setw(14)<< (finalPosition->y())/nanometer
                   << setw(14)<< (finalPosition->z())/nanometer ;
        }

        fOutput << G4endl;
    }

    if(fActiveChemistry)
    {
        G4Molecule* e_aq = new G4Molecule(G4Electron_aq::Definition());
        G4Track * e_aqTrack = e_aq->BuildTrack(picosecond,theIncomingTrack->GetPosition());
        e_aqTrack -> SetTrackStatus(fAlive);
        e_aqTrack -> SetParentID(theIncomingTrack->GetTrackID());
        G4ITStepManager::Instance()->PushTrack(e_aqTrack);
        G4ITManager<G4Molecule>::Instance()->Push(e_aqTrack);
    }
}
