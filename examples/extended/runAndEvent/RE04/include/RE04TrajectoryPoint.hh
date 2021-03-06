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


#ifndef RE04TrajectoryPoint_h
#define RE04TrajectoryPoint_h 1

#include "G4VTrajectoryPoint.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Allocator.hh"
class G4Material;

////////////////////////
class RE04TrajectoryPoint : public G4VTrajectoryPoint
//////////////////////// 
{

//--------
public: 
//--------

// Constructor/Destructor
   RE04TrajectoryPoint();
   RE04TrajectoryPoint(G4ThreeVector pos,const G4Material* mat);
   RE04TrajectoryPoint(const RE04TrajectoryPoint &right);
   virtual ~RE04TrajectoryPoint();

// Operators
   inline void *operator new(size_t);
   inline void operator delete(void *aTrajectoryPoint);
   inline int operator==(const RE04TrajectoryPoint& right) const
   { return (this==&right); };

// Get/Set functions
   inline const G4ThreeVector GetPosition() const
   { return fPosition; };
   inline const G4Material* GetMaterial() const
   { return fpMaterial; };

// Get method for HEPRep style attributes
   virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
   virtual std::vector<G4AttValue>* CreateAttValues() const;

//---------
   private:
//---------

// Member data
   G4ThreeVector fPosition;
   const G4Material* fpMaterial;

};

extern G4Allocator<RE04TrajectoryPoint> aTrajPointAllocator;

inline void* RE04TrajectoryPoint::operator new(size_t)
{
   void *aTrajectoryPoint;
   aTrajectoryPoint = (void *) aTrajPointAllocator.MallocSingle();
   return aTrajectoryPoint;
}

inline void RE04TrajectoryPoint::operator delete(void *aTrajectoryPoint)
{
   aTrajPointAllocator.FreeSingle((RE04TrajectoryPoint *) aTrajectoryPoint);
}

#endif

