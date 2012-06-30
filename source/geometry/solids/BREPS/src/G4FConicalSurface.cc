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
// $Id: G4FConicalSurface.cc,v 1.19 2006-06-29 18:42:12 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// ----------------------------------------------------------------------
// GEANT 4 class source file
//
// G4FConicalSurface.cc
//
// ----------------------------------------------------------------------

#include "G4FConicalSurface.hh"
#include "G4Sort.hh"
#include "G4CircularCurve.hh"


G4FConicalSurface::G4FConicalSurface()
{
  length       = 1.0;
  small_radius = 0.0;
  large_radius = 1.0;
  tan_angle = (large_radius-small_radius)/length;
}

G4FConicalSurface::~G4FConicalSurface()
{
}

G4FConicalSurface::G4FConicalSurface(const G4Point3D&  o, 
				     const G4Vector3D& a,
				     G4double          l, 
				     G4double          sr, 
				     G4double          lr 
				    ) 
{
  // Make a G4FConicalSurface with origin o, axis a, length l, small radius 
  // sr, and large radius lr. The angle is calculated below and the SetAngle
  // function of G4ConicalSurface is used to set it properly from the default
  // value used above in the initialization.
 
  // Create the position with origin o, axis a, and a direction

  G4Vector3D dir(1,1,1);
  Position.Init(dir, a, o);
  origin = o;
  
  //  Require length to be nonnegative
  if (l >=0)
    length = l;
  else 
  {
    std::ostringstream message;
    message << "Negative length." << G4endl
	    << "Default length of 0.0 is used.";    
    G4Exception("G4FConicalSurface::G4FConicalSurface()",
                "GeomSolids1001", JustWarning, message);

    length = 0.0;
  }
  
  //  Require small radius to be non-negative (i.e., allow zero)
  if ( sr >= 0.0 )
    small_radius = sr;
  else 
  {
    std::ostringstream message;
    message << "Negative small radius." << G4endl
	    << "Default value of 0.0 is used.";    
    G4Exception("G4FConicalSurface::G4FConicalSurface()",
                "GeomSolids1001", JustWarning, message);
   
    small_radius = 0.0;
  }

  //  Require large radius to exceed small radius
  if ( lr > small_radius )
    large_radius = lr;
  else 
  {
    std::ostringstream message;
    message << "Large radius must exceed small radius" << G4endl
	    << "Default value of small radius +1 is used.";    
    G4Exception("G4FConicalSurface::G4FConicalSurface()",
                "GeomSolids1001", JustWarning, message);

    large_radius = small_radius + 1.0;
  }

  //  Calculate the angle of the G4ConicalSurface from the length and radii
  tan_angle =  ( large_radius - small_radius ) / length ;
}


const char* G4FConicalSurface::Name() const
{
  return "G4FConicalSurface";
}

// Modified by L. Broglia (01/12/98)
void G4FConicalSurface::CalcBBox()
{
  G4Point3D Max   = G4Point3D(-PINFINITY);
  G4Point3D Min   = G4Point3D( PINFINITY);
  G4Point3D Tmp;

  G4Point3D Origin    = Position.GetLocation();
  G4Point3D EndOrigin = G4Point3D( Origin + (length * Position.GetAxis()) );
  
  G4double radius = large_radius;
  G4Point3D Radius(radius, radius, 0);

  // Default BBox
  G4Point3D Tolerance(kCarTolerance, kCarTolerance, kCarTolerance);
  G4Point3D BoxMin(Origin-Tolerance);
  G4Point3D BoxMax(Origin+Tolerance);

  bbox = new G4BoundingBox3D();
  bbox->Init(BoxMin, BoxMax);

  Tmp = (Origin - Radius);
  bbox->Extend(Tmp);
  
  Tmp = Origin + Radius;
  bbox->Extend(Tmp);

  Tmp = EndOrigin - Radius;
  bbox->Extend(Tmp);
  
  Tmp = EndOrigin + Radius;
  bbox->Extend(Tmp);
}


void G4FConicalSurface::PrintOn( std::ostream& os ) const
{ 
  //  printing function using C++ std::ostream class
  os << "G4FConicalSurface with origin: " << origin << "\t"
     << "and axis: " << Position.GetAxis() << "\n"
     << "\t small radius: " << small_radius 
     << "\t large radius: " << large_radius
     << "\t and length: " << length << "\n";
}


