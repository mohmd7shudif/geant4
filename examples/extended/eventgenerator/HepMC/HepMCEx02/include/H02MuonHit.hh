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
// ====================================================================
//
//   H02MuonHit.hh
//   $Id: H02MuonHit.hh,v 1.4 2006-06-29 17:09:53 gunter Exp $
//
// ====================================================================
#ifndef H02_MUON_HIT_H
#define H02_MUON_HIT_H
 
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class H02MuonHit : public G4VHit {
private:
  G4int moduleID;
  G4String pname;
  G4ThreeVector momentum;
  G4ThreeVector position;
  G4double tof;

public:
  H02MuonHit();
  H02MuonHit(G4int imod, G4String aname, const G4ThreeVector& pxyz,
	    const G4ThreeVector& xyz, G4double atof);
  ~H02MuonHit();

  H02MuonHit(const H02MuonHit& right);
  const H02MuonHit& operator=(const H02MuonHit& right);
  G4int operator==(const H02MuonHit& right) const;
  
  void* operator new(size_t);
  void operator delete(void* aHit);

  // set/get functions...
  void SetModuleID(G4int i);
  G4int GetModuleID() const;

  void SetParticle(G4String aname);
  G4String GetParticle() const;

  void SetMomentum(const G4ThreeVector& pxyz);
  G4ThreeVector GetMomentum() const;

  void SetPosition(const G4ThreeVector& xyz);
  G4ThreeVector GetPosition() const;

  void SetTOF(G4double atof);
  G4double GetTOF() const;

  // methods...
  virtual void Draw();
  virtual void Print();  
};

// ====================================================================
// inline functions
// ====================================================================

inline void H02MuonHit::SetModuleID(G4int i) { moduleID=i; }
inline G4int H02MuonHit::GetModuleID() const { return moduleID; }

inline void H02MuonHit::SetParticle(G4String aname) { pname=aname; }
inline G4String H02MuonHit::GetParticle() const { return pname; }

inline void H02MuonHit::SetMomentum(const G4ThreeVector& pxyz) 
{ momentum=pxyz; }
inline G4ThreeVector H02MuonHit::GetMomentum() const { return momentum; }

inline void H02MuonHit::SetPosition(const G4ThreeVector& xyz) { position=xyz; }
inline G4ThreeVector H02MuonHit::GetPosition() const { return position; }

inline void H02MuonHit::SetTOF(G4double atof) { tof=atof; }
inline G4double H02MuonHit::GetTOF() const { return tof; }

typedef G4THitsCollection<H02MuonHit> H02MuonHitsCollection;
extern G4Allocator<H02MuonHit> H02MuonHitAllocator;

inline void* H02MuonHit::operator new(size_t)
{
  void* aHit;
  aHit= (void*)H02MuonHitAllocator.MallocSingle();
  return aHit;
}

inline void H02MuonHit::operator delete(void* aHit)
{
  H02MuonHitAllocator.FreeSingle((H02MuonHit*) aHit);
}

#endif
