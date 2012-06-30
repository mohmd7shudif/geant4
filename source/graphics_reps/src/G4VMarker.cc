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
// $Id: G4VMarker.cc,v 1.12 2009-02-24 10:58:04 allison Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

#include "G4VMarker.hh"

#include "G4VisAttributes.hh"

G4VMarker::G4VMarker ():
  fPosition   (G4Point3D ()),
  fWorldSize  (0.),
  fScreenSize (0.),
  fFillStyle  (noFill)
{
  // fInfo: default initialisation.
}

G4VMarker::G4VMarker (const G4VMarker& m):
  G4Visible   (m),
  fPosition   (m.fPosition),
  fWorldSize  (m.fWorldSize),
  fScreenSize (m.fScreenSize),
  fFillStyle  (m.fFillStyle),
  fInfo       (m.fInfo)
{}

G4VMarker::G4VMarker (const G4Point3D& pos):
  fPosition   (pos),
  fWorldSize  (0.),
  fScreenSize (0.),
  fFillStyle  (noFill),
  fInfo       (G4String())
{
  // fInfo: default initialisation.
}

G4VMarker::~G4VMarker () {}

G4VMarker& G4VMarker::operator = (const G4VMarker& m) {
  if (&m == this) return *this;
  G4Visible::operator = (m);
  fPosition   = m.fPosition;
  fWorldSize  = m.fWorldSize;
  fScreenSize = m.fScreenSize;
  fFillStyle  = m.fFillStyle;
  fInfo       = m.fInfo;
  return *this;
}

G4bool G4VMarker::operator != (const G4VMarker& m) const {
  if (
      (G4Visible::operator != (m))   ||
      (fWorldSize  != m.fWorldSize)   ||
      (fScreenSize != m.fScreenSize)  ||
      (fFillStyle  != m.fFillStyle)   ||
      !(fPosition  == m.fPosition)    ||
      (fInfo       != m.fInfo)
      )
    return true;
  return false;
}

std::ostream& operator << (std::ostream& os, const G4VMarker& marker) {
  os << "G4VMarker: position: " << marker.fPosition
     << ", world size: " << marker.fWorldSize
     << ", screen size: " << marker.fScreenSize << '\n'
     << "           fill style: ";
  switch (marker.fFillStyle) {
  case G4VMarker::noFill:
    os << "no fill";
    break;
  case G4VMarker::hashed:
    os << "hashed";
    break;
  case G4VMarker::filled:
    os << "filled";
    break;
  default:
    os << "unrecognised"; break;
  }
  if (!marker.fInfo.empty()) os << "\n  User information: " << marker.fInfo;
  os << "\n           " << (const G4Visible&) marker;
  return os;
}

G4VMarker::SizeType G4VMarker::GetSizeType () const {
  SizeType type = none;
  if (fWorldSize) type = world;
  else if (fScreenSize) type = screen;
  return type;
}

void G4VMarker::SetSize (SizeType sizeType, G4double size) {
  fWorldSize = fScreenSize = 0.;
  if (sizeType == world) fWorldSize = size;
  else if (sizeType == screen) fScreenSize = size;
}
