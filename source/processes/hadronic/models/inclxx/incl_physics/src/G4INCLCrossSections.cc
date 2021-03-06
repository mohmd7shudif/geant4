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

#include "G4INCLCrossSections.hh"
#include "G4INCLKinematicsUtils.hh"
#include "G4INCLParticleTable.hh"
#include "G4INCLLogger.hh"
//#include <cassert>

namespace G4INCL {

  G4double CrossSections::total(Particle const * const p1, Particle const * const p2) {
    G4double inelastic = 0.0;
    if(p1->isNucleon() && p2->isNucleon()) {
      inelastic = CrossSections::deltaProduction(p1, p2);
    } else if((p1->isNucleon() && p2->isDelta()) ||
	      (p1->isDelta() && p2->isNucleon())) {
      inelastic = CrossSections::recombination(p1, p2);
    } else if((p1->isNucleon() && p2->isPion()) ||
        (p1->isPion() && p2->isNucleon())) {
      inelastic = CrossSections::pionNucleon(p1, p2);
    } else {
      inelastic = 0.0;
    }

    return inelastic + CrossSections::elastic(p1, p2);
  }

  G4double CrossSections::pionNucleon(Particle const * const particle1, Particle const * const particle2) {
    //      FUNCTION SPN(X,IND2T3,IPIT3,f17)
    // SIGMA(PI+ + P) IN THE (3,3) REGION
    // NEW FIT BY J.VANDERMEULEN  + FIT BY Th AOUST ABOVE (3,3) RES
    //                              CONST AT LOW AND VERY HIGH ENERGY
    //      COMMON/BL8/RATHR,RAMASS                                           REL21800
    //      G4integer f17
    // RATHR and RAMASS are always 0.0!!!

    G4double x = KinematicsUtils::totalEnergyInCM(particle1, particle2);
    if(x>10000.) return 0.0; // no cross section above this value

    G4int ipit3 = 0;
    G4int ind2t3 = 0;
    G4double ramass = 0.0;

    if(particle1->isPion()) {
      ipit3 = ParticleTable::getIsospin(particle1->getType());
    } else if(particle2->isPion()) {
      ipit3 = ParticleTable::getIsospin(particle2->getType());
    }

    if(particle1->isNucleon()) {
      ind2t3 = ParticleTable::getIsospin(particle1->getType());
    } else if(particle2->isNucleon()) {
      ind2t3 = ParticleTable::getIsospin(particle2->getType());
    }

    G4double y=x*x;
    G4double q2=(y-1076.0*1076.0)*(y-800.0*800.0)/y/4.0;
    if (q2 <= 0.) {
      return 0.0;
    }
    G4double q3 = std::pow(std::sqrt(q2),3);
    G4double f3 = q3/(q3 + 5832000.); // 5832000 = 180^3
    G4double spnResult = 326.5/(std::pow((x-1215.0-ramass)*2.0/(110.0-ramass), 2)+1.0);
    spnResult = spnResult*(1.0-5.0*ramass/1215.0);
    G4double cg = 4.0 + G4double(ind2t3)*G4double(ipit3);
    spnResult = spnResult*f3*cg/6.0;

    if(x < 1200.0  && spnResult < 5.0) {
      spnResult = 5.0;
    }

    // HE pi+ p and pi- n
    if(x > 1290.0) {
      if((ind2t3 == 1 && ipit3 == 2) || (ind2t3 == -1 && ipit3 == -2))
	spnResult=CrossSections::spnPiPlusPHE(x);
      else if((ind2t3 == 1 && ipit3 == -2) || (ind2t3 == -1 && ipit3 == 2))
	spnResult=CrossSections::spnPiMinusPHE(x);
      else if(ipit3 == 0) spnResult = (CrossSections::spnPiPlusPHE(x) + CrossSections::spnPiMinusPHE(x))/2.0; // (spnpipphe(x)+spnpimphe(x))/2.0
      else {
        ERROR("Unknown configuration!" << std::endl);
      }
    }

    return spnResult;
  }

  G4double CrossSections::spnPiPlusPHE(const G4double x) {
    // HE and LE pi- p and pi+ n
    if(x <= 1750.0) {
      return -2.33730e-06*std::pow(x, 3)+1.13819e-02*std::pow(x,2)
	-1.83993e+01*x+9893.4;
    } else if(x > 1750.0 && x <= 2175.0) {
      return 1.13531e-06*std::pow(x, 3)-6.91694e-03*std::pow(x, 2)
	+1.39907e+01*x-9360.76;
    } else {
      return -3.18087*std::log(x)+52.9784;
    }
  }

  G4double CrossSections::spnPiMinusPHE(const G4double x) {
    // HE pi- p and pi+ n
    if(x <= 1475.0) {
      return 0.00120683*(x-1372.52)*(x-1372.52)+26.2058;
    } else if(x > 1475.0  && x <= 1565.0) {
      return 1.15873e-05*x*x+49965.6/((x-1519.59)*(x-1519.59)+2372.55);
    } else if(x > 1565.0 && x <= 2400.0) {
      return 34.0248+43262.2/((x-1681.65)*(x-1681.65)+1689.35);
    } else if(x > 2400.0 && x <= 7500.0) {
      return 3.3e-7*(x-7500.0)*(x-7500.0)+24.5;
    } else {
      return 24.5;
    }
  }

