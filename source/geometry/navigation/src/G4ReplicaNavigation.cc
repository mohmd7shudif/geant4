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
// $Id: G4ReplicaNavigation.cc,v 1.20 2010-07-13 15:59:42 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
// class G4ReplicaNavigation Implementation
//
// Author: P.Kent, 1996
//
// --------------------------------------------------------------------

#include "G4ReplicaNavigation.hh"

#include "G4AffineTransform.hh"
#include "G4SmartVoxelProxy.hh"
#include "G4SmartVoxelNode.hh"
#include "G4VSolid.hh"
#include "G4GeometryTolerance.hh"

#include <assert.h>

// ********************************************************************
// Constructor
// ********************************************************************
//
G4ReplicaNavigation::G4ReplicaNavigation()
  : fCheck(false), fVerbose(0)
{
  kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();
  kRadTolerance = G4GeometryTolerance::GetInstance()->GetRadialTolerance();
  kAngTolerance = G4GeometryTolerance::GetInstance()->GetAngularTolerance();
}

// ********************************************************************
// Destructor
// ********************************************************************
//
G4ReplicaNavigation::~G4ReplicaNavigation()
{
}

// ********************************************************************
// Inside
// ********************************************************************
//
EInside
G4ReplicaNavigation::Inside(const G4VPhysicalVolume *pVol,
                            const G4int replicaNo,
                            const G4ThreeVector &localPoint) const
{
  EInside in = kOutside;

  // Replication data
  //
  EAxis axis;
  G4int nReplicas;
  G4double width, offset;
  G4bool consuming;
  
  G4double coord, rad2, rmin, tolRMax2, rmax, tolRMin2;

  pVol->GetReplicationData(axis, nReplicas, width, offset, consuming);
  assert(consuming);

  switch (axis)
  {
    case kXAxis:
    case kYAxis:
    case kZAxis:
      coord = std::fabs(localPoint(axis))-width*0.5;
      if ( coord<=-kCarTolerance*0.5 )
      {
        in = kInside;
      }
      else if ( coord<=kCarTolerance*0.5 )
      {
        in = kSurface;
      }
      break;
    case kPhi:
      if ( localPoint.y()||localPoint.x() )
      {
        coord = std::fabs(std::atan2(localPoint.y(),localPoint.x()))-width*0.5;
        if ( coord<=-kAngTolerance*0.5 )
        {
          in = kInside;
        }
        else if ( coord<=kAngTolerance*0.5 )
        {
          in = kSurface;
        }
      }
      else
      {
        in = kSurface;
      }
      break;
    case kRho:
      rad2 = localPoint.perp2();
      rmax = (replicaNo+1)*width+offset;
      tolRMax2  = rmax-kRadTolerance*0.5;
      tolRMax2 *= tolRMax2;
      if ( rad2>tolRMax2 )
      {
        tolRMax2 = rmax+kRadTolerance*0.5;
        tolRMax2 *= tolRMax2;
        if ( rad2<=tolRMax2 )
        {
          in = kSurface;
        }
      }
      else
      {
        // Known to be inside outer radius
        //
        if ( replicaNo||offset )
        {
          rmin = rmax-width;
          tolRMin2 = rmin-kRadTolerance*0.5;
          tolRMin2 *= tolRMin2;
          if ( rad2>tolRMin2 )
          {
            tolRMin2 = rmin+kRadTolerance*0.5;
            tolRMin2 *= tolRMin2;
            if ( rad2>=tolRMin2 )
            {
              in = kInside;
            }
            else
            {
              in = kSurface;
            }
          }
        }
        else
        {
          in = kInside;
        }
      }
      break;
    default:
      G4Exception("G4ReplicaNavigation::Inside()", "GeomNav0002",
                  FatalException, "Unknown axis!");
      break;
  }
  return in;
}

