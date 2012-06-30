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
// $Id: G4Surface.cc,v 1.20 2010-09-06 16:02:12 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// ----------------------------------------------------------------------
// GEANT 4 class source file
//
// G4Surface.cc
//
// ----------------------------------------------------------------------

#include "G4Surface.hh"
#include "G4CompositeCurve.hh"
#include "G4GeometryTolerance.hh"

G4Surface::G4Surface()
  : G4STEPEntity(),
    bbox(0), next(0), Intersected(0), Type(0), AdvancedFace(0),
    active(1), distance(kInfinity), uhit(0.), vhit(0.), sameSense(0)
{
  kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();
  kAngTolerance = G4GeometryTolerance::GetInstance()->GetAngularTolerance();
}


G4Surface::~G4Surface()
{
}


G4Surface::G4Surface( const G4Surface& c )
  : G4STEPEntity(),
    bbox(c.bbox), next(c.next), Intersected(c.Intersected), Type(c.Type),
    AdvancedFace(c.AdvancedFace), active(c.active), distance(c.distance),
    uhit(c.uhit), vhit(c.vhit), sameSense(c.sameSense)
{
  kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();
  kAngTolerance = G4GeometryTolerance::GetInstance()->GetAngularTolerance();
}


G4Surface&
G4Surface::operator=( const G4Surface& c )
{
  if (&c == this)  { return *this; }
  bbox = c.bbox;
  next = c.next;
  Intersected = c.Intersected;
  Type = c.Type;
  AdvancedFace = c.AdvancedFace;
  active = c.active;
  distance = c.distance;
  uhit = c.uhit;
  vhit = c.vhit;

  kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();
  kAngTolerance = G4GeometryTolerance::GetInstance()->GetAngularTolerance();

  return *this;
}


G4int G4Surface::operator==( const G4Surface& s )
{
  return origin == s.origin;
}

G4String G4Surface::GetEntityType() const
{
  return G4String("Surface");
}

const char* G4Surface::Name() const
{
  return "G4Surface";
}

G4int G4Surface::MyType() const
{
  return Type;
}

void G4Surface::InitBounded()
{
}

G4double G4Surface::GetUHit() const
{
  return uhit;
}

G4double G4Surface::GetVHit() const
{
  return vhit;
}

//void G4Surface::read_surface(fstream& tmp){;}

G4Point3D G4Surface::Evaluation(const G4Ray&)
{
  return closest_hit;
}

G4int G4Surface::Evaluate(const G4Ray&)
{
  return 0;
}

void G4Surface::Reset()
{
  Intersected = 0;
  active = 1;
  distance = kInfinity;
}

void G4Surface::SetBoundaries(G4CurveVector* boundaries)
{
  surfaceBoundary.Init(*boundaries);
  InitBounded();
}

void G4Surface::CalcBBox()
{
  // Finds the bounds of the surface iow
  // calculates the bounds for a bounding box
  // to the surface. The bounding box is used
  // for a preliminary check of intersection.

  bbox = new G4BoundingBox3D(surfaceBoundary.BBox().GetBoxMin(),
			     surfaceBoundary.BBox().GetBoxMax());
  // old implementation
  //  G4Point3d BoundaryMax = OuterBoundary->GetBoundsMax();
  //  G4Point3d BoundaryMin = OuterBoundary->GetBoundsMin();
  //  bbox = new G4BoundingBox( BoundaryMin, BoundaryMax);    
  //  return;
}

G4Vector3D G4Surface::Normal( const G4Vector3D&  ) const
{  //  return the Normal unit vector to a Surface at the point p on
   //  (or nearly on) the Surface.
   //  The default is not well defined, so return ( 0, 0, 0 ).
   	return G4Vector3D( 0.0, 0.0, 0.0 );
}


G4int G4Surface::Intersect(const G4Ray&)
{
  G4int Result = 0;

  G4Exception("G4Surface::Intersect()", "GeomSolids0001",
              FatalException, "Sorry, not yet implemented.");

#ifdef NEW_IMPLEMENTATION
  // get the intersection
  //    Result = Intersect(rayref);
 
  // Check that the point is within the polyline
  // Get Normal at Hitpoint
  const G4Vector3D& Vec = Normal(closest_hit);
  G4Ray Normal(closest_hit, Vec);

  // Project points & Hit
  //    OuterBoundary->ProjectBoundaryTo2D(Normal.GetPlane(1), 
  //                                       Normal.GetPlane(2), 0);
  


  
  G4Point3D Hit = closest_hit.Project(Normal.GetPlane(1), 
				      Normal.GetPlane(2) );
  // Check point in polygon
  //    Result = OuterBoundary->Inside(Hit, rayref);

#endif 
  return Result;
}


