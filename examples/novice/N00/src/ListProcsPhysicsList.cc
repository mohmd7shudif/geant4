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

#include <iomanip>

#include "ListProcsPhysicsList.hh"
#include "G4RunManager.hh"

ListProcsPhysicsList::ListProcsPhysicsList()
{
}

ListProcsPhysicsList::~ListProcsPhysicsList()
{
}

/*******************************************************************************
 * For every particle, print its name, type and available physics' processes to
 * standard output stream.
 ******************************************************************************/

void ListProcsPhysicsList::dumpAllParticlesAndProcesses() const
{
	for (theParticleIterator->reset();
	     (*theParticleIterator)();
	     /* none */) {
		G4ParticleDefinition *pParticle = theParticleIterator->value();
		G4ProcessManager *pProcManager = pParticle->GetProcessManager();

		G4ProcessVector *pProcVector = pProcManager->GetProcessList();
		for (int i = 0; i < pProcVector->size(); i++) {
			G4cout << std::left
			       << std::setw(22) << pParticle->GetParticleName() << " "
			       << std::setw(15) << pParticle->GetParticleType() << " "
			       << std::setw(22) << (*pProcVector)[i]->GetProcessName()
			       << G4endl;
		}
	}
}