G4int G4FConicalSurface::operator==( const G4FConicalSurface& c ) const
{
  return ( origin             == c.origin                &&
	   Position.GetAxis() == c.Position.GetAxis()    &&
	   small_radius       == c.small_radius          && 
	   large_radius       == c.large_radius          && 
	   length      	      == c.length                &&
	   tan_angle          == c.tan_angle                );
}


G4int G4FConicalSurface::WithinBoundary( const G4Vector3D& x ) const
{ 
  //  return 1 if point x is within the boundaries of the G4FConicalSurface
  //  return 0 otherwise (assume it is on the G4ConicalSurface)
  G4Vector3D q = G4Vector3D( x - origin );
  
  G4double qmag = q.mag();
  G4double s    = std::sin( std::atan2(large_radius-small_radius, length) );
  G4double ls   = small_radius / s;
  G4double ll   = large_radius / s;
  
  if ( ( qmag >= ls )  &&  ( qmag <= ll ) )
    return 1;
  else
    return 0;
}


G4double G4FConicalSurface::Scale() const
{  
  //  Returns the small radius of a G4FConicalSurface unless it is zero, in 
  //  which case returns the large radius.
  //  Used for Scale-invariant tests of surface thickness.
  if ( small_radius == 0.0 )
    return large_radius;
  else
    return small_radius;
}


G4double G4FConicalSurface::Area() const
{ 
 //  Returns the Area of a G4FConicalSurface
  G4double rdif = large_radius - small_radius; 
  
  return ( pi * ( small_radius + large_radius ) * 
	   std::sqrt( length * length  +  rdif * rdif ) );
}


void G4FConicalSurface::resize( G4double l, G4double sr, G4double lr )
{
  //  Resize a G4FConicalSurface to a new length l, and new radii sr and lr.
  //  Must Reset angle of the G4ConicalSurface as well based on these new 
  //  values.
  //  Require length to be non-negative
  
  //	if ( l > 0.0 )
  if ( l >= 0.0 )
    length = l;
  else 
  {
    std::ostringstream message;
    message << "Negative length." << G4endl
	    << "Original value of " << length << " is retained.";    
    G4Exception("G4FConicalSurface::resize()",
                "GeomSolids1001", JustWarning, message);
  }

  //  Require small radius to be non-negative (i.e., allow zero)
  if ( sr >= 0.0 )
    small_radius = sr;
  else 
  {
    std::ostringstream message;
    message << "Negative small radius." << G4endl
	    << "Original value of " << small_radius << " is retained.";    
    G4Exception("G4FConicalSurface::resize()",
                "GeomSolids1001", JustWarning, message);
  }

  //  Require large radius to exceed small radius
  if ( lr > small_radius )
    large_radius = lr;
  else 
  {
    G4double r = small_radius + 1.0;
    lr = ( large_radius <= small_radius ) ? r : large_radius;
    large_radius = lr;
    
    std::ostringstream message;
    message << "Large radius must exceed small radius." << G4endl
	    << "Default value of " << large_radius << " is used.";    
    G4Exception("G4FConicalSurface::resize()",
                "GeomSolids1001", JustWarning, message);
  }

  //  Calculate the angle of the G4ConicalSurface from the length and radii
  tan_angle =  ( large_radius - small_radius ) / length ;
 
}


