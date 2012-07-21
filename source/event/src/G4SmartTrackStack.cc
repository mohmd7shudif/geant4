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
// $Id: G4SmartTrackStack.cc,v 1.5 2010-11-24 22:56:57 asaim Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//

#include "simple_probes.h"

#include "G4SmartTrackStack.hh"
#include "G4VTrajectory.hh"

#include "G4Electron.hh"

G4double
G4SmartTrackStack::getEnergyOfStack(G4TrackStackDQ *aTrackStack)
{
	G4double totalEnergy = 0.0f;

	if (aTrackStack) {
		std::vector<G4StackedTrack*>::iterator itr;

		for (itr  = aTrackStack->dqStackedTracks.begin();
		     itr != aTrackStack->dqStackedTracks.end(); itr++) {
			totalEnergy +=
			  (*itr)->GetTrack()->GetDynamicParticle()->GetTotalEnergy();
		}
	}

	return totalEnergy;
}

void
G4SmartTrackStack::dumpStatistics()
{
	// Print to stderr so that we can split stats output
	// from normal output of geant4 which is typically being
	// printed to stdout
	for (int i = 0; i < 5; i++) {
	  G4cerr << stacks[i]->GetNTrack() << " ";
	  G4cerr << getEnergyOfStack(stacks[i]) << " ";
#if 0
	  G4cerr << stacks[i]->GetSafetyValve1() << " ";
	  G4cerr << stacks[i]->GetSafetyValve2() << " ";
	  G4cerr << stacks[i]->GetNStick();
	  G4cerr << G4endl;
#endif
	}
	G4cerr << G4endl;

}

G4SmartTrackStack::G4SmartTrackStack()
	:fTurn(0), nTurn(5)
{
	stacks[0] = new G4TrackStackDQ();
	stacks[1] = new G4TrackStackDQ();
	stacks[2] = new G4TrackStackDQ(G4Electron::Definition()); // electrons
	stacks[3] = new G4TrackStackDQ();
	stacks[4] = new G4TrackStackDQ();

#if 0
	// If entry of one sub-stack exceeds safetyValve1, we will stick
	// to that sub-stack until entry of that sub-stack goes down
	// to safetyValve2.
	nStick = 100;
	safetyValve1 = 3000;
	safetyValve2 = safetyValve1 - nStick;
#endif
	maxNTracks = 0;
}

G4SmartTrackStack::~G4SmartTrackStack()
{
	for (int i  = 0; i < nTurn; i++) {
		delete stacks[i];
	}
}

const G4SmartTrackStack &
G4SmartTrackStack::operator=(const G4SmartTrackStack &)
{
	return *this;
}

int G4SmartTrackStack::operator==(const G4SmartTrackStack &right) const
{
	return (this==&right);
}

int G4SmartTrackStack::operator!=(const G4SmartTrackStack &right) const
{
	return (this!=&right);
}

void G4SmartTrackStack::TransferTo(G4TrackStackDQ * aStack)
{
	for (int i = 0; i < nTurn; i++) {
		stacks[i]->TransferTo(aStack);
	}
}

G4StackedTrack * G4SmartTrackStack::PopFromStack()
{
	G4StackedTrack * aStackedTrack = 0;

	if (n_stackedTrack()) {
		while (!aStackedTrack) {
			if (stacks[fTurn]->GetNTrack()) {
				aStackedTrack = stacks[fTurn]->PopFromStack();
				SIMPLE_POPTRACK(
					stacks[0]->GetNTrack(),
					stacks[1]->GetNTrack(),
					stacks[2]->GetNTrack(),
					stacks[3]->GetNTrack(),
					stacks[4]->GetNTrack());
				energies[fTurn] -=
				    aStackedTrack->GetTrack()->GetDynamicParticle()->GetTotalEnergy();
			} else {
				fTurn = (fTurn+1) % nTurn;
			}
		}
	}

	//	dumpStatistics();

	return aStackedTrack;
}

#include "G4Neutron.hh"
#include "G4Gamma.hh"
#include "G4Positron.hh"

void G4SmartTrackStack::PushToStack( G4StackedTrack * aStackedTrack )
{
	static G4ParticleDefinition* neutDef = G4Neutron::Definition();
	static G4ParticleDefinition* elecDef = G4Electron::Definition();
	static G4ParticleDefinition* gammDef = G4Gamma::Definition();
	static G4ParticleDefinition* posiDef = G4Positron::Definition();

	if (aStackedTrack) {
		G4int iDest = 0;
		if (aStackedTrack->GetTrack()->GetParentID()) {
			G4ParticleDefinition* partDef = aStackedTrack->GetTrack()->GetDefinition();
			if (partDef == elecDef)
				iDest = 2;
			else if (partDef == gammDef)
				iDest = 3;
			else if (partDef == posiDef)
				iDest = 4;
			else if (partDef == neutDef)
				iDest = 1;
		} else {
			// We have a primary track, which should go first.
			fTurn = 0; // reseting the turn
		}
		stacks[iDest]->PushToStack(aStackedTrack);
		SIMPLE_PUSHTRACK(
			stacks[0]->GetNTrack(),
			stacks[1]->GetNTrack(),
			stacks[2]->GetNTrack(),
			stacks[3]->GetNTrack(),
			stacks[4]->GetNTrack());
		energies[iDest] +=
		    aStackedTrack->GetTrack()->GetDynamicParticle()->GetTotalEnergy();

#define abs(x) ((x) >= 0 ? (x) : -(x))

		G4int dy1 = stacks[iDest]->GetNTrack() - stacks[iDest]->GetSafetyValve1();
		G4int dy2 = stacks[fTurn]->GetNTrack() - stacks[fTurn]->GetSafetyValve2();
		if (dy1 > 0 || dy1 > dy2 || (iDest == 2 && energies[iDest] < energies[fTurn])) {
			fTurn = iDest;
		}

		if (n_stackedTrack() > maxNTracks)
			maxNTracks = n_stackedTrack();

		//		dumpStatistics();
	}
}

void G4SmartTrackStack::clear()
{
	for (int i = 0; i < nTurn; i++)
		stacks[i]->clear();
}
