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
// INCL++ intra-nuclear cascade model
// Pekka Kaitaniemi, CEA and Helsinki Institute of Physics
// Davide Mancusi, CEA
// Alain Boudard, CEA
// Sylvie Leray, CEA
// Joseph Cugnon, University of Liege
//
// INCL++ revision: v5.0_rc3
//
#define INCLXX_IN_GEANT4_MODE 1

#include "globals.hh"

/** \file G4INCLRootFinder.hh
 * \brief Static root-finder algorithm.
 *
 * Provides a (nearly) stateless root-finder algorithm.
 *
 * Created on: 2nd March 2011
 *     Author: Davide Mancusi
 */

#ifndef G4INCLROOTFINDER_HH_
#define G4INCLROOTFINDER_HH_

#include <utility>

namespace G4INCL {

  class RootFunctor {
  public:
    virtual G4double operator()(const G4double x) const = 0;
    virtual void cleanUp(const G4bool success) const = 0;
    virtual ~RootFunctor() {};
  protected:
    RootFunctor() {};
  };

  class RootFinder {
  public:
    /** \brief Numerically solve a one-dimensional equation.
     *
     * Numerically solves the equation f(x)==0. This implementation uses the
     * false-position method.
     *
     * If a root is found, it can be retrieved using the getSolution() method,
     *
     * \param f poG4inter to a RootFunctor
     * \param x0 initial value of the function argument
     * \return true if a root was found
     */
    static G4bool solve(RootFunctor const * const f, const G4double x0);

    /** \brief Get the solution of the last call to solve().
     *
     * \return the solution, as an (x,y) pair
     */
    static std::pair<G4double,G4double> const &getSolution() { return RootFinder::solution; }

  private:
    /// \brief The solution obtained in the last call to solve().
    static std::pair<G4double,G4double> solution;

    /** \brief Bracket the root of the function f.
     *
     * Tries to find a bracketing value for the function root.
     *
     * \param f poG4inter to a RootFunctor
     * \param x0 starting value
     * \return if the root could be bracketed, returns two values of x
     *   bracketing the root, as a pair. If the bracketing failed, returns a
     *   pair with first > second.
     */
    static std::pair<G4double,G4double> bracketRoot(RootFunctor const * const f, const G4double x0);

    /// \brief Maximum number of iterations for convergence
    static const G4int maxIterations=50;

    /// \brief Tolerance on the y value
    static const G4double toleranceY;

  protected:
    RootFinder() {};
    ~RootFinder() {};
    
  };
}
#endif /* G4INCLROOTFINDER_HH_ */