// ********************************************************************
// DistanceToOut
// ********************************************************************
//
G4double
G4ReplicaNavigation::DistanceToOut(const G4VPhysicalVolume *pVol,
                                   const G4int replicaNo,
                                   const G4ThreeVector &localPoint) const
{
  // Replication data
  //
  EAxis axis;
  G4int nReplicas;
  G4double width,offset;
  G4bool consuming;
  
  G4double safety=0.;
  G4double safe1,safe2;
  G4double coord, rho, rmin, rmax;

  pVol->GetReplicationData(axis, nReplicas, width, offset, consuming);
  assert(consuming);
  switch(axis)
  {
    case kXAxis:
    case kYAxis:
    case kZAxis:
       coord = localPoint(axis);
       safe1 = width*0.5-coord;
       safe2 = width*0.5+coord;
       safety = (safe1<=safe2) ? safe1 : safe2;
       break;
    case kPhi:
      if ( localPoint.y()<=0 )
      {
        safety = localPoint.x()*std::sin(width*0.5)
               + localPoint.y()*std::cos(width*0.5);
      }
      else
      {
        safety = localPoint.x()*std::sin(width*0.5)
               - localPoint.y()*std::cos(width*0.5);
      }
      break;
    case kRho:
      rho = localPoint.perp();
      rmax = width*(replicaNo+1)+offset;
      if ( replicaNo||offset )
      {
        rmin  = rmax-width;
        safe1 = rho-rmin;
        safe2 = rmax-rho;
        safety = (safe1<=safe2) ? safe1 : safe2;
      }
      else
      {
        safety = rmax-rho;
      }
      break;
    default:
     G4Exception("G4ReplicaNavigation::DistanceToOut()", "GeomNav0002",
                 FatalException, "Unknown axis!");
     break;
  }
  return (safety >= kCarTolerance) ? safety : 0;
}

// ********************************************************************
// DistanceToOut
// ********************************************************************
//
G4double
G4ReplicaNavigation::DistanceToOut(const G4VPhysicalVolume *pVol,
                                   const G4int replicaNo,
                                   const G4ThreeVector &localPoint,
                                   const G4ThreeVector &localDirection) const
{
  // Replication data
  //
  EAxis axis;
  G4int nReplicas;
  G4double width, offset;
  G4bool consuming;

  G4double Dist=kInfinity;
  G4double coord, Comp, lindist;

  pVol->GetReplicationData(axis, nReplicas, width, offset, consuming);
  assert(consuming);
  switch(axis)
  {
    case kXAxis:
    case kYAxis:
    case kZAxis:
      coord = localPoint(axis);
      Comp = localDirection(axis);
      if ( Comp>0 )
      {
        lindist = width*0.5-coord;
        Dist = (lindist>0) ? lindist/Comp : 0;
      }
      else if ( Comp<0 )
      {
        lindist = width*0.5+coord;
        Dist = (lindist>0) ? -lindist/Comp : 0;
      }
      else
      {
        Dist = kInfinity;
      }
      break;
    case kPhi:
      Dist = DistanceToOutPhi(localPoint, localDirection, width);
      break;
    case kRho:
      Dist=DistanceToOutRad(localPoint,localDirection,width,offset,replicaNo);
      break;
    default:
     G4Exception("G4ReplicaNavigation::DistanceToOut()", "GeomNav0002",
                 FatalException, "Unknown axis!");
     break;
  }
  return Dist;
}

