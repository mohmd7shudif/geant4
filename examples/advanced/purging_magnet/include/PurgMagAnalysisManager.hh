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
// Code developed by:
//  S.Larsson
//
//    ************************************
//    *                                  *
//    *    PurgMagAnalysisManager.hh     *
//    *                                  *
//    ************************************
//
// $Id: PurgMagAnalysisManager.hh,v 1.4 2009-04-08 10:23:31 cirrone Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//

#ifdef G4ANALYSIS_USE
#ifndef G4PROCESSTESTANALYSIS_HH
#define G4PROCESSTESTANALYSIS_HH

#include "globals.hh"
#include <vector>
#include "G4ThreeVector.hh"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IAnalysisFactory.h"

namespace AIDA
{
  class ITree;
  class IHistogramFactory;
  class IAnalysisFactory;
  class ITupleFactory;
  class ITuple;
  class ITreeFactory;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class PurgMagAnalysisManager

{
public:
  
  ~PurgMagAnalysisManager();
  
  void book();
  
  void finish();
  
  static PurgMagAnalysisManager* getInstance();
  
  void fill_Tuple_Electrons(G4double,G4double,G4double,G4double,G4double,G4double,G4double);
  void fill_Tuple_Gamma(G4double,G4double,G4double,G4double,G4double,G4double,G4double);
  void fill_Tuple_Positrons(G4double,G4double,G4double,G4double,G4double,G4double,G4double);
  
private:
  
  G4double ex,ey,ez,ee,epx,epy,epz,gx,gy,gz,ge,gpx,gpy,gpz,px,py,pz,pe,ppx,ppy,ppz;
  
  static PurgMagAnalysisManager* instance;
  
private:
  PurgMagAnalysisManager();
  
private:
  
  AIDA::IAnalysisFactory*  aFact;
  AIDA::ITree*             theTree;
  AIDA::IHistogramFactory *histFact;
  AIDA::ITupleFactory     *tupFact;
  AIDA::ITreeFactory      *treeFact;
  AIDA::ITuple *ntuple1;
  AIDA::ITuple *ntuple2;
  AIDA::ITuple *ntuple3;
};

#endif
#endif



