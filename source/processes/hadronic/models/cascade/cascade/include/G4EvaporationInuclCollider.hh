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
// $Id: G4EvaporationInuclCollider.hh,v 1.6 2010-07-14 15:41:12 mkelsey Exp $
// GEANT4 tag: $Name: not supported by cvs2svn $
//
// 20100315  M. Kelsey -- Remove "using" directive and unnecessary #includes.
// 20100413  M. Kelsey -- Pass G4CollisionOutput by ref to ::collide()
// 20100517  M. Kelsey -- Inherit from common base class, make other colliders
//		simple data members
// 20100714  M. Kelsey -- Switch to new G4CascadeColliderBase class
// 20110728  M. Kelsey -- Fix Coverity #23843, add destructor.

#ifndef G4EVAPORATIONINUCL_COLLIDER_HH
#define G4EVAPORATIONINUCL_COLLIDER_HH
 
#include "G4CascadeColliderBase.hh"

class G4InuclParticle;
class G4CollisionOutput;
class G4EquilibriumEvaporator;
class G4BigBanger;

class G4EvaporationInuclCollider : public G4CascadeColliderBase {
public:
  G4EvaporationInuclCollider();
  ~G4EvaporationInuclCollider();

  void collide(G4InuclParticle* bullet, G4InuclParticle* target,
	       G4CollisionOutput& output);
  
private: 
  G4EquilibriumEvaporator* theEquilibriumEvaporator;
};        

#endif /* G4EVAPORATIONINUCL_COLLIDER_HH */