// ********************************************************************
// DistanceToOutPhi
// ********************************************************************
//
G4double
G4ReplicaNavigation::DistanceToOutPhi(const G4ThreeVector &localPoint,
                                      const G4ThreeVector &localDirection,
                                      const G4double width) const
{
  // Phi Intersection
  // NOTE: width<=pi by definition
  //
  G4double sinSPhi, cosSPhi;
  G4double pDistS, pDistE, compS, compE, Dist, dist2, yi;
  if ( localPoint.x()||localPoint.y() )
  {
    sinSPhi = std::sin(-width*0.5);  // SIN of starting phi plane
    cosSPhi = std::cos(width*0.5);   // COS of starting phi plane

    // pDist -ve when inside
    //
    pDistS = localPoint.x()*sinSPhi-localPoint.y()*cosSPhi;
    pDistE = localPoint.x()*sinSPhi+localPoint.y()*cosSPhi;

    // Comp -ve when in direction of outwards normal
    //
    compS = -sinSPhi*localDirection.x()+cosSPhi*localDirection.y();
    compE = -sinSPhi*localDirection.x()-cosSPhi*localDirection.y();

    if ( (pDistS<=0)&&(pDistE<=0) )
    {
      // Inside both phi *full* planes
      //
      if ( compS<0 )
      {
        dist2 = pDistS/compS;
        yi = localPoint.y()+dist2*localDirection.y();

        // Check intersecting with correct half-plane (no -> no intersect)
        //
        if ( yi<=0 )
        {
          Dist = (pDistS<=-kCarTolerance*0.5) ? dist2 : 0;
        }
        else
        {
          Dist = kInfinity;
        }
      }
      else
      {
        Dist = kInfinity;
      }
      if ( compE<0 )
      {
        dist2 = pDistE/compE;
        
        // Only check further if < starting phi intersection
        //
        if ( dist2<Dist )
        {
          yi = localPoint.y()+dist2*localDirection.y();

          // Check intersecting with correct half-plane
          //
          if ( yi>=0 )
          {
            // Leaving via ending phi
            //
            Dist = (pDistE<=-kCarTolerance*0.5) ? dist2 : 0;
          }
        }
      }
    }
    else if ( (pDistS>=0)&&(pDistE>=0) )
    {
      // Outside both *full* phi planes
      // if towards both >=0 then once inside will remain inside
      //
      Dist = ((compS>=0)&&(compE>=0)) ? kInfinity : 0;
    }
    else if ( (pDistS>0)&&(pDistE<0) )
    {
      // Outside full starting plane, inside full ending plane
      //
      if ( compE<0 )
      {      
        dist2 = pDistE/compE;
        yi = localPoint.y()+dist2*localDirection.y();

        // Check intersection in correct half-plane
        // (if not -> remain in extent)
        //
        Dist = (yi>0) ? dist2 : kInfinity;
      }
      else  // Leaving immediately by starting phi
      {
        Dist = kInfinity;
      }
    }
    else
    {
      // Must be (pDistS<0)&&(pDistE>0)
      // Inside full starting plane, outside full ending plane
      //
      if ( compE>=0 )
      {
        if ( compS<0 )
        {
          dist2 = pDistS/compS;
          yi = localPoint.y()+dist2*localDirection.y();

          // Check intersection in correct half-plane
          // (if not -> remain in extent)
          //
          Dist = (yi<0) ? dist2 : kInfinity;
        }
        else
        {
          Dist = kInfinity;
        }
      }
      else
      {
        // Leaving immediately by ending phi
        //
        Dist = 0;
      }
    }
  }
  else
  {
    // On z axis + travel not || to z axis -> use direction vector
    //
    Dist = (std::fabs(localDirection.phi())<=width*0.5) ? kInfinity : 0;
  }
  return Dist;
}