  G4double CrossSections::recombination(Particle const * const p1, Particle const * const p2) {
    const G4int isospin = ParticleTable::getIsospin(p1->getType()) + ParticleTable::getIsospin(p2->getType());
    if(isospin==4 || isospin==-4) return 0.0;

    G4double s = KinematicsUtils::squareTotalEnergyInCM(p1, p2);
    G4double Ecm = std::sqrt(s);
    G4int deltaIsospin;
    G4double deltaMass;
    if(p1->isDelta()) {
      deltaIsospin = ParticleTable::getIsospin(p1->getType());
      deltaMass = p1->getMass();
    } else {
      deltaIsospin = ParticleTable::getIsospin(p2->getType());
      deltaMass = p2->getMass();
    }

    if(Ecm <= 938.3 + deltaMass) {
      return 0.0;
    }

    if(Ecm < 938.3 + deltaMass + 2.0) {
      Ecm = 938.3 + deltaMass + 2.0;
      s = Ecm*Ecm;
    }
    
    const G4double x = (s - 4.*ParticleTable::effectiveNucleonMass2) /
      (s - std::pow(ParticleTable::effectiveNucleonMass + deltaMass, 2));
    const G4double y = s/(s - std::pow(deltaMass - ParticleTable::effectiveNucleonMass, 2));
    /* Concerning the way we calculate the lab momentum, see the considerations
     * in CrossSections::elasticNNLegacy().
     */
    const G4double pLab = KinematicsUtils::momentumInLab(s, ParticleTable::effectiveNucleonMass, ParticleTable::effectiveNucleonMass);
    G4double result = 0.5 * x * y * deltaProduction(isospin, pLab);
    result *= 3.*(32.0 + isospin * isospin * (deltaIsospin * deltaIsospin - 5))/64.0;
    result /= 1.0 + 0.25 * isospin * isospin;
    return result;
  }

  G4double CrossSections::deltaProduction(Particle const * const p1, Particle const * const p2) {
    // assert(p1->isNucleon() && p2->isNucleon());
    const G4double sqrts = KinematicsUtils::totalEnergyInCM(p1,p2);
    if(sqrts < ParticleTable::effectivePionMass + 2*ParticleTable::effectiveNucleonMass + 50.) { // approximately yields INCL4.6's hard-coded threshold in collis, 2065 MeV
      return 0.0;
    } else {
      const G4double pLab = KinematicsUtils::momentumInLab(p1,p2);
      const G4int isospin = ParticleTable::getIsospin(p1->getType()) + ParticleTable::getIsospin(p2->getType());
      return deltaProduction(isospin, pLab);
    }
  }

  G4double CrossSections::deltaProduction(const G4int isospin, const G4double pLab) {
    G4double xs = 0.0;
    // assert(isospin==-2 || isospin==0 || isospin==2);

    const G4double momentumGeV = 0.001 * pLab;
    if(pLab < 800.0) {
      return 0.0;
    }

    if(isospin==2 || isospin==-2) { // pp, nn
      if(pLab >= 2000.0) {
        xs = (41.0 + (60.0*momentumGeV - 54.0)*std::exp(-1.2*momentumGeV) - 77.0/(momentumGeV + 1.5));
      } else if(pLab >= 1500.0 && pLab < 2000.0) {
        xs = (41.0 + 60.0*(momentumGeV - 0.9)*std::exp(-1.2*momentumGeV) - 1250.0/(momentumGeV+50.0)+ 4.0*std::pow(momentumGeV - 1.3, 2));
      } else if(pLab < 1500.0) {
        xs = (23.5 + 24.6/(1.0 + std::exp(-10.0*momentumGeV + 12.0))
            -1250.0/(momentumGeV +50.0)+4.0*std::pow(momentumGeV - 1.3,2));
      }
    } else if(isospin==0) { // pn
      if(pLab >= 2000.0) {
        xs = (42.0 - 77.0/(momentumGeV + 1.5));
      } else if(pLab >= 1000.0 && pLab < 2000.0) {
        xs = (24.2 + 8.9*momentumGeV - 31.1/std::sqrt(momentumGeV));
      } else if(pLab < 1000.0) {
        xs = (33.0 + 196.0*std::sqrt(std::pow(std::abs(momentumGeV - 0.95),5))
            -31.1/std::sqrt(momentumGeV));
      }
    }

    if(xs < 0.0) return 0.0;
    else return xs;
  }

  G4double CrossSections::elasticNNHighEnergy(const G4double momentum) {
    return 77.0/(momentum + 1.5);
  }

  G4double CrossSections::elasticProtonNeutron(const G4double momentum) {
    if(momentum < 0.450) {
      const G4double alp = std::log(momentum);
      return 6.3555*std::exp(-3.2481*alp-0.377*alp*alp);
    } else if(momentum >= 0.450 && momentum < 0.8) {
      return (33.0 + 196.0 * std::sqrt(std::pow(std::abs(momentum - 0.95), 5)));
    } else if(momentum > 2.0) {
      return CrossSections::elasticNNHighEnergy(momentum);
    } else {
      return 31.0/std::sqrt(momentum);
    }
  }

