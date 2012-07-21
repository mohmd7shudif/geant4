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

#include "G4TrackStackDQ.hh"
#include "G4SmartTrackStack.hh"
#include "G4VTrajectory.hh"
#include "G4Electron.hh"

G4TrackStackDQ::G4TrackStackDQ()
	:n_stackedTrack(0), firstStackedTrack(0), lastStackedTrack(0)
{
	maxNTracks = 0;

	dqStackedTracks.reserve(5000);
	nstick = 100;
	safetyValve1 = 4000;
	safetyValve2 = safetyValve1 - nstick;
}

// PORTED
G4TrackStackDQ::G4TrackStackDQ(G4ParticleDefinition *partDef)
	:n_stackedTrack(0), firstStackedTrack(0), lastStackedTrack(0)
{
	maxNTracks = 0;

	G4cout << "+++ Using the DQ Track Stack +++       " << n_stackedTrack << G4endl;
	if (partDef == G4Electron::Definition()) {
	  G4cout << "+++ Electron stack created +++" << G4endl;
		dqStackedTracks.reserve(5000);
		nstick = 100;
		safetyValve1 = 4000;
		safetyValve2 = safetyValve1 - nstick;
	} else {
		G4cout << "+++ Default stack created +++" << G4endl;
                dqStackedTracks.reserve(5000);
                nstick = 100;
                safetyValve1 = 4000;
                safetyValve2 = safetyValve1 - nstick;
	}
}

// PORTED
G4TrackStackDQ::~G4TrackStackDQ()
{
	clear();
}

const G4TrackStackDQ & G4TrackStackDQ::operator=(const G4TrackStackDQ &right) 
{
  G4cout << "++++++++++++++++++++++++ = operator called ! right = " << right.n_stackedTrack << G4endl;
	n_stackedTrack    = right.n_stackedTrack;
	firstStackedTrack = right.firstStackedTrack;
	lastStackedTrack  = right.lastStackedTrack;

	return *this; 
}

int G4TrackStackDQ::operator==(const G4TrackStackDQ &right) const
{
	return (firstStackedTrack == right.firstStackedTrack);
}

int G4TrackStackDQ::operator!=(const G4TrackStackDQ &right) const
{
	return (firstStackedTrack != right.firstStackedTrack);
}

// PORTED
void G4TrackStackDQ::TransferTo(G4TrackStackDQ *aStack)
{
	if (aStack->n_stackedTrack) {
		aStack->dqStackedTracks.insert(
			aStack->dqStackedTracks.end(),
			dqStackedTracks.begin(),
			dqStackedTracks.end());

		aStack->lastStackedTrack = lastStackedTrack;
		aStack->n_stackedTrack += n_stackedTrack;
	} else {
		*aStack = *this;
	}
	
	n_stackedTrack    = 0;
	firstStackedTrack = 0;
	lastStackedTrack  = 0;
}

// PORTED
void G4TrackStackDQ::TransferTo(G4SmartTrackStack *aStack)
{
	while (n_stackedTrack)
		aStack->PushToStack(PopFromStack());
}

// PORTED
G4StackedTrack *G4TrackStackDQ::PopFromStack()
{
	G4StackedTrack *aStackedTrack = NULL;

	if (n_stackedTrack) {
		aStackedTrack = dqStackedTracks.back();
		dqStackedTracks.pop_back();
		n_stackedTrack--;

		firstStackedTrack = dqStackedTracks.front();
		if (!dqStackedTracks.empty())
		  lastStackedTrack  = dqStackedTracks.back();
		else
		  lastStackedTrack = NULL;
	}

	return aStackedTrack;
}

// PORTED
void G4TrackStackDQ::PushToStack(G4StackedTrack *aStackedTrack)
{
	if (aStackedTrack) {
		dqStackedTracks.push_back(aStackedTrack);
		lastStackedTrack = aStackedTrack;
		n_stackedTrack++;
		if (n_stackedTrack > maxNTracks)
			maxNTracks = n_stackedTrack;
	}
}

// PORTED
void G4TrackStackDQ::clear()
{
	// delete tracks in the stack
	std::vector<G4StackedTrack*>::iterator itr;
	for (itr  = dqStackedTracks.begin();
	     itr != dqStackedTracks.end(); itr++) {  
	  delete (*itr)->GetTrack();
	  delete (*itr)->GetTrajectory();
	  delete (*itr);
	}

	n_stackedTrack    = 0;
	firstStackedTrack = 0;
	lastStackedTrack  = 0;
}