// ********************************************************************
// DistanceToOutRad
// ********************************************************************
//
G4double
G4ReplicaNavigation::DistanceToOutRad(const G4ThreeVector &localPoint,
                                      const G4ThreeVector &localDirection,
                                      const G4double width,
                                      const G4double offset,
                                      const G4int replicaNo) const
{
  G4double rmin, rmax, t1, t2, t3, deltaR;
  G4double b, c, d2, sr;

  //
  // Radial Intersections
  //
  
  // Find intersction with cylinders at rmax/rmin
  // Intersection point (xi,yi,zi) on line
  // x=localPoint.x+t*localDirection.x etc.
  //
  // Intersects with x^2+y^2=R^2
  //
  // Hence (localDirection.x^2+localDirection.y^2)t^2+
  //     2t(localPoint.x*localDirection.x+localPoint.y*localDirection.y)+
  //        localPoint.x^2+localPoint.y^2-R^2=0
  //
  //            t1                t2                    t3

  rmin = replicaNo*width+offset;
  rmax = (replicaNo+1)*width+offset;

  t1 = 1.0-localDirection.z()*localDirection.z();   // since v normalised
  t2 = localPoint.x()*localDirection.x()+localPoint.y()*localDirection.y();
  t3 = localPoint.x()*localPoint.x()+localPoint.y()*localPoint.y();
  
  if ( t1>0 )        // Check not parallel
  {
    // Calculate sr, r exit distance
    //
    if ( t2>=0 )
    {
      // Delta r not negative => leaving via rmax
      //
      deltaR = t3-rmax*rmax;
    
      // NOTE: Should use
      // rho-rmax<-kRadTolerance*0.5 - [no sqrts for efficiency]
      //
      if ( deltaR<-kRadTolerance*0.5 )
      {
        b  = t2/t1;
        c  = deltaR/t1;
        sr = -b+std::sqrt(b*b-c);
      }
      else
      {
        // On tolerant boundary & heading outwards (or locally
        // perpendicular to) outer radial surface -> leaving immediately
        //
        sr = 0;
      }
    }
    else
    {
      // Possible rmin intersection
      //
      if (rmin)
      {
        deltaR = t3-rmin*rmin;
        b  = t2/t1;
        c  = deltaR/t1;
        d2 = b*b-c;
        if ( d2>=0 )
        {
          // Leaving via rmin
          // NOTE: Should use
          // rho-rmin>kRadTolerance*0.5 - [no sqrts for efficiency]
          //
          sr = (deltaR>kRadTolerance*0.5) ? -b-std::sqrt(d2) : 0;
        }
        else
        {
          // No rmin intersect -> must be rmax intersect
          //
          deltaR = t3-rmax*rmax;
          c  = deltaR/t1;
          sr = -b+std::sqrt(b*b-c);
        }
      }
      else
      {
        // No rmin intersect -> must be rmax intersect
        //
        deltaR = t3-rmax*rmax;
        b  = t2/t1;
        c  = deltaR/t1;
        sr = -b+std::sqrt(b*b-c);
      }
    }
  }
  else
  {
    sr=kInfinity;
  }
  return sr;
}

// ********************************************************************
// ComputeTransformation
//
// Setup transformation and transform point into local system
// ********************************************************************
//
void
G4ReplicaNavigation::ComputeTransformation(const G4int replicaNo,
                                                 G4VPhysicalVolume* pVol,
                                                 G4ThreeVector& point) const
{
  G4double val,cosv,sinv,tmpx,tmpy;

  // Replication data
  //
  EAxis axis;
  G4int nReplicas;
  G4double width,offset;
  G4bool consuming;

  pVol->GetReplicationData(axis, nReplicas, width, offset, consuming);
  assert(consuming);

  switch (axis)
  {
    case kXAxis:
      val = -width*0.5*(nReplicas-1)+width*replicaNo;
      pVol->SetTranslation(G4ThreeVector(val,0,0));
      point.setX(point.x()-val);
      break;
    case kYAxis:
      val = -width*0.5*(nReplicas-1)+width*replicaNo;
      pVol->SetTranslation(G4ThreeVector(0,val,0));
      point.setY(point.y()-val);
      break;
    case kZAxis:
      val = -width*0.5*(nReplicas-1)+width*replicaNo;
      pVol->SetTranslation(G4ThreeVector(0,0,val));
      point.setZ(point.z()-val);
      break;
    case kPhi:
      val = -(offset+width*(replicaNo+0.5));
      SetPhiTransformation(val,pVol);
      cosv = std::cos(val);
      sinv = std::sin(val);
      tmpx = point.x()*cosv-point.y()*sinv;
      tmpy = point.x()*sinv+point.y()*cosv;
      point.setY(tmpy);
      point.setX(tmpx);
      break;
    case kRho:
      // No setup required for radial case
    default:
      break;
  }
}

