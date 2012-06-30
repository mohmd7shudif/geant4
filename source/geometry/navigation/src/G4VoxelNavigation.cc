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
// $Id: G4VoxelNavigation.cc,v 1.13 2010-11-04 18:18:00 japost Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
// class G4VoxelNavigation Implementation
//
// Author: P.Kent, 1996
//
// --------------------------------------------------------------------

#include "G4VoxelNavigation.hh"
#include "G4GeometryTolerance.hh"
#include "G4VoxelSafety.hh"

// ********************************************************************
// Constructor
// ********************************************************************
//
G4VoxelNavigation::G4VoxelNavigation()
  : fBList(), fVoxelDepth(-1),
    fVoxelAxisStack(kNavigatorVoxelStackMax,kXAxis),
    fVoxelNoSlicesStack(kNavigatorVoxelStackMax,0),
    fVoxelSliceWidthStack(kNavigatorVoxelStackMax,0.),
    fVoxelNodeNoStack(kNavigatorVoxelStackMax,0),
    fVoxelHeaderStack(kNavigatorVoxelStackMax,(G4SmartVoxelHeader*)0),
    fVoxelNode(0), fpVoxelSafety(0), fCheck(false), fBestSafety(false)
{
  fLogger = new G4NavigationLogger("G4VoxelNavigation");
  fpVoxelSafety = new G4VoxelSafety (); 
}

// ********************************************************************
// Destructor
// ********************************************************************
//
G4VoxelNavigation::~G4VoxelNavigation()
{
  delete fpVoxelSafety;
  delete fLogger;
}