  G4double CrossSections::elasticProtonProtonOrNeutronNeutron(const G4double momentum)
  {
    if(momentum < 0.440) {
      return 34.0*std::pow(momentum/0.4, -2.104);
    } else if(momentum < 0.8 && momentum >= 0.440) {
      return (23.5 + 1000.0*std::pow(momentum-0.7, 4));
    } else if(momentum < 2.0) {
      return (1250.0/(50.0 + momentum) - 4.0*std::pow(momentum-1.3, 2));
    } else {
      return CrossSections::elasticNNHighEnergy(momentum);
    }
  }

  G4double CrossSections::elasticNN(Particle const * const p1, Particle const * const p2) {
    G4double momentum = 0.0;
    momentum = 0.001 * KinematicsUtils::momentumInLab(p1, p2);
    if((p1->getType() == Proton && p2->getType() == Proton) ||
       (p1->getType() == Neutron && p2->getType() == Neutron)) {
      return CrossSections::elasticProtonProtonOrNeutronNeutron(momentum);
    } else if((p1->getType() == Proton && p2->getType() == Neutron) ||
	      (p1->getType() == Neutron && p2->getType() == Proton)) {
      return CrossSections::elasticProtonNeutron(momentum);
    } else {
      ERROR("G4INCL::CrossSections::elasticNN: Bad input!" << std::endl
        << p1->prG4int() << p2->prG4int() << std::endl);
    }
    return 0.0;
  }

  G4double CrossSections::elasticNNLegacy(Particle const * const part1, Particle const * const part2) {
    G4double scale = 1.0;


    G4int i = ParticleTable::getIsospin(part1->getType())
      + ParticleTable::getIsospin(part2->getType());
    G4double sel = 0.0;

    /* The NN cross section is parametrised as a function of the lab momentum
     * of one of the nucleons. For NDelta or DeltaDelta, the physical
     * assumption is that the cross section is the same as NN *for the same
     * total CM energy*. Thus, we calculate s from the particles involved, and
     * we convert this value to the lab momentum of a nucleon *as if this were
     * an NN collision*.
     */
    const G4double s = KinematicsUtils::squareTotalEnergyInCM(part1, part2);
    G4double plab = KinematicsUtils::momentumInLab(s, ParticleTable::effectiveNucleonMass, ParticleTable::effectiveNucleonMass);

    G4double p1=0.001*plab;
    if(plab > 2000.) goto sel13;
    if(part1->isNucleon() && part2->isNucleon())
      goto sel1;
    else
      goto sel3;
  sel1: if (i == 0) goto sel2;
  sel3: if (plab < 800.) goto sel4;
    if (plab > 2000.) goto sel13;
    sel=(1250./(50.+p1)-4.*std::pow(p1-1.3, 2))*scale;
    goto sel100;
    return sel;
  sel4: if (plab < 440.) {
      sel=34.*std::pow(p1/0.4, (-2.104))*scale;
    } else {
      sel=(23.5+1000.*std::pow(p1-0.7, 4))*scale;
    }
    goto sel100;
    return sel;
  sel13: sel=77./(p1+1.5)*scale;
    goto sel100;
    return sel;
  sel2: if (plab < 800.) goto sel11;
    if (plab > 2000.) goto sel13;
    sel=31./std::sqrt(p1)*scale;
    goto sel100;
    return sel;
  sel11: if (plab < 450.) {
      G4double alp=std::log(p1);
      sel=6.3555*std::exp(-3.2481*alp-0.377*alp*alp)*scale;
    } else {
      sel=(33.+196.*std::sqrt(std::pow(std::abs(p1-0.95),5)))*scale;
    }

  sel100: return sel;
  }

  G4double CrossSections::elastic(Particle const * const p1, Particle const * const p2) {
    if(!p1->isPion() && !p2->isPion())
      //    return elasticNN(p1, p2); // New implementation
      return elasticNNLegacy(p1, p2); // Translated from INCL4.6 FORTRAN
    else
      return 0.0; // No pion-nucleon elastic scattering
  }

  G4double CrossSections::calculateNNDiffCrossSection(G4double pl, G4int iso) {
    G4double x = 0.001 * pl; // Change to GeV
    if(iso != 0) {
      if(pl <= 2000.0) {
        x = std::pow(x, 8);
	return 5.5e-6 * x/(7.7 + x);
      } else {
	return (5.34 + 0.67*(x - 2.0)) * 1.0e-6;
      }
    } else {
      if(pl < 800.0) {
	G4double b = (7.16 - 1.63*x) * 1.0e-6;
	return b/(1.0 + std::exp(-(x - 0.45)/0.05));
      } else if(pl < 1100.0) {
	return (9.87 - 4.88 * x) * 1.0e-6;
      } else {
	return (3.68 + 0.76*x) * 1.0e-6;
      }
    }
    return 0.0; // Should never reach this poG4int
  }
}