// ********************************************************************
// ComputeTransformation
//
// Setup transformation into local system
// ********************************************************************
//
void
G4ReplicaNavigation::ComputeTransformation(const G4int replicaNo,
                                                 G4VPhysicalVolume* pVol) const
{
  G4double val;

  // Replication data
  //
  EAxis axis;
  G4int nReplicas;
  G4double width, offset;
  G4bool consuming;

  pVol->GetReplicationData(axis, nReplicas, width, offset, consuming);
  assert(consuming);

  switch (axis)
  {
    case kXAxis:
      val = -width*0.5*(nReplicas-1)+width*replicaNo;
      pVol->SetTranslation(G4ThreeVector(val,0,0));
      break;
    case kYAxis:
      val = -width*0.5*(nReplicas-1)+width*replicaNo;
      pVol->SetTranslation(G4ThreeVector(0,val,0));
      break;
    case kZAxis:
      val = -width*0.5*(nReplicas-1)+width*replicaNo;
      pVol->SetTranslation(G4ThreeVector(0,0,val));
      break;
    case kPhi:
      val = -(offset+width*(replicaNo+0.5));
      SetPhiTransformation(val,pVol);
      break;
    case kRho:
      // No setup required for radial case
    default:
      break;
  }
}

// ********************************************************************
// ComputeStep
// ********************************************************************
//
G4double
G4ReplicaNavigation::ComputeStep(const G4ThreeVector &globalPoint,
                                 const G4ThreeVector &globalDirection,
                                 const G4ThreeVector &localPoint,
                                 const G4ThreeVector &localDirection,
                                 const G4double currentProposedStepLength,
                                       G4double &newSafety,
                                       G4NavigationHistory &history,
                                       G4bool &validExitNormal,
                                       G4ThreeVector &exitNormal,
                                       G4bool &exiting,
                                       G4bool &entering,
                                       G4VPhysicalVolume *(*pBlockedPhysical),
                                       G4int &blockedReplicaNo )
{
  G4VPhysicalVolume *repPhysical, *motherPhysical;
  G4VPhysicalVolume *samplePhysical, *blockedExitedVol=0;
  G4LogicalVolume *repLogical;
  G4VSolid *motherSolid;
  G4ThreeVector repPoint, repDirection, sampleDirection;
  G4double ourStep=currentProposedStepLength;
  G4double ourSafety=kInfinity;
  G4double sampleStep, sampleSafety, motherStep, motherSafety;
  G4int localNoDaughters, sampleNo;
  G4int depth;

  // Exiting normal optimisation
  //
  if ( exiting&&validExitNormal )
  {
    if ( localDirection.dot(exitNormal)>=kMinExitingNormalCosine )
    {
      // Block exited daughter volume
      //
      blockedExitedVol = *pBlockedPhysical;
      ourSafety = 0;
    }
  }
  exiting  = false;
  entering = false;

  repPhysical = history.GetTopVolume();
  repLogical  = repPhysical->GetLogicalVolume();

  //
  // Compute intersection with replica boundaries & replica safety
  //

  sampleSafety = DistanceToOut(repPhysical,
                               history.GetTopReplicaNo(),
                               localPoint);

  if ( sampleSafety<ourSafety )
  {
    ourSafety = sampleSafety;
  }
  if ( sampleSafety<ourStep )
  {
    sampleStep = DistanceToOut(repPhysical,
                               history.GetTopReplicaNo(),
                               localPoint,
                               localDirection);
    if ( sampleStep<ourStep )
    {
      ourStep = sampleStep;
      exiting = true;
      validExitNormal = false;
    }
  }

  depth = history.GetDepth()-1;
  while ( history.GetVolumeType(depth)==kReplica )
  {
    repPoint = history.GetTransform(depth).TransformPoint(globalPoint);
    sampleSafety = DistanceToOut(history.GetVolume(depth),
                                 history.GetReplicaNo(depth),
                                 repPoint);
    if ( sampleSafety<ourSafety )
    {
      ourSafety = sampleSafety;
    }
    if ( sampleSafety<ourStep )
    {
      sampleStep = DistanceToOut(history.GetVolume(depth),
                                 history.GetReplicaNo(depth),
                                 repPoint,
                   history.GetTransform(depth).TransformAxis(globalDirection));
      if ( sampleStep<ourStep )
      {
        ourStep = sampleStep;
        exiting = true;
        validExitNormal = false;
      }
    }
    depth--;
  }

  // Compute mother safety & intersection
  //
  repPoint = history.GetTransform(depth).TransformPoint(globalPoint);
  motherPhysical = history.GetVolume(depth);
  motherSolid = motherPhysical->GetLogicalVolume()->GetSolid();
  motherSafety = motherSolid->DistanceToOut(repPoint);
  repDirection = history.GetTransform(depth).TransformAxis(globalDirection);
  motherStep = motherSolid->DistanceToOut(repPoint,repDirection,true,
                                          &validExitNormal,&exitNormal);

  // Push in principle no longer necessary. G4Navigator now takes care of ...
  // Removing this will however generate warnings for pushed particles from
  // G4Navigator, particularly for the case of 3D replicas (Cartesian or
  // combined Radial/Phi cases).
  // Requires further investigation and eventually reimplementation of
  // LevelLocate() to take into account point and direction ...
  //
  if  ( ( !ourStep && (sampleSafety<0.5*kCarTolerance) )
     && ( repLogical->GetSolid()->Inside(localPoint)==kSurface ) )
  {
    ourStep += kCarTolerance;
  }

  if ( motherSafety<ourSafety )
  {
    ourSafety = motherSafety;
  }

#ifdef G4VERBOSE
  if ( fCheck )
  {
    if( motherSolid->Inside(localPoint)==kOutside )
    {
      std::ostringstream message;
      message << "Point outside volume !" << G4endl
              << "          Point " << localPoint
              << " is outside current volume " << motherPhysical->GetName()
              << G4endl;
      G4double estDistToSolid= motherSolid->DistanceToIn(localPoint); 
      message << "          Estimated isotropic distance to solid (distToIn)= " 
              << estDistToSolid;
      if( estDistToSolid > 100.0 * kCarTolerance )
      {
        motherSolid->DumpInfo();
        G4Exception("G4ReplicaNavigation::ComputeStep()",
                    "GeomNav0003", FatalException, message,
                    "Point is far outside Current Volume !" ); 
      }
      else
        G4Exception("G4ReplicaNavigation::ComputeStep()",
                    "GeomNav1002", JustWarning, message,
                    "Point is a little outside Current Volume."); 
    }
  }
#endif

  // May need precision protection
  //
  if ( motherSafety<=ourStep )
  {
    if ( motherStep<=ourStep )
    {
      ourStep = motherStep;
      exiting = true;
      if ( validExitNormal )
      {
        const G4RotationMatrix* rot = motherPhysical->GetRotation();
        if ( rot )
        {
          exitNormal *= rot->inverse();
        }
      }
    }
    else
    {
      validExitNormal = false;
    }
  }
  //
  // Compute daughter safeties & intersections
  //
  localNoDaughters = repLogical->GetNoDaughters();
  for ( sampleNo=localNoDaughters-1; sampleNo>=0; sampleNo-- )
  {
    samplePhysical = repLogical->GetDaughter(sampleNo);
    if ( samplePhysical!=blockedExitedVol )
    {
      G4AffineTransform sampleTf(samplePhysical->GetRotation(),
                                 samplePhysical->GetTranslation());
      sampleTf.Invert();
      const G4ThreeVector samplePoint =
                        sampleTf.TransformPoint(localPoint);
      const G4VSolid* sampleSolid =
                        samplePhysical->GetLogicalVolume()->GetSolid();
      const G4double sampleSafetyDistance =
                        sampleSolid->DistanceToIn(samplePoint);
      if ( sampleSafetyDistance<ourSafety )
      {
        ourSafety = sampleSafetyDistance;
      }
      if ( sampleSafetyDistance<=ourStep )
      {
        sampleDirection = sampleTf.TransformAxis(localDirection);
        const G4double sampleStepDistance =
                        sampleSolid->DistanceToIn(samplePoint,sampleDirection);
        if ( sampleStepDistance<=ourStep )
        {
          ourStep  = sampleStepDistance;
          entering = true;
          exiting  = false;
          *pBlockedPhysical = samplePhysical;
          blockedReplicaNo  = -1;
#ifdef G4VERBOSE
          // Check to see that the resulting point is indeed in/on volume.
          // This check could eventually be made only for successful candidate.

          if ( ( fCheck ) && ( sampleStepDistance < kInfinity ) )
          {
            G4ThreeVector intersectionPoint;
            intersectionPoint= samplePoint
                             + sampleStepDistance * sampleDirection;
            EInside insideIntPt= sampleSolid->Inside(intersectionPoint); 
            if ( insideIntPt != kSurface )
            {
              G4int oldcoutPrec = G4cout.precision(16); 
              std::ostringstream message;
              message << "Navigator gets conflicting response from Solid."
                      << G4endl
                      << "          Inaccurate DistanceToIn for solid "
                      << sampleSolid->GetName() << G4endl
                      << "          Solid gave DistanceToIn = "
                      << sampleStepDistance << " yet returns " ;
              if ( insideIntPt == kInside )
                message << "-kInside-"; 
              else if ( insideIntPt == kOutside )
                message << "-kOutside-";
              else
                message << "-kSurface-"; 
              message << " for this point !" << G4endl
                      << "          Point = " << intersectionPoint << G4endl;
              if ( insideIntPt != kInside )
                message << "        DistanceToIn(p) = " 
                       << sampleSolid->DistanceToIn(intersectionPoint)
                       << G4endl;
              if ( insideIntPt != kOutside ) 
                message << "        DistanceToOut(p) = " 
                       << sampleSolid->DistanceToOut(intersectionPoint);
              G4Exception("G4ReplicaNavigation::ComputeStep()", 
                          "GeomNav1002", JustWarning, message); 
              G4cout.precision(oldcoutPrec);
            }
          }
#endif
        }
      }
    }
  }
  newSafety = ourSafety;
  return ourStep;
}