G4double G4Surface::ClosestDistanceToPoint(const G4Point3D& Pt)
{
  // in fact, a squared distance is returned

  // a bit suspicious, this function
  // the distance is almost always an overestimate
  G4double pointDistance= kInfinity;
  G4double tmpDistance;
  const G4CurveVector& bounds= surfaceBoundary.GetBounds();

  G4int entr = bounds.size();

  for (G4int i=0; i<entr; i++) 
  {
    G4Curve* c= bounds[i];

    if (c->GetEntityType() == "G4CompositeCurve") 
    {
      G4CompositeCurve* cc= (G4CompositeCurve*)c;
      const G4CurveVector& segments= cc->GetSegments();
      for (size_t i=0; i<segments.size(); i++) 
      {
	G4Curve* ccc= segments[i];
	tmpDistance= (G4Point3D(Pt.x(), Pt.y(), Pt.z())-ccc->GetEnd()).mag2();
	if (pointDistance > tmpDistance) 
	{
	  pointDistance= tmpDistance;
	}
      }
      
    } 
    else 
    {
      tmpDistance= (G4Point3D(Pt.x(), Pt.y(), Pt.z())-c->GetEnd()).mag2();
      if (pointDistance > tmpDistance) 
      {
	pointDistance= tmpDistance;
      } 
    }
  }

  // L. Broglia
  // Be carreful ! pointdistance is the squared distance
  return std::sqrt(pointDistance);
  
  //  G4double PointDistance=kInfinity;
  //  G4double TmpDistance=0;
  //  PointDistance = OuterBoundary->ClosestDistanceToPoint(Pt);
  //  TmpDistance =0;
  //  for(G4int a=0;a<NumberOfInnerBoundaries;a++)
  //    {
  //      TmpDistance = InnerBoundary[a]->ClosestDistanceToPoint(Pt);
  //      if(PointDistance > TmpDistance) PointDistance = TmpDistance;
  //    }
  //  return PointDistance;

  //G4double G4Boundary::ClosestDistanceToPoint(const G4ThreeVec& Pt)
  //{
  //  G4double PointDistance = kInfinity;
  //  G4double TmpDistance = 0;
  //  for(G4int a =0; a < NumberOfPoints;a++)
  //    {
  //      G4Point3d& Pt2 = GetPoint(a);
  //      TmpDistance = Pt2.Distance(Pt);
  //      if(PointDistance > TmpDistance)PointDistance = TmpDistance;
  //    }
  //  return PointDistance;
  //}
}


std::ostream& operator<<( std::ostream& os, const G4Surface& )
{
  // overwrite output operator << to Print out Surface objects
  // using the PrintOn function defined below
  //	s.PrintOn( os );
  return os;
}


G4double G4Surface::HowNear( const G4Vector3D& x ) const
{
  //  Distance from the point x to a Surface.
  //  The default for a Surface is the distance from the point to the origin.
  G4Vector3D p = G4Vector3D( x - origin );
  return p.mag();
}

void G4Surface::Project()
{
}

void G4Surface::CalcNormal()
{
}  

G4int G4Surface::IsConvex() const
{
  return -1;
}

G4int G4Surface::GetConvex() const
{
  return 0;
}

G4int G4Surface::GetNumberOfPoints() const
{
  return 0;
}

const G4Point3D& G4Surface::GetPoint(G4int) const
{
  const G4Point3D* tmp= new G4Point3D(0,0,0);
  return *tmp;
}

G4Ray* G4Surface::Norm()
{
  return (G4Ray*)0;
}

void G4Surface::Project (G4double& Coord,
                         const G4Point3D& Pt2, 
                         const G4Plane& Pl1)
{
  Coord = Pt2.x()*Pl1.a + Pt2.y()*Pl1.b + Pt2.z()*Pl1.c - Pl1.d;
}

