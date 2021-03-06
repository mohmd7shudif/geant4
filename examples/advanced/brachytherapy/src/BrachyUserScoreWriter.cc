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
/*
// Code developed by:
// S.Guatelli, susanna@uow.edu.au
//
Original code from geant4/examples/extended/runAndEvent/RE03, by M. Asai
*/

#include "BrachyUserScoreWriter.hh"
#include "BrachyAnalysis.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4SDParticleFilter.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4VScoringMesh.hh"

#include <map>
#include <fstream>

// The default output is
// voxelX, voxelY, voxelZ, edep
// The BrachyUserScoreWriter allows to change the format of the output file.
// in the specific case:
// xx (mm)  yy(mm) zz(mm) edep(keV)

// The same information is stored in a ntuple, in the 
// brachytherapy.root file

BrachyUserScoreWriter::BrachyUserScoreWriter(): G4VScoreWriter() 
{;}

BrachyUserScoreWriter::~BrachyUserScoreWriter() 
{;}

void BrachyUserScoreWriter::DumpQuantityToFile(G4String & psName, G4String & fileName, G4String & option) 
{
  if(verboseLevel > 0) {
    G4cout << "BrachyUserScorer-defined DumpQuantityToFile() method is invoked."
           << G4endl; }

  // change the option string into lowercase to the case-insensitive.
  G4String opt = option;
  std::transform(opt.begin(), opt.end(), opt.begin(), (int (*)(int))(tolower));

  // confirm the option
  if(opt.size() == 0) opt = "csv";

  // open the file
  std::ofstream ofile(fileName);
  if(!ofile) {
    G4cerr << "ERROR : DumpToFile : File open error -> "
	   << fileName << G4endl;
    return;
  }
  ofile << "# mesh name: " << fScoringMesh->GetWorldName() << G4endl;

  // retrieve the map
  MeshScoreMap fSMap = fScoringMesh -> GetScoreMap();
  
  MeshScoreMap::const_iterator msMapItr = fSMap.find(psName);
  if(msMapItr == fSMap.end()) {
    G4cerr << "ERROR : DumpToFile : Unknown quantity, \""
	   << psName << "\"." << G4endl;
    return;
  }
  std::map<G4int, G4double*> * score = msMapItr -> second-> GetMap();
  ofile << "# primitive scorer name: " << msMapItr -> first << G4endl;


  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  //
  // Open a ROOT output file
  //

  analysisManager -> OpenFile("brachytherapy");
   
  //
  // Creating ntuple
  //
  analysisManager -> CreateNtuple("EnergyDeposition", "Edep(keV) in the phantom");
  analysisManager -> CreateNtupleDColumn("xx");
  analysisManager -> CreateNtupleDColumn("yy");
  analysisManager -> CreateNtupleDColumn("zz");
  analysisManager -> CreateNtupleDColumn("edep");
  analysisManager -> FinishNtuple();

 //
 // Write quantity in the ASCII output file and in brachytherapy.root
 //
 ofile << std::setprecision(16); // for double value with 8 bytes
  for(int x = 0; x < fNMeshSegments[0]; x++) {
    for(int y = 0; y < fNMeshSegments[1]; y++) {   
       for(int z = 0; z < fNMeshSegments[2]; z++) {

       G4int numberOfVoxel = fNMeshSegments[0];

       // If the voxel width is changed in the macro file, 
       // the voxel width variable must be updated
       G4double voxelWidth = 1. *mm;
       //

       G4double xx = ( - numberOfVoxel + 1+ 2*x )* voxelWidth/2; 
       G4double yy = ( - numberOfVoxel + 1+ 2*y )* voxelWidth/2;
       G4double zz = ( - numberOfVoxel + 1+ 2*z )* voxelWidth/2;

        G4int idx = GetIndex(x, y, z);
	
	std::map<G4int, G4double*>::iterator value = score -> find(idx);
    
      if (value != score -> end()) 
        {
        // Print in the ASCII output file the information
        ofile << xx << "  " << yy << "  " << zz <<"  " <<*(value->second)/keV << G4endl;
           
        // Save the same information in the output analysis file
        analysisManager = G4AnalysisManager::Instance();
        analysisManager -> FillNtupleDColumn(0, xx);
        analysisManager -> FillNtupleDColumn(1, yy);
        analysisManager -> FillNtupleDColumn(2, zz);
        analysisManager -> FillNtupleDColumn(3, *(value->second)/keV);
        analysisManager -> AddNtupleRow();
        }
     } 
    } 
  } 
  ofile << std::setprecision(6);

  // Close the output ASCII file
  ofile.close();

  // Close the output brachytherapy.root 
  analysisManager -> Write();
  analysisManager -> CloseFile();
}