// ********************************************************************
// ComputeSafety
//
// Compute the isotropic distance to current volume's boundaries
// and to daughter volumes.
// ********************************************************************
//
G4double
G4ReplicaNavigation::ComputeSafety(const G4ThreeVector &globalPoint,
                                   const G4ThreeVector &localPoint,
                                         G4NavigationHistory &history,
                                   const G4double )
{
  G4VPhysicalVolume *repPhysical, *motherPhysical;
  G4VPhysicalVolume *samplePhysical, *blockedExitedVol=0;
  G4LogicalVolume *repLogical;
  G4VSolid *motherSolid;
  G4ThreeVector repPoint;
  G4double ourSafety=kInfinity;
  G4double sampleSafety;
  G4int localNoDaughters, sampleNo;
  G4int depth;

  repPhysical = history.GetTopVolume();
  repLogical  = repPhysical->GetLogicalVolume();

  //
  // Compute intersection with replica boundaries & replica safety
  //

  sampleSafety = DistanceToOut(history.GetTopVolume(),
                               history.GetTopReplicaNo(),
                               localPoint);
  if ( sampleSafety<ourSafety )
  {
    ourSafety = sampleSafety;
  }

  depth = history.GetDepth()-1;
  while ( history.GetVolumeType(depth)==kReplica )
  {
    repPoint = history.GetTransform(depth).TransformPoint(globalPoint);
    sampleSafety = DistanceToOut(history.GetVolume(depth),
                                 history.GetReplicaNo(depth),
                                 repPoint);
    if ( sampleSafety<ourSafety )
    {
      ourSafety = sampleSafety;
    }
    depth--;
  }

  // Compute mother safety & intersection
  //
  repPoint = history.GetTransform(depth).TransformPoint(globalPoint);
  motherPhysical = history.GetVolume(depth);
  motherSolid = motherPhysical->GetLogicalVolume()->GetSolid();
  sampleSafety = motherSolid->DistanceToOut(repPoint);

  if ( sampleSafety<ourSafety )
  {
    ourSafety = sampleSafety;
  }

  // Compute daughter safeties & intersections
  //
  localNoDaughters = repLogical->GetNoDaughters();
  for ( sampleNo=localNoDaughters-1; sampleNo>=0; sampleNo-- )
  {
    samplePhysical = repLogical->GetDaughter(sampleNo);
    if ( samplePhysical!=blockedExitedVol )
    {
      G4AffineTransform sampleTf(samplePhysical->GetRotation(),
                                 samplePhysical->GetTranslation());
      sampleTf.Invert();
      const G4ThreeVector samplePoint =
                            sampleTf.TransformPoint(localPoint);
      const G4VSolid *sampleSolid =
                            samplePhysical->GetLogicalVolume()->GetSolid();
      const G4double sampleSafetyDistance =
                            sampleSolid->DistanceToIn(samplePoint);
      if ( sampleSafetyDistance<ourSafety )
      {
        ourSafety = sampleSafetyDistance;
      }
    }
  }
  return ourSafety;
}