// ********************************************************************
// ComputeStep
// ********************************************************************
//
G4double
G4VoxelNavigation::ComputeStep( const G4ThreeVector& localPoint,
                                const G4ThreeVector& localDirection,
                                const G4double currentProposedStepLength,
                                      G4double& newSafety,
                                      G4NavigationHistory& history,
                                      G4bool& validExitNormal,
                                      G4ThreeVector& exitNormal,
                                      G4bool& exiting,
                                      G4bool& entering,
                                      G4VPhysicalVolume *(*pBlockedPhysical),
                                      G4int& blockedReplicaNo )
{
  G4VPhysicalVolume *motherPhysical, *samplePhysical, *blockedExitedVol=0;
  G4LogicalVolume *motherLogical;
  G4VSolid *motherSolid;
  G4ThreeVector sampleDirection;
  G4double ourStep=currentProposedStepLength, motherSafety, ourSafety;
  G4int localNoDaughters, sampleNo;

  G4bool initialNode, noStep;
  G4SmartVoxelNode *curVoxelNode;
  G4int curNoVolumes, contentNo;
  G4double voxelSafety;

  motherPhysical = history.GetTopVolume();
  motherLogical = motherPhysical->GetLogicalVolume();
  motherSolid = motherLogical->GetSolid();

  //
  // Compute mother safety
  //

  motherSafety = motherSolid->DistanceToOut(localPoint);
  ourSafety = motherSafety;                 // Working isotropic safety
  
#ifdef G4VERBOSE
  if ( fCheck )
  {
    fLogger->PreComputeStepLog (motherPhysical, motherSafety, localPoint);
  }
#endif

  //
  // Compute daughter safeties & intersections
  //

  // Exiting normal optimisation
  //
  if ( exiting && validExitNormal )
  {
    if ( localDirection.dot(exitNormal)>=kMinExitingNormalCosine )
    {
      // Block exited daughter volume
      //
      blockedExitedVol = *pBlockedPhysical;
      ourSafety = 0;
    }
  }
  exiting = false;
  entering = false;

  localNoDaughters = motherLogical->GetNoDaughters();

  fBList.Enlarge(localNoDaughters);
  fBList.Reset();

  initialNode = true;
  noStep = true;

  while (noStep)
  {
    curVoxelNode = fVoxelNode;
    curNoVolumes = curVoxelNode->GetNoContained();
    for (contentNo=curNoVolumes-1; contentNo>=0; contentNo--)
    {
      sampleNo = curVoxelNode->GetVolume(contentNo);
      if ( !fBList.IsBlocked(sampleNo) )
      {
        fBList.BlockVolume(sampleNo);
        samplePhysical = motherLogical->GetDaughter(sampleNo);
        if ( samplePhysical!=blockedExitedVol )
        {
          G4AffineTransform sampleTf(samplePhysical->GetRotation(),
                                     samplePhysical->GetTranslation());
          sampleTf.Invert();
          const G4ThreeVector samplePoint =
                     sampleTf.TransformPoint(localPoint);
          const G4VSolid *sampleSolid     =
                     samplePhysical->GetLogicalVolume()->GetSolid();
          const G4double sampleSafety     =
                     sampleSolid->DistanceToIn(samplePoint);
#ifdef G4VERBOSE
          if( fCheck )
          {
            fLogger->PrintDaughterLog(sampleSolid,samplePoint,sampleSafety,0);
          }
#endif
          if ( sampleSafety<ourSafety )
          {
            ourSafety = sampleSafety;
          }
          if ( sampleSafety<=ourStep )
          {
            sampleDirection = sampleTf.TransformAxis(localDirection);
            G4double sampleStep =
                     sampleSolid->DistanceToIn(samplePoint, sampleDirection);
#ifdef G4VERBOSE
            if( fCheck )
            {
              fLogger->PrintDaughterLog(sampleSolid, samplePoint,
                                        sampleSafety, sampleStep);
            }
#endif
            if ( sampleStep<=ourStep )
            {
              ourStep = sampleStep;
              entering = true;
              exiting = false;
              *pBlockedPhysical = samplePhysical;
              blockedReplicaNo = -1;
#ifdef G4VERBOSE
              // Check to see that the resulting point is indeed in/on volume.
              // This check could eventually be made only for successful
              // candidate.

              if ( fCheck )
              {
                fLogger->AlongComputeStepLog (sampleSolid, samplePoint,
                  sampleDirection, localDirection, sampleSafety, sampleStep);
              }
#endif
            }
          }
        }
      }
    }
    if (initialNode)
    {
      initialNode = false;
      voxelSafety = ComputeVoxelSafety(localPoint);
      if ( voxelSafety<ourSafety )
      {
        ourSafety = voxelSafety;
      }
      if ( currentProposedStepLength<ourSafety )
      {
        // Guaranteed physics limited
        //      
        noStep = false;
        entering = false;
        exiting = false;
        *pBlockedPhysical = 0;
        ourStep = kInfinity;
      }
      else
      {
        //
        // Compute mother intersection if required
        //
        if ( motherSafety<=ourStep )
        {
          G4double motherStep =
              motherSolid->DistanceToOut(localPoint,
                                         localDirection,
                                         true, &validExitNormal, &exitNormal);
#ifdef G4VERBOSE
          if ( fCheck )
          {
            fLogger->PostComputeStepLog(motherSolid, localPoint, localDirection,
                                        motherStep, motherSafety);
          }
#endif
          if ( motherStep<=ourStep )
          {
            ourStep = motherStep;
            exiting = true;
            entering = false;
            if ( validExitNormal )
            {
              const G4RotationMatrix *rot = motherPhysical->GetRotation();
              if (rot)
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
      }
      newSafety = ourSafety;
    }
    if (noStep)
    {
      noStep = LocateNextVoxel(localPoint, localDirection, ourStep);
    }
  }  // end -while (noStep)- loop

  return ourStep;
}

// ********************************************************************
// ComputeVoxelSafety
//
// Computes safety from specified point to voxel boundaries
// using already located point
// o collected boundaries for most derived level
// o adjacent boundaries for previous levels
// ********************************************************************
//
G4double
G4VoxelNavigation::ComputeVoxelSafety(const G4ThreeVector& localPoint) const
{
  G4SmartVoxelHeader *curHeader;
  G4double voxelSafety, curNodeWidth;
  G4double curNodeOffset, minCurCommonDelta, maxCurCommonDelta;
  G4int minCurNodeNoDelta, maxCurNodeNoDelta;
  G4int localVoxelDepth, curNodeNo;
  EAxis curHeaderAxis;

  localVoxelDepth = fVoxelDepth;

  curHeader = fVoxelHeaderStack[localVoxelDepth];
  curHeaderAxis = fVoxelAxisStack[localVoxelDepth];
  curNodeNo = fVoxelNodeNoStack[localVoxelDepth];
  curNodeWidth = fVoxelSliceWidthStack[localVoxelDepth];
  
  // Compute linear intersection distance to boundaries of max/min
  // to collected nodes at current level
  //
  curNodeOffset = curNodeNo*curNodeWidth;
  maxCurNodeNoDelta = fVoxelNode->GetMaxEquivalentSliceNo()-curNodeNo;
  minCurNodeNoDelta = curNodeNo-fVoxelNode->GetMinEquivalentSliceNo();
  minCurCommonDelta = localPoint(curHeaderAxis)
                      - curHeader->GetMinExtent() - curNodeOffset;
  maxCurCommonDelta = curNodeWidth-minCurCommonDelta;

  if ( minCurNodeNoDelta<maxCurNodeNoDelta )
  {
    voxelSafety = minCurNodeNoDelta*curNodeWidth;
    voxelSafety += minCurCommonDelta;
  }
  else if (maxCurNodeNoDelta < minCurNodeNoDelta)
  {
    voxelSafety = maxCurNodeNoDelta*curNodeWidth;
    voxelSafety += maxCurCommonDelta;
  }
  else    // (maxCurNodeNoDelta == minCurNodeNoDelta)
  {
    voxelSafety = minCurNodeNoDelta*curNodeWidth;
    voxelSafety += std::min(minCurCommonDelta,maxCurCommonDelta);
  }

  // Compute isotropic safety to boundaries of previous levels
  // [NOT to collected boundaries]
  //
  while ( (localVoxelDepth>0) && (voxelSafety>0) )
  {
    localVoxelDepth--;
    curHeader = fVoxelHeaderStack[localVoxelDepth];
    curHeaderAxis = fVoxelAxisStack[localVoxelDepth];
    curNodeNo = fVoxelNodeNoStack[localVoxelDepth];
    curNodeWidth = fVoxelSliceWidthStack[localVoxelDepth];
    curNodeOffset = curNodeNo*curNodeWidth;
    minCurCommonDelta = localPoint(curHeaderAxis)
                        - curHeader->GetMinExtent() - curNodeOffset;
    maxCurCommonDelta = curNodeWidth-minCurCommonDelta;
    
    if ( minCurCommonDelta<voxelSafety )
    {
      voxelSafety = minCurCommonDelta;
    }
    if ( maxCurCommonDelta<voxelSafety )
    {
      voxelSafety = maxCurCommonDelta;
    }
  }
  if ( voxelSafety<0 )
  {
    voxelSafety = 0;
  }

  return voxelSafety;
}

// ********************************************************************
// LocateNextVoxel
//
// Finds the next voxel from the current voxel and point
// in the specified direction
//
// Returns false if all voxels considered
//              [current Step ends inside same voxel or leaves all voxels]
//         true  otherwise
//              [the information on the next voxel is put into the set of
//               fVoxel* variables & "stacks"] 
// ********************************************************************
// 
G4bool
G4VoxelNavigation::LocateNextVoxel(const G4ThreeVector& localPoint,
                                   const G4ThreeVector& localDirection,
                                   const G4double currentStep)
{
  G4SmartVoxelHeader *workHeader=0, *newHeader=0;
  G4SmartVoxelProxy *newProxy=0;
  G4SmartVoxelNode *newVoxelNode=0;
  G4ThreeVector targetPoint, voxelPoint;
  G4double workNodeWidth, workMinExtent, workCoord;
  G4double minVal, maxVal, newDistance=0.;
  G4double newHeaderMin, newHeaderNodeWidth;
  G4int depth=0, newDepth=0, workNodeNo=0, newNodeNo=0, newHeaderNoSlices=0;
  EAxis workHeaderAxis, newHeaderAxis;
  G4bool isNewVoxel=false;
  
  G4double currentDistance = currentStep;
  static const G4double sigma = 0.5*G4GeometryTolerance::GetInstance()
                                    ->GetSurfaceTolerance();

  // Determine if end of Step within current voxel
  //
  for (depth=0; depth<fVoxelDepth; depth++)
  {
    targetPoint = localPoint+localDirection*currentDistance;
    newDistance = currentDistance;
    workHeader = fVoxelHeaderStack[depth];
    workHeaderAxis = fVoxelAxisStack[depth];
    workNodeNo = fVoxelNodeNoStack[depth];
    workNodeWidth = fVoxelSliceWidthStack[depth];
    workMinExtent = workHeader->GetMinExtent();
    workCoord = targetPoint(workHeaderAxis);
    minVal = workMinExtent+workNodeNo*workNodeWidth;

    if ( minVal<=workCoord+sigma )
    {
      maxVal = minVal+workNodeWidth;
      if ( maxVal<=workCoord-sigma )
      {
        // Must consider next voxel
        //
        newNodeNo = workNodeNo+1;
        newHeader = workHeader;
        newDistance = (maxVal-localPoint(workHeaderAxis))
                    / localDirection(workHeaderAxis);
        isNewVoxel = true;
        newDepth = depth;
      }
    }
    else
    {
      newNodeNo = workNodeNo-1;
      newHeader = workHeader;
      newDistance = (minVal-localPoint(workHeaderAxis))
                  / localDirection(workHeaderAxis);
      isNewVoxel = true;
      newDepth = depth;
    }
    currentDistance = newDistance;
  }
  targetPoint = localPoint+localDirection*currentDistance;

  // Check if end of Step within collected boundaries of current voxel
  //
  depth = fVoxelDepth;
  {
    workHeader = fVoxelHeaderStack[depth];
    workHeaderAxis = fVoxelAxisStack[depth];
    workNodeNo = fVoxelNodeNoStack[depth];
    workNodeWidth = fVoxelSliceWidthStack[depth];
    workMinExtent = workHeader->GetMinExtent();
    workCoord = targetPoint(workHeaderAxis);
    minVal = workMinExtent+fVoxelNode->GetMinEquivalentSliceNo()*workNodeWidth;

    if ( minVal<=workCoord+sigma )
    {
      maxVal = workMinExtent+(fVoxelNode->GetMaxEquivalentSliceNo()+1)
                            *workNodeWidth;
      if ( maxVal<=workCoord-sigma )
      {
        newNodeNo = fVoxelNode->GetMaxEquivalentSliceNo()+1;
        newHeader = workHeader;
        newDistance = (maxVal-localPoint(workHeaderAxis))
                    / localDirection(workHeaderAxis);
        isNewVoxel = true;
        newDepth = depth;
      }
    }
    else
    {
      newNodeNo = fVoxelNode->GetMinEquivalentSliceNo()-1;
      newHeader = workHeader;
      newDistance = (minVal-localPoint(workHeaderAxis))
                  / localDirection(workHeaderAxis);
      isNewVoxel = true;
      newDepth = depth;
    }
    currentDistance = newDistance;
  }
  if (isNewVoxel)
  {
    // Compute new voxel & adjust voxel stack
    //
    // newNodeNo=Candidate node no at 
    // newDepth =refinement depth of crossed voxel boundary
    // newHeader=Header for crossed voxel
    // newDistance=distance to crossed voxel boundary (along the track)
    //
    if ( (newNodeNo<0) || (newNodeNo>=newHeader->GetNoSlices()))
    {
      // Leaving mother volume
      //
      isNewVoxel = false;
    }
    else
    {
      // Compute intersection point on the least refined
      // voxel boundary that is hit
      //
      voxelPoint = localPoint+localDirection*newDistance;
      fVoxelNodeNoStack[newDepth] = newNodeNo;
      fVoxelDepth = newDepth;
      newVoxelNode = 0;
      while ( !newVoxelNode )
      {
        newProxy = newHeader->GetSlice(newNodeNo);
        if (newProxy->IsNode())
        {
          newVoxelNode = newProxy->GetNode();
        }
        else
        {
          fVoxelDepth++;
          newHeader = newProxy->GetHeader();
          newHeaderAxis = newHeader->GetAxis();
          newHeaderNoSlices = newHeader->GetNoSlices();
          newHeaderMin = newHeader->GetMinExtent();
          newHeaderNodeWidth = (newHeader->GetMaxExtent()-newHeaderMin)
                             / newHeaderNoSlices;
          newNodeNo = G4int( (voxelPoint(newHeaderAxis)-newHeaderMin)
                             / newHeaderNodeWidth );
          // Rounding protection
          //
          if ( newNodeNo<0 )
          {
            newNodeNo=0;
          }
          else if ( newNodeNo>=newHeaderNoSlices )
               {
                 newNodeNo = newHeaderNoSlices-1;
               }
          // Stack info for stepping
          //
          fVoxelAxisStack[fVoxelDepth] = newHeaderAxis;
          fVoxelNoSlicesStack[fVoxelDepth] = newHeaderNoSlices;
          fVoxelSliceWidthStack[fVoxelDepth] = newHeaderNodeWidth;
          fVoxelNodeNoStack[fVoxelDepth] = newNodeNo;
          fVoxelHeaderStack[fVoxelDepth] = newHeader;
        }
      }
      fVoxelNode = newVoxelNode;
    }
  }
  return isNewVoxel;        
}

// ********************************************************************
// ComputeSafety
//
// Calculates the isotropic distance to the nearest boundary from the
// specified point in the local coordinate system. 
// The localpoint utilised must be within the current volume.
// ********************************************************************
//
G4double
G4VoxelNavigation::ComputeSafety(const G4ThreeVector& localPoint,
                                 const G4NavigationHistory& history,
                                 const G4double       maxLength)
{
  G4VPhysicalVolume *motherPhysical, *samplePhysical;
  G4LogicalVolume *motherLogical;
  G4VSolid *motherSolid;
  G4double motherSafety, ourSafety;
  G4int sampleNo;
  G4SmartVoxelNode *curVoxelNode;
  G4int curNoVolumes, contentNo;
  G4double voxelSafety;

  motherPhysical = history.GetTopVolume();
  motherLogical = motherPhysical->GetLogicalVolume();
  motherSolid = motherLogical->GetSolid();

  if( fBestSafety )
  { 
    return fpVoxelSafety->ComputeSafety( localPoint,*motherPhysical,maxLength );
  }

  //
  // Compute mother safety
  //

  motherSafety = motherSolid->DistanceToOut(localPoint);
  ourSafety = motherSafety;                 // Working isotropic safety

#ifdef G4VERBOSE
  if( fCheck )
  {
    fLogger->ComputeSafetyLog (motherSolid, localPoint, motherSafety, true);
  }
#endif
  //
  // Compute daughter safeties
  //
  // Look only inside the current Voxel only (in the first version).
  //
  curVoxelNode = fVoxelNode;
  curNoVolumes = curVoxelNode->GetNoContained();

  for ( contentNo=curNoVolumes-1; contentNo>=0; contentNo-- )
  {
    sampleNo = curVoxelNode->GetVolume(contentNo);
    samplePhysical = motherLogical->GetDaughter(sampleNo);

    G4AffineTransform sampleTf(samplePhysical->GetRotation(),
                               samplePhysical->GetTranslation());
    sampleTf.Invert();
    const G4ThreeVector samplePoint =
                          sampleTf.TransformPoint(localPoint);
    const G4VSolid *sampleSolid     =
                          samplePhysical->GetLogicalVolume()->GetSolid();
    G4double sampleSafety = sampleSolid->DistanceToIn(samplePoint);
    if ( sampleSafety<ourSafety )
    {
      ourSafety = sampleSafety;
    }
#ifdef G4VERBOSE
    if( fCheck )
    {
      fLogger->ComputeSafetyLog (sampleSolid,samplePoint,sampleSafety,false);
    }
#endif
  }
  voxelSafety = ComputeVoxelSafety(localPoint);
  if ( voxelSafety<ourSafety )
  {
    ourSafety = voxelSafety;
  }
  return ourSafety;
}

// ********************************************************************
// SetVerboseLevel
// ********************************************************************
//
void  G4VoxelNavigation::SetVerboseLevel(G4int level)
{
  if( fLogger )       fLogger->SetVerboseLevel(level);
  if( fpVoxelSafety)  fpVoxelSafety->SetVerboseLevel( level ); 
}