G4int G4FConicalSurface::Intersect(const G4Ray& ry )
{ 
  // This function count the number of intersections of a 
  // bounded conical surface by a ray.
  // At first, calculates the intersections with the semi-infinite 
  // conical surfsace. After, count the intersections within the
  // finite conical surface boundaries, and set "distance" to the 
  // closest distance from the start point to the nearest intersection
  // If the point is on the surface it returns or the intersection with
  // the opposite surface or kInfinity
  // If no intersection is founded, set distance = kInfinity and
  // return 0

  distance    = kInfinity;
  closest_hit = PINFINITY;

  // origin and direction of the ray
  G4Point3D  x    = ry.GetStart();
  G4Vector3D dhat = ry.GetDir();

  // cone angle and axis
  G4double   ta   = tan_angle;
  G4Vector3D ahat = Position.GetAxis();
 
  //  array of solutions in distance along the ray
  G4double s[2];
  s[0]=-1.0;
  s[1]=-1.0;

  // calculate the two intersections (quadratic equation)   
  G4Vector3D gamma =  G4Vector3D( x - Position.GetLocation() );
  
  G4double t  = 1  +  ta * ta;
  G4double ga = gamma * ahat;
  G4double da = dhat * ahat;

  G4double A = t * da * da - dhat * dhat;
  G4double B = 2 * ( -gamma * dhat + t * ga * da - large_radius * ta * da);
  G4double C = ( -gamma * gamma + t * ga * ga 
		 - 2 * large_radius * ta * ga
		 + large_radius * large_radius );

  G4double radical = B * B  -  4.0 * A * C; 

  if ( radical < 0.0 ) 
    // no intersection
    return 0;
  else 
  {
    G4double root = std::sqrt( radical );
    s[0] = ( - B + root ) / ( 2. * A );
    s[1] = ( - B - root ) / ( 2. * A );
  }
  
  // validity of the solutions
  // the hit point must be into the bounding box of the conical surface
  G4Point3D p0 = G4Point3D( x + s[0]*dhat );
  G4Point3D p1 = G4Point3D( x + s[1]*dhat );
  
  if( !GetBBox()->Inside(p0) )
    s[0] = kInfinity;

  if( !GetBBox()->Inside(p1) )
    s[1] = kInfinity;
 
  // now loop over each positive solution, keeping the first one (smallest
  // distance along the ray) which is within the boundary of the sub-shape
  G4int nbinter = 0;
  distance = kInfinity;

  for ( G4int i = 0; i < 2; i++ ) 
  {  
    if(s[i] < kInfinity) {
      if ( (s[i] > kCarTolerance*0.5)  ) {
	nbinter++;
  	if ( distance > (s[i]*s[i]) ) {
	  distance = s[i]*s[i];
	}
      }
    }
  }

  return nbinter;
}


G4double G4FConicalSurface::HowNear( const G4Vector3D& x ) const
{ 
//  Shortest distance from the point x to the G4FConicalSurface.
//  The distance will be always positive
//  This function works only with Cone axis equal (0,0,1) or (0,0,-1), it project
//  the surface and the point on the x,z plane and compute the distance in analytical
//  way
  
  G4double   hownear ;

  G4Vector3D upcorner = G4Vector3D ( small_radius, 0 , origin.z()+Position.GetAxis().z()*length);
  G4Vector3D downcorner = G4Vector3D ( large_radius, 0 , origin.z());
  G4Vector3D xd;  
  
  xd = G4Vector3D ( std::sqrt ( x.x()*x.x() + x.y()*x.y() ) , 0 , x.z() );
    
  G4double m = (upcorner.z() - downcorner.z()) / (upcorner.x() - downcorner.x());
  G4double q = (downcorner.z()*upcorner.x() - upcorner.z()*downcorner.x()) /
               (upcorner.x() - downcorner.x());
  
  G4double Zinter = (xd.z()*m*m + xd.x()*m +q)/(1+m*m) ;
  
  if ( ((Zinter >= downcorner.z()) && (Zinter <=upcorner.z())) ||
       ((Zinter >= upcorner.z()) && (Zinter <=downcorner.z())) ) {
    hownear = std::fabs(m*xd.x()-xd.z()+q)/std::sqrt(1+m*m);
    return hownear;
  } else {
    hownear = std::min ( (xd-upcorner).mag() , (xd-downcorner).mag() );
    return hownear;
  }


}


G4Vector3D G4FConicalSurface::SurfaceNormal( const G4Point3D& p ) const
{  
  //  return the Normal unit vector to the G4ConicalSurface at a point p 
  //  on (or nearly on) the G4ConicalSurface
  G4Vector3D s  = G4Vector3D( p - origin );
  G4double   da = s * Position.GetAxis();
  G4double   r  = std::sqrt( s*s - da*da);
  G4double   z  = tan_angle * r; 
  
  if (Position.GetAxis().z() < 0)
    z = -z; 

  G4Vector3D n(p.x(), p.y(), z);
  n = n.unit();
  
  if( !sameSense )
    n = -n;

  return n; 
}

G4int G4FConicalSurface::Inside ( const G4Vector3D& x ) const
{ 
  // Return 0 if point x is outside G4ConicalSurface, 1 if Inside.
  if ( HowNear( x ) >= -0.5*kCarTolerance )
    return 1;
  else
    return 0; 
}

