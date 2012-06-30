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
// $Id: G4LEKaonZeroSInelastic.hh,v 1.12 2007-02-26 18:25:37 dennis Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// G4 Gheisha High Energy model class -- header file
// H. Fesefeldt, RWTH Aachen 23-October-1996
// A prototype of the Gheisha High Energy collision model.

#ifndef G4LEKaonZeroSInelastic_h
#define G4LEKaonZeroSInelastic_h 1

#include "G4LEKaonZeroInelastic.hh"
#include "G4LEAntiKaonZeroInelastic.hh"
#include "Randomize.hh"

class G4LEKaonZeroSInelastic : public G4InelasticInteraction  
{
  public: 
    G4LEKaonZeroSInelastic() : G4InelasticInteraction("G4LEKaonZeroSInelastic")
    {
      SetMinEnergy(0.0);
      SetMaxEnergy(25.*GeV);
    }

    virtual ~G4LEKaonZeroSInelastic(){ }

    G4HadFinalState* ApplyYourself(const G4HadProjectile& aTrack, G4Nucleus& targetNucleus)
    {
      if(G4UniformRand() < 0.50)
      {
         return theKaonZeroInelastic.ApplyYourself(aTrack, targetNucleus);
      }
      else
      {
         return theAntiKaonZeroInelastic.ApplyYourself(aTrack, targetNucleus);
      }
    }

    virtual void ModelDescription(std::ostream& outFile) const
    {
      outFile << "G4LEKaonZeroSInelastic is one of the Low Energy\n"
              << "Parameterized (LEP) models used to implement K0S\n"
              << "scattering from nuclei.  Upon interaction with a nucleus\n"
              << "the K0S is treated as a K0 50% of the time and an antiK0\n"
              << "50% of the time.  Then the K0 or antiK0 interacts with the\n"
              << "nucleus using the re-engineered GHEISHA code of\n"
              << "H. Fesefeldt, which divides the initial collision products\n"
              << "into backward- and forward-going clusters which are then\n"
              << "decayed into final state hadrons.  The model does not\n"
              << "conserve energy or charge on an event-by-event basis.  It\n"
              << "may be applied to K0S with initial energies between 0 and\n"
              << "25 GeV.\n";
    }

  private:
    G4LEKaonZeroInelastic theKaonZeroInelastic;
    G4LEAntiKaonZeroInelastic theAntiKaonZeroInelastic;	

};
#endif                     
                                         

