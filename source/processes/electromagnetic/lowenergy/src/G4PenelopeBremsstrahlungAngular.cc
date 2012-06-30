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
// $Id: G4PenelopeBremsstrahlungAngular.cc,v 1.1 2010-12-20 14:11:37 pandola Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
// 
// --------------------------------------------------------------
//
// File name:     G4PenelopeBremsstrahlungAngular
//
// Author:        Luciano Pandola
// 
// Creation date: November 2010
//
// History:
// -----------
// 23 Nov 2010  L. Pandola       1st implementation
// 24 May 2011  L. Pandola       Renamed (make v2008 as default Penelope)
//
//----------------------------------------------------------------

#include "G4PenelopeBremsstrahlungAngular.hh"
#include "G4PhysicsFreeVector.hh"
#include "G4PhysicsTable.hh"
#include "Randomize.hh"
#include "globals.hh"

G4PenelopeBremsstrahlungAngular::G4PenelopeBremsstrahlungAngular() : 
  theLorentzTables1(0),theLorentzTables2(0)
{
  dataRead = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4PenelopeBremsstrahlungAngular::~G4PenelopeBremsstrahlungAngular()
{
  ClearTables();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4PenelopeBremsstrahlungAngular::ClearTables()
{
  std::map<G4double,G4PhysicsTable*>::iterator j;

  if (theLorentzTables1)
    {
      for (j=theLorentzTables1->begin(); j != theLorentzTables1->end(); j++)
        {
	  G4PhysicsTable* tab = j->second;
          tab->clearAndDestroy();
          delete tab;
        }
      delete theLorentzTables1;
      theLorentzTables1 = 0;
    }

  if (theLorentzTables2)
    {
      for (j=theLorentzTables2->begin(); j != theLorentzTables2->end(); j++)
        {
	  G4PhysicsTable* tab = j->second;
          tab->clearAndDestroy();
          delete tab;
        }
      delete theLorentzTables2;
      theLorentzTables2 = 0;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4PenelopeBremsstrahlungAngular::ReadDataFile()
{
   //Read information from DataBase file
  char* path = getenv("G4LEDATA");
  if (!path)
    {
      G4String excep = 
	"G4PenelopeBremsstrahlungAngular - G4LEDATA environment variable not set!";
      G4Exception("G4PenelopeBremsstrahlungAngular::ReadDataFile()",
		  "em0006",FatalException,excep);
      return;
    }
  G4String pathString(path);
  G4String pathFile = pathString + "/penelope/bremsstrahlung/pdbrang.p08";
  std::ifstream file(pathFile);
  
  if (!file.is_open())
    {
      G4String excep = "G4PenelopeBremsstrahlungAngular - data file " + pathFile + " not found!";
      G4Exception("G4PenelopeBremsstrahlungAngular::ReadDataFile()",
		  "em0003",FatalException,excep);
      return;
    }
  G4int i=0,j=0,k=0; // i=index for Z, j=index for E, k=index for K 

  for (k=0;k<NumberofKPoints;k++)    
    for (i=0;i<NumberofZPoints;i++)	
      for (j=0;j<NumberofEPoints;j++)
	{
	  G4double a1,a2;
	  G4int ik1,iz1,ie1; 
	  G4double zr,er,kr;
	  file >> iz1 >> ie1 >> ik1 >> zr >> er >> kr >> a1 >> a2;
	  //check the data are correct
	  if ((iz1-1 == i) && (ik1-1 == k) && (ie1-1 == j))
	    {
	      QQ1[i][j][k]=a1;
	      QQ2[i][j][k]=a2;
	    }
	  else
	    {	     
	      G4ExceptionDescription ed;
	      ed << "Corrupted data file " << pathFile << "?" << G4endl;
	      G4Exception("G4PenelopeBremsstrahlungAngular::ReadDataFile()",
		  "em0005",FatalException,ed);	  
	    }
	}   
  file.close();
  dataRead = true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4PenelopeBremsstrahlungAngular::PrepareInterpolationTables(G4double Zmat)
{
  //Check if data file has already been read
  if (!dataRead)
    {
      ReadDataFile();
      if (!dataRead)	
	G4Exception("G4PenelopeBremsstrahlungAngular::PrepareInterpolationTables()",
		    "em2001",FatalException,"Unable to build interpolation table");
    }

  const G4int reducedEnergyGrid=21;
  //Support arrays. 
  G4double betas[NumberofEPoints]; //betas for interpolation
  //tables for interpolation
  G4double Q1[NumberofEPoints][NumberofKPoints];
  G4double Q2[NumberofEPoints][NumberofKPoints];
  //expanded tables for interpolation
  G4double Q1E[NumberofEPoints][reducedEnergyGrid];
  G4double Q2E[NumberofEPoints][reducedEnergyGrid]; 
  G4double pZ[NumberofZPoints] = {2.0,8.0,13.0,47.0,79.0,92.0};

  G4int i=0,j=0,k=0; // i=index for Z, j=index for E, k=index for K 
  //Interpolation in Z
  for (i=0;i<NumberofEPoints;i++)
    {
      for (j=0;j<NumberofKPoints;j++)
	{
	  G4PhysicsFreeVector* QQ1vector = new G4PhysicsFreeVector(NumberofZPoints);
	  QQ1vector->SetSpline(true);
	  G4PhysicsFreeVector* QQ2vector = new G4PhysicsFreeVector(NumberofZPoints);
	  QQ2vector->SetSpline(true);

	  //fill vectors
	  for (k=0;k<NumberofZPoints;k++)
	    {
	      QQ1vector->PutValue(k,pZ[k],std::log(QQ1[k][i][j]));
	      QQ2vector->PutValue(k,pZ[k],QQ2[k][i][j]);
	    }
	  
	  Q1[i][j]= std::exp(QQ1vector->Value(Zmat));	  
	  Q2[i][j]=QQ2vector->Value(Zmat);
	  delete QQ1vector;
	  delete QQ2vector;
	}
    }
  G4double pE[NumberofEPoints] = {1.0e-03*MeV,5.0e-03*MeV,1.0e-02*MeV,5.0e-02*MeV,
				  1.0e-01*MeV,5.0e-01*MeV};
  G4double pK[NumberofKPoints] = {0.0,0.6,0.8,0.95};
  G4double ppK[reducedEnergyGrid]; 

  for(i=0;i<reducedEnergyGrid;i++)
    ppK[i]=((G4double) i) * 0.05;
  

  for(i=0;i<NumberofEPoints;i++)
    betas[i]=std::sqrt(pE[i]*(pE[i]+2*electron_mass_c2))/(pE[i]+electron_mass_c2);
  

  for (i=0;i<NumberofEPoints;i++)
    {
      for (j=0;j<NumberofKPoints;j++)
	Q1[i][j]=Q1[i][j]/Zmat;
    }

  //Expanded table of distribution parameters
  for (i=0;i<NumberofEPoints;i++)
    {
      G4PhysicsFreeVector* Q1vector = new G4PhysicsFreeVector(NumberofKPoints);    
      G4PhysicsFreeVector* Q2vector = new G4PhysicsFreeVector(NumberofKPoints);

      for (j=0;j<NumberofKPoints;j++)
	{
	  Q1vector->PutValue(j,pK[j],std::log(Q1[i][j])); //logarithmic 
	  Q2vector->PutValue(j,pK[j],Q2[i][j]);
	}

      for (j=0;j<reducedEnergyGrid;j++)
	{
	  Q1E[i][j]=Q1vector->Value(ppK[j]);
	  Q2E[i][j]=Q2vector->Value(ppK[j]);
	}
      delete Q1vector;
      delete Q2vector;
    } 
  //
  //TABLES to be stored
  //
  G4PhysicsTable* theTable1 = new G4PhysicsTable();
  G4PhysicsTable* theTable2 = new G4PhysicsTable();
  // the table will contain reducedEnergyGrid G4PhysicsFreeVectors with different
  // values of k, 
  // Each of the G4PhysicsFreeVectors has a profile of
  // y vs. E
  //
  //reserve space of the vectors.
  for (j=0;j<reducedEnergyGrid;j++)   
    {
      G4PhysicsFreeVector* thevec = new G4PhysicsFreeVector(NumberofEPoints);
      thevec->SetSpline(true);
      theTable1->push_back(thevec);
      G4PhysicsFreeVector* thevec2 = new G4PhysicsFreeVector(NumberofEPoints);
      thevec2->SetSpline(true);
      theTable2->push_back(thevec2);
    }  

  for (j=0;j<reducedEnergyGrid;j++)
    {
      G4PhysicsFreeVector* thevec = (G4PhysicsFreeVector*) (*theTable1)[j];
      G4PhysicsFreeVector* thevec2 = (G4PhysicsFreeVector*) (*theTable2)[j];
      for (i=0;i<NumberofEPoints;i++)
	{
	  thevec->PutValue(i,betas[i],Q1E[i][j]);
	  thevec2->PutValue(i,betas[i],Q2E[i][j]);
	}
    }

  if (theLorentzTables1 && theLorentzTables2)
    {
      theLorentzTables1->insert(std::make_pair(Zmat,theTable1));
      theLorentzTables2->insert(std::make_pair(Zmat,theTable2));
    }
  else
    {
      G4ExceptionDescription ed;
      ed << "Unable to create tables of Lorentz coefficients for " << G4endl;
      ed << "<Z>= "  << Zmat << " in G4PenelopeBremsstrahlungAngular" << G4endl;
      delete theTable1;
      delete theTable2;
      G4Exception("G4PenelopeBremsstrahlungAngular::PrepareInterpolationTables()",
		  "em2005",FatalException,ed);	
    }
  
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double G4PenelopeBremsstrahlungAngular::SampleCosTheta(G4double Zmat,
							   G4double ePrimary,
							   G4double eGamma)
{
  G4double beta = std::sqrt(ePrimary*(ePrimary+2*electron_mass_c2))/
    (ePrimary+electron_mass_c2);
  G4double cdt = 0;

  //Use a pure dipole distribution for energy above 500 keV
  if (ePrimary > 500*keV)
    {
      cdt = 2.0*G4UniformRand() - 1.0;
      if (G4UniformRand() > 0.75)
	{
	  if (cdt<0)
	    cdt = -1.0*std::pow(-cdt,1./3.);
	  else
	    cdt = std::pow(cdt,1./3.);
	}
      cdt = (cdt+beta)/(1.0+beta*cdt);
      return cdt;
    }
  
  //Else, retrieve tables and go through the full thing
  if (!theLorentzTables1)
      theLorentzTables1 = new std::map<G4double,G4PhysicsTable*>;
  if (!theLorentzTables2)
    theLorentzTables2 = new std::map<G4double,G4PhysicsTable*>;

  //Check if tables exist for the given Zmat
  if (!(theLorentzTables1->count(Zmat)))
    PrepareInterpolationTables(Zmat);

  if (!(theLorentzTables1->count(Zmat)) || !(theLorentzTables2->count(Zmat)))
    {
      G4ExceptionDescription ed;
      ed << "Unable to retrieve Lorentz tables for Z= " << Zmat << G4endl;
      G4Exception("G4PenelopeBremsstrahlungAngular::SampleCosTheta()",
		  "em2006",FatalException,ed);
    }
    
  //retrieve actual tables
  G4PhysicsTable* theTable1 = theLorentzTables1->find(Zmat)->second;
  G4PhysicsTable* theTable2 = theLorentzTables2->find(Zmat)->second;
      
  G4double RK=20.0*eGamma/ePrimary;
  G4int ik=std::min((G4int) RK,19);
  
  G4double P10=0,P11=0,P1=0;
  G4double P20=0,P21=0,P2=0;

  //First coefficient
  G4PhysicsFreeVector* v1 = (G4PhysicsFreeVector*) (*theTable1)[ik];
  G4PhysicsFreeVector* v2 = (G4PhysicsFreeVector*) (*theTable1)[ik+1];
  P10 = v1->Value(beta);
  P11 = v2->Value(beta);
  P1=P10+(RK-(G4double) ik)*(P11-P10);
  
  //Second coefficient
  G4PhysicsFreeVector* v3 = (G4PhysicsFreeVector*) (*theTable2)[ik];
  G4PhysicsFreeVector* v4 = (G4PhysicsFreeVector*) (*theTable2)[ik+1];
  P20=v3->Value(beta); 
  P21=v4->Value(beta);
  P2=P20+(RK-(G4double) ik)*(P21-P20);
  
  //Sampling from the Lorenz-trasformed dipole distributions
  P1=std::min(std::exp(P1)/beta,1.0);
  G4double betap = std::min(std::max(beta*(1.0+P2/beta),0.0),0.9999);
  
  G4double testf=0;
  
  if (G4UniformRand() < P1)
    {
      do{
	cdt = 2.0*G4UniformRand()-1.0;
	testf=2.0*G4UniformRand()-(1.0+cdt*cdt);
      }while(testf>0);
    }
  else
    {
      do{
	cdt = 2.0*G4UniformRand()-1.0;
	testf=G4UniformRand()-(1.0-cdt*cdt);
      }while(testf>0);
    }
  cdt = (cdt+betap)/(1.0+betap*cdt);
  return cdt;
}
