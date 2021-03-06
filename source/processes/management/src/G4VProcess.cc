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
// $Id: G4VProcess.cc,v 1.18 2010-12-22 09:14:54 kurasige Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// --------------------------------------------------------------
//	GEANT 4 class implementation file
//
//	History: first implementation, based on object model of
//	2nd December 1995, G.Cosmo
// --------------------------------------------------------------
//   New Physics scheme           8 Jan. 1997  H.Kurahige
// ------------------------------------------------------------
//   removed thePhysicsTable           02 Aug. 1998 H.Kurashige
//   Modified DumpInfo                 15 Aug. 1998 H.Kurashige

#include "G4PhysicsTable.hh"
#include "G4MaterialTable.hh"
#include "G4ElementTable.hh"
#include "G4ElementVector.hh"
#include "G4VProcess.hh"

G4VProcess::G4VProcess(const G4String& aName, G4ProcessType   aType )
                  : aProcessManager(0),
	            pParticleChange(0),
                    theNumberOfInteractionLengthLeft(-1.0),
                    currentInteractionLength(-1.0),
                    theProcessName(aName),
		    theProcessType(aType),
		    theProcessSubType(-1),
                    thePILfactor(1.0),
                    enableAtRestDoIt(true),
                    enableAlongStepDoIt(true),
                    enablePostStepDoIt(true),
                    verboseLevel(0)

{
  pParticleChange = &aParticleChange;
}

G4VProcess::~G4VProcess()
{
}

G4VProcess::G4VProcess(const G4VProcess& right)
          : aProcessManager(0),
	    pParticleChange(0),
            theNumberOfInteractionLengthLeft(-1.0),
            currentInteractionLength(-1.0),
            theProcessName(right.theProcessName),
            theProcessType(right.theProcessType),
	    theProcessSubType(right.theProcessSubType),
            thePILfactor(1.0),
            enableAtRestDoIt(right.enableAtRestDoIt),
            enableAlongStepDoIt(right.enableAlongStepDoIt),
            enablePostStepDoIt(right.enablePostStepDoIt),
            verboseLevel(right.verboseLevel)
{
}


void G4VProcess::SubtractNumberOfInteractionLengthLeft(
                                  G4double previousStepSize )
{
  if (currentInteractionLength>0.0) {
    theNumberOfInteractionLengthLeft -= previousStepSize/currentInteractionLength;
    if(theNumberOfInteractionLengthLeft<0.) {
       theNumberOfInteractionLengthLeft=perMillion;
    }          

  } else {
#ifdef G4VERBOSE
    if (verboseLevel>0) {
      G4cerr << "G4VProcess::SubtractNumberOfInteractionLengthLeft()";
      G4cerr << " [" << theProcessName << "]" <<G4endl;
      G4cerr << " currentInteractionLength = " << currentInteractionLength/cm << " [cm]";
      G4cerr << " previousStepSize = " << previousStepSize/cm << " [cm]";
      G4cerr << G4endl;
    }
#endif
    G4String msg = "Negative currentInteractionLength for ";
    msg += 	theProcessName;
    G4Exception("G4VProcess::SubtractNumberOfInteractionLengthLeft()",
		"ProcMan201",EventMustBeAborted,
		msg);
  }
}

void G4VProcess::StartTracking(G4Track*)
{
  currentInteractionLength = -1.0;
  theNumberOfInteractionLengthLeft = -1.0;
#ifdef G4VERBOSE
  if (verboseLevel>2) {
    G4cout << "G4VProcess::StartTracking() [" << theProcessName << "]" <<G4endl;
  }
#endif
}

void G4VProcess::EndTracking()
{
#ifdef G4VERBOSE
  if (verboseLevel>2) {
    G4cout << "G4VProcess::EndTracking() [" << theProcessName << "]" <<G4endl;
  }
#endif
  theNumberOfInteractionLengthLeft = -1.0;
  currentInteractionLength = -1.0;
}


const G4String& G4VProcess::GetProcessTypeName(G4ProcessType aType ) 
{
  static G4String typeNotDefined= "NotDefined";
  static G4String typeTransportation = "Transportation";
  static G4String typeElectromagnetic = "Electromagnetic";
  static G4String typeOptical = "Optical";
  static G4String typeHadronic = "Hadronic";
  static G4String typePhotolepton_hadron = "Photolepton_hadron";
  static G4String typeDecay = "Decay";
  static G4String typeGeneral = "General";
  static G4String typeParameterisation = "Parameterisation";
  static G4String typeUserDefined = "UserDefined";
  static G4String noType = "------";   // Do not modify this !!!!

  if (aType ==   fNotDefined) {
    return  typeNotDefined;
  } else if  (aType ==   fTransportation ) {
    return typeTransportation;
  } else if  (aType ==   fElectromagnetic ) {
    return typeElectromagnetic;
  } else if  (aType ==   fOptical ) {
    return typeOptical;
  } else if  (aType ==   fHadronic ) {
    return typeHadronic;
  } else if  (aType ==   fPhotolepton_hadron ) {
    return typePhotolepton_hadron;
  } else if  (aType ==   fDecay ) {
    return typeDecay;
  } else if  (aType ==   fGeneral ) {
    return typeGeneral;
  } else if  (aType ==   fParameterisation ) {
    return typeParameterisation;
  } else if  (aType ==   fUserDefined ) {
    return typeUserDefined;
  } else {
    return noType;  
  }
}

G4VProcess & G4VProcess::operator=(const G4VProcess &)
{
  G4Exception("G4VProcess::operator=","ProcMan101",
	      JustWarning,"Assignment operator is called but NO effect");
  return *this;
}

G4int G4VProcess::operator==(const G4VProcess &right) const
{
  return (this == &right);
}

G4int G4VProcess::operator!=(const G4VProcess &right) const
{
  return (this !=  &right);
}

void G4VProcess::DumpInfo() const
{
  G4cout << "Process Name " << theProcessName ;
  G4cout << " : Type[" << GetProcessTypeName(theProcessType) << "]";
  G4cout << " : SubType[" << theProcessSubType << "]"<< G4endl;
}


const G4String&  G4VProcess::GetPhysicsTableFileName(const G4ParticleDefinition* particle,
						     const G4String& directory,
						     const G4String& tableName,
						     G4bool ascii)
{
  G4String thePhysicsTableFileExt;
  if (ascii) thePhysicsTableFileExt = ".asc";
  else       thePhysicsTableFileExt = ".dat";

  thePhysicsTableFileName = directory + "/";
  thePhysicsTableFileName += tableName + "." +  theProcessName + ".";
  thePhysicsTableFileName += particle->GetParticleName() + thePhysicsTableFileExt;
  
  return thePhysicsTableFileName;
}











