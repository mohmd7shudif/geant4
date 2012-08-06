#include <iomanip>

#include "G4RunManager.hh"
#include "G4PhysListFactory.hh"

#include "ListProcsDetectorConstruction.hh"
#include "ListProcsPhysicsList.hh"
#include "ListProcsPrimaryGeneratorAction.hh"

// Function prototypes
static void usage(const char *name);
static void dumpAllPhysicsLists(const G4PhysListFactory *physListFactory);

int
main(int argc, char *argv[])
{
	G4String physListName;
	if (argc > 2) {
		usage(argv[0]);
		// NEVER REACHED
	} else if (argc == 2) {
		physListName = argv[1];
		G4cout << "Using Physics List: '" << physListName << "'" << G4endl;
	} else {
		physListName = "QGSP_BERT";
		G4cout << "Using Physics List: 'QGSP_BERT' [default]" << G4endl;
	}

	G4RunManager* runManager = new G4RunManager;

	G4PhysListFactory factory;
	G4VUserPhysicsList* physList;
	if (factory.IsReferencePhysList(physListName)) {
		physList = factory.GetReferencePhysList(physListName);
		runManager->SetUserInitialization(physList);
	} else {
		G4cerr << "ERROR Physics List: '" << physListName
		       << "' does NOT exist"	  << G4endl;
		usage(argv[0]);
		// NEVER REACHED
	}

	// Set mandatory initialization and user action classes
	G4VUserDetectorConstruction   *detector   =
	    new ListProcsDetectorConstruction;
	G4VUserPrimaryGeneratorAction *gen_action =
	    new ListProcsPrimaryGeneratorAction;
	runManager->SetUserInitialization(detector);
       	runManager->SetUserAction(gen_action);

	runManager->Initialize();

	// We are downcasting, which needs to be explicit
	ListProcsPhysicsList *ppl = static_cast<ListProcsPhysicsList *>(physList);
	ppl->dumpAllParticlesAndProcesses();

	delete runManager;

	return 0;
}

/*******************************************************************************
 * Prints the available physics lists of a G4PhysListFactory object.
 ******************************************************************************/

static void
dumpAllPhysicsLists(const G4PhysListFactory *physListFactory)
{
	if (physListFactory) {
		std::vector<G4String> availablePhysLists =
		    physListFactory->AvailablePhysLists();

		for (unsigned int i = 0; i < availablePhysLists.size(); i++) {
			if (i && i % 4 == 0)
				G4cerr << G4endl;
			G4cerr << std::left
			       << std::setw(15)
			       << availablePhysLists[i] << " ";
		}
		G4cerr << G4endl;
	}
}

/*******************************************************************************
 * Prints usage syntax, along with the available physics lists to standard error
 * stream and then exits with a failure exit code.
 *******************************************************************************/

static void
usage(const char *name)
{
	G4cerr << "Usage: " << name << " [<Physics List>]" << G4endl;
	G4cerr << "\t\t--- Available Physics Lists ---" << G4endl;

	G4PhysListFactory factory;
	dumpAllPhysicsLists(&factory);

	exit(EXIT_FAILURE);
	// NEVER REACHED
}