/*
G4double G4Surface::distanceAlongRay( G4int which_way, const G4Ray* ry,
			          G4ThreeVec& p ) const 
{  //  Distance along a Ray (straight line with G4ThreeVec) to leave or enter
   //  a Surface.  The input variable which_way should be set to +1 to indicate
   //  leaving a Surface, -1 to indicate entering a Surface.
   //  p is the point of intersection of the Ray with the Surface.
   //  This is a default function which just gives the distance
   //  between the origin of the Ray and the origin of the Surface.
   //  Since a generic Surface doesn't have a well-defined Normal, no
   //  further checks are Done.
   
        //  This should always be overwritten for derived classes so Print out
        //  a warning message if this is called.
	G4cout << "WARNING from Surface::distanceAlongRay\n"
	     << "    This function should be overwritten by a derived class.\n"
	     << "    Using the Surface base class default.\n";
	p = GetOrigin();
	G4ThreeVec d = ry->Position() - p;
	return d.Magnitude();
}

G4double G4Surface::distanceAlongHelix( G4int which_way, const Helix* hx,
			            G4ThreeVec& p ) const 
{  //  Distance along a Helix to leave or enter a Surface.  
   //  The input variable which_way should be set to +1 to indicate
   //  leaving a Surface, -1 to indicate entering a Surface.
   //  p is the point of intersection of the Helix with the Surface.
   //  This is a default function which just gives the distance
   //  between the origin of the Helix and the origin of the Surface.
   //  Since a generic Surface doesn't have a well-defined Normal, no
   //  further checks are Done.

        //  This should always be overwritten for derived classes so Print out
        //  a warning message if this is called.
	G4cout << "WARNING from Surface::distanceAlongHelix\n"
	     << "    This function should be overwritten by a derived class.\n"
	     << "    Using the Surface base class default.\n";
	p = GetOrigin();
	G4ThreeVec d = hx->position() - p;
	return d.Magnitude();
}


G4ThreeVec G4Surface::Normal() const
{  //  return the Normal unit vector to a Surface
   //  (This is only meaningful for Surfaces for which the Normal does
   //  not depend on location on the Surface). 
   //  The default is not well defined, so return ( 0, 0, 0 ).
   	return G4ThreeVec( 0.0, 0.0, 0.0 );
}


G4ThreeVec G4Surface::Normal( const G4ThreeVec&  ) const
{  //  return the Normal unit vector to a Surface at the point p on
   //  (or nearly on) the Surface.
   //  The default is not well defined, so return ( 0, 0, 0 ).
   	return G4ThreeVec( 0.0, 0.0, 0.0 );
}

G4int G4Surface::Inside( const G4ThreeVec&  ) const
{  //  return 0 if point p is outside Surface, 1 if Inside
   //  default is not well defined, so return 0
   	return 0;
}

void G4Surface::move( const G4ThreeVec& p )
{  //  translate origin of Surface by vector p
	origin += p;	
}

void G4Surface::rotate( G4double alpha, G4double beta,
		      G4double gamma, G4ThreeMat& m, G4int inverse )
{  //  rotate Surface first about global x-axis by angle alpha,
   //                second about global y-axis by angle beta,
   //             and third about global z-axis by angle gamma
   //  by creating and using G4ThreeMat objects
   //  angles are assumed to be given in radians
   //  returns also the overall rotation matrix for use by subclasses
   //  if inverse is non-zero, the order of rotations is reversed
   //  for a generic Surface, only the origin is rotated
//	G4double ax[3][3] = { 0., 0., 0., 0., 0., 0., 0., 0., 0. };
    G4double ax[3][3];
    G4double ay[3][3];
    G4double az[3][3];
//	G4double ay[3][3] = { 0., 0., 0., 0., 0., 0., 0., 0., 0. };
//	G4double az[3][3] = { 0., 0., 0., 0., 0., 0., 0., 0., 0. };
	ax[0][0] = 1.;
	ax[1][1] = std::cos( alpha );
	ax[2][2] = ax[1][1];
	ax[2][1] = std::sin( alpha );
	ax[1][2] = -ax[2][1];
	ay[1][1] = 1.;
	ay[0][0] = std::cos( beta );
	ay[2][2] = ay[0][0];
	ay[0][2] = std::sin( beta );
	ay[2][0] = -ay[0][2];
	az[2][2] = 1.;
	az[0][0] = std::cos( gamma );
	az[1][1] = az[0][0];
	az[1][0] = std::sin( gamma );
	az[0][1] = -az[1][0];
	G4ThreeMat &Rx = *new G4ThreeMat( ax );  // x-rotation matrix
	G4ThreeMat &Ry = *new G4ThreeMat( ay );  // y-rotation matrix
	G4ThreeMat &Rz = *new G4ThreeMat( az );  // z-rotation matrix
	if ( inverse )
		m = Rx * ( Ry * Rz );
	else
		m = Rz * ( Ry * Rx );
	origin = m * origin;
}

void G4Surface::rotate( G4double alpha, G4double beta,
		      G4double gamma, G4int inverse )
{  //  rotate Surface first about global x-axis by angle alpha,
   //                second about global y-axis by angle beta,
   //             and third about global z-axis by angle gamma
   //  by creating and using G4ThreeMat objects
   //  angles are assumed to be given in radians
   //  if inverse is non-zero, the order of rotations is reversed
	G4ThreeMat m;
//  Just call the above function to do this rotation
	rotate( alpha, beta, gamma, m, inverse );
}

*/
