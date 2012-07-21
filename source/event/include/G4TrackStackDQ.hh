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

#ifndef G4TrackStackDQ_h
#define G4TrackStackDQ_h 1

#include <vector>
#include "G4StackedTrack.hh"
#include "globals.hh"

class G4SmartTrackStack;

// class description:
//
// This is a stack class used by G4StackManager. This class object
// stores G4StackedTrack class objects in the form of bi-directional
// linked list.

class G4TrackStackDQ 
{
public:
        G4TrackStackDQ();
	G4TrackStackDQ(G4ParticleDefinition *partDef);
	~G4TrackStackDQ();

private:
	const G4TrackStackDQ & operator=(const G4TrackStackDQ &right);
	G4int operator==(const G4TrackStackDQ &right) const;
	G4int operator!=(const G4TrackStackDQ &right) const;

public:
	void PushToStack(G4StackedTrack *aStackedTrack);
	G4StackedTrack *PopFromStack();
	void clear();
	void TransferTo(G4TrackStackDQ *aStack);
	void TransferTo(G4SmartTrackStack *aStack);

private:
	G4int n_stackedTrack;
	G4StackedTrack *firstStackedTrack;
	G4StackedTrack *lastStackedTrack;
	G4int maxNTracks;
	G4int safetyValve1, safetyValve2;
	G4int nstick;

public: // XXX
	std::vector<G4StackedTrack*> dqStackedTracks;

public:
	inline G4int GetNTrack()    const { return n_stackedTrack; }
	inline G4int GetMaxNTrack() const { return maxNTracks; }

	inline  G4int GetSafetyValve1() const
	{ return safetyValve1; }
	inline G4int GetSafetyValve2() const
	{ return safetyValve2; }
	inline G4int GetNStick() const
	{ return nstick; }

	G4double getTotalEnergy(void) const;

	void SetSafetyValve2(int x) { if (x < 0) { x = 0; } safetyValve2 = x; }
};

#endif