// ********************************************************************
// BackLocate
// ********************************************************************
//
EInside
G4ReplicaNavigation::BackLocate(G4NavigationHistory &history,
                          const G4ThreeVector &globalPoint,
                                G4ThreeVector &localPoint,
                          const G4bool &exiting,
                                G4bool &notKnownInside ) const
{
  G4VPhysicalVolume *pNRMother=0;
  G4VSolid *motherSolid;
  G4ThreeVector repPoint, goodPoint;
  G4int mdepth, depth, cdepth;
  EInside insideCode;

  cdepth = history.GetDepth();
  
  // Find non replicated mother
  //
  for ( mdepth=cdepth-1; mdepth>=0; mdepth-- )
  {
    if ( history.GetVolumeType(mdepth)!=kReplica )
    {
      pNRMother = history.GetVolume(mdepth);
      break;
    }
  }

  if( pNRMother==0 ) 
  {
    // All the tree of mother volumes were Replicas. 
    // This is an error, as the World volume must be a Placement
    //
    G4Exception("G4ReplicaNavigation::BackLocate()", "GeomNav0002",
                FatalException, "The World volume must be a Placement!");
    return kInside;
  }

  motherSolid = pNRMother->GetLogicalVolume()->GetSolid();
  goodPoint = history.GetTransform(mdepth).TransformPoint(globalPoint);
  insideCode = motherSolid->Inside(goodPoint);
  if ( (insideCode==kOutside)||((insideCode==kSurface)&&exiting) )
  {
    // Outside mother -> back up to mother level
    // Locate.. in Navigator will back up one more level
    // localPoint not required
    //
    history.BackLevel(cdepth-mdepth);
    //      localPoint = goodPoint;
  }
  else
  {
    notKnownInside = false;

    // Still within replications
    // Check down: if on outside stop at this level
    //
    for ( depth=mdepth+1; depth<cdepth; depth++)
    {
      repPoint = history.GetTransform(depth).TransformPoint(globalPoint);
      insideCode = Inside(history.GetVolume(depth),
                          history.GetReplicaNo(depth),
                          repPoint);
      if ( (insideCode==kOutside)||((insideCode==kSurface)&&exiting) )
      {
        localPoint = goodPoint;
        history.BackLevel(cdepth-depth);
        return insideCode;
      }
      else
      {
        goodPoint = repPoint;
      }
    }
    localPoint = history.GetTransform(depth).TransformPoint(globalPoint);
    insideCode = Inside(history.GetVolume(depth),
                        history.GetReplicaNo(depth),
                        localPoint);
    // If outside level, set localPoint = coordinates in reference system
    // of *previous* level - location code in navigator will back up one
    // level [And also manage blocking]
    //
    if ( (insideCode==kOutside)||((insideCode==kSurface)&&exiting) )
    {
      localPoint = goodPoint;
    }
  }
  return insideCode;
}
