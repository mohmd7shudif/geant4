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
// $Id: G4ConicalSurface.cc,v 1.12 2010-07-07 14:45:31 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// ----------------------------------------------------------------------
// GEANT 4 class source file
//
// G4ConicalSurface.cc
//
// ----------------------------------------------------------------------

#include "G4ConicalSurface.hh"
#include "G4Sort.hh"
#include "G4Globals.hh"


G4ConicalSurface::G4ConicalSurface()
  : G4Surface(), axis(G4Vector3D(1.,0.,0.)), angle(1.)
{  
}

G4ConicalSurface::G4ConicalSurface( const G4Point3D&, 
				    const G4Vector3D& a,
				    G4double e           )
  : G4Surface()
{  
  // Normal constructor
  // require axis to be a unit vector

  G4double amag = a.mag2();

  if ( amag != 0.0 )
  {
    axis = a*(1/amag);
  }
  else
  {
    std::ostringstream message;
    message << "Axis has zero length." << G4endl
	    << "Default axis ( 1.0, 0.0, 0.0 ) is used.";
    G4Exception("G4ConicalSurface::G4ConicalSurface()", "GeomSolids1001",
                JustWarning, message);

    axis = G4Vector3D( 1.0, 0.0, 0.0 );
  }

  //  Require angle to range from 0 to PI/2
  //
  if ( ( e > 0.0 ) && ( e < ( 0.5 * pi ) ) )
  {
    angle = e;
  }
  else
  {
    std::ostringstream message;
    message << "Angle out of range." << G4endl
            << "Asked for angle out of allowed range of 0 to "
	    << 0.5*pi << " (PI/2): " << e << G4endl
	    << "Default angle of 1.0 is used.";    
    G4Exception("G4ConicalSurface::G4ConicalSurface()", "GeomSolids1001",
                JustWarning, message);
    angle = 1.0;
  }
}


G4ConicalSurface::~G4ConicalSurface()
{
}


G4ConicalSurface::G4ConicalSurface( const G4ConicalSurface& c )
  : G4Surface(), axis(c.axis), angle(c.angle)
{
}


G4ConicalSurface&
G4ConicalSurface::operator=( const G4ConicalSurface& c )
{
  if (&c == this)  { return *this; }
  axis    = c.axis;
  angle   = c.angle;

  return *this;
}


const char* G4ConicalSurface::NameOf() const
{
   return "G4ConicalSurface";
}

void G4ConicalSurface::CalcBBox()
{
  bbox= new G4BoundingBox3D(surfaceBoundary.BBox().GetBoxMin(), 
                            surfaceBoundary.BBox().GetBoxMax());
}

void G4ConicalSurface::PrintOn( std::ostream& os ) const
{ 
  // printing function using C++ std::ostream class

  os << "G4ConicalSurface surface with origin: " << origin << "\t"
     << "angle: " << angle << " radians \tand axis " << axis << "\n";
}


G4double G4ConicalSurface::HowNear( const G4Vector3D& x ) const
{ 
  // Distance from the point x to the semi-infinite G4ConicalSurface.
  // The distance will be positive if the point is Inside the G4ConicalSurface,
  // negative if the point is outside.
  // Note that this may not be correct for a bounded conical object
  // subclassed to G4ConicalSurface.

  G4Vector3D d    = G4Vector3D( x - origin );
  G4double   l    = d * axis;
  G4Vector3D q    = G4Vector3D( origin  +  l * axis );
  G4Vector3D v    = G4Vector3D( x - q );

  G4double   Dist = ( l*std::tan(angle) - v.mag2() ) * std::cos(angle);

  return Dist;
}


G4int G4ConicalSurface::Intersect( const G4Ray& ry )  
{
  //  Distance along a Ray (straight line with G4Vector3D) to leave or enter
  //  a G4ConicalSurface.  The input variable which_way should be set to +1 to
  //  indicate leaving a G4ConicalSurface, -1 to indicate entering a 
  //  G4ConicalSurface.
  //  p is the point of intersection of the Ray with the G4ConicalSurface.
  //  If the G4Vector3D of the Ray is opposite to that of the Normal to
  //  the G4ConicalSurface at the intersection point, it will not leave the 
  //  G4ConicalSurface.
  //  Similarly, if the G4Vector3D of the Ray is along that of the Normal 
  //  to the G4ConicalSurface at the intersection point, it will not enter the
  //  G4ConicalSurface.
  //  This method is called by all finite shapes sub-classed to 
  //  G4ConicalSurface.
  //  Use the virtual function table to check if the intersection point
  //  is within the boundary of the finite shape.
  //  A negative result means no intersection.
  //  If no valid intersection point is found, set the distance
  //  and intersection point to large numbers.
  
  G4int which_way = -1; // Originally a parameter.Read explanation above. 

  distance = kInfinity;

  G4Vector3D lv ( kInfinity, kInfinity, kInfinity );
  closest_hit = lv;

  //  Origin and G4Vector3D unit vector of Ray.
  //
  G4Vector3D x = G4Vector3D( ry.GetStart() );
  G4Vector3D dhat = ry.GetDir();
  
  
  //  Cone angle and axis unit vector.
  //
  G4double   ta      = std::tan( GetAngle() );
  G4Vector3D ahat    = GetAxis();
  G4int      isoln   = 0, 
             maxsoln = 2;

  //  array of solutions in distance along the Ray
  //
  G4double s[2];
  s[0] = -1.0; 
  s[1] = -1.0 ;

  //  calculate the two solutions (quadratic equation)
  //
  G4Vector3D gamma = G4Vector3D( x - GetOrigin() );
  G4double   T  = 1.0  +  ta * ta;
  G4double   ga = gamma * ahat;
  G4double   da = dhat * ahat;
  G4double   A  = 1.0 - T * da * da;
  G4double   B  = 2.0 * ( gamma * dhat - T * ga * da );
  G4double   C  = gamma * gamma - T * ga * ga;

  //  if quadratic term vanishes, just do the simple solution
  //
  if ( std::fabs( A ) < kCarTolerance ) 
  {
    if ( B == 0.0 )
      { return 1; }
    else
      { s[0] = -C / B; }
  }

  //  Normal quadratic case, no intersection if radical is less than zero
  //
  else 
  {
    G4double radical = B * B  -  4.0 * A * C; 
    if ( radical < 0.0 ) 
    {
      return 1;
    }
    else 
    {
      G4double root = std::sqrt( radical );
      s[0] = ( - B + root ) / ( 2. * A );
      s[1] = ( - B - root ) / ( 2. * A );
    }
  }

  //  order the possible solutions by increasing distance along the Ray
  //
  sort_double( s, isoln, maxsoln-1 );

  //  now loop over each positive solution, keeping the first one (smallest
  //  distance along the Ray) which is within the boundary of the sub-shape
  //  and which also has the correct G4Vector3D with respect to the Normal to
  //  the G4ConicalSurface at the intersection point
  //
  for ( isoln = 0; isoln < maxsoln; isoln++ ) 
  {
    if ( s[isoln] >= 0.0 ) 
    {
      if ( s[isoln] >= kInfinity )  // quit if too large
      {
        return 1;
      }
      
      distance = s[isoln];
      closest_hit = ry.GetPoint( distance );

      //  Following line necessary to select non-reflective solutions.
      //
      if (( ahat * ( closest_hit - GetOrigin() ) > 0.0 ) && 
	  ((( dhat * SurfaceNormal( closest_hit ) * which_way )) >= 0.0 ) && 
	  ( std::fabs(HowNear( closest_hit )) < 0.1)                               )
      {
        return 1;
      }
    }
  }

  //  get here only if there was no solution within the boundary, Reset
  //  distance and intersection point to large numbers
  //
  distance = kInfinity;
  closest_hit = lv;

  return 0;
}


/*
  G4double G4ConicalSurface::distanceAlongHelix(G4int which_way, const Helix* hx,
  G4Vector3D& p ) const 
  {  //  Distance along a Helix to leave or enter a G4ConicalSurface.
  //  The input variable which_way should be set to +1 to
  //  indicate leaving a G4ConicalSurface, -1 to indicate entering a 
  //  G4ConicalSurface.
  //  p is the point of intersection of the Helix with the G4ConicalSurface.
  //  If the G4Vector3D of the Helix is opposite to that of the Normal to
  //  the G4ConicalSurface at the intersection point, it will not leave the 
  //  G4ConicalSurface.
  //  Similarly, if the G4Vector3D of the Helix is along that of the Normal 
  //  to the G4ConicalSurface at the intersection point, it will not enter the
  //  G4ConicalSurface.
  //  This method is called by all finite shapes sub-classed to 
  //  G4ConicalSurface.
  //  Use the virtual function table to check if the intersection point
  //  is within the boundary of the finite shape.
  //  If no valid intersection point is found, set the distance
  //  and intersection point to large numbers.
  //  Possible negative distance solutions are discarded.
  G4double Dist = kInfinity;
  G4Vector3D lv ( kInfinity, kInfinity, kInfinity );
  p = lv;
  G4int isoln = 0, maxsoln = 4;
  
  //  Array of solutions in turning angle
  //	G4double s[4] = { -1.0, -1.0, -1.0, -1.0 };
  G4double s[4];s[0] = -1.0; s[1]= -1.0 ;s[2] = -1.0; s[3]= -1.0 ;
  
  //  Flag set to 1 if exact solution is found
  G4int exact = 0;
  
  //  Helix parameters
  G4double   rh     = hx->GetRadius();      // radius of Helix
  G4Vector3D oh     = hx->position();       // origin of Helix
  G4Vector3D dh     = hx->direction( 0.0 ); // initial G4Vector3D of Helix
  G4Vector3D prp    = hx->getPerp();	    // perpendicular vector
  G4double   prpmag = prp.Magnitude();
  G4double   rhp    = rh / prpmag;

   //  G4ConicalSurface parameters
   G4double   ta = std::tan( GetAngle() );  // tangent of angle of G4ConicalSurface
   G4Vector3D oc = GetOrigin();        // origin of G4ConicalSurface
   G4Vector3D ac = GetAxis();          // axis of G4ConicalSurface
   
   //  Calculate quantities of use later on
   G4Vector3D alpha = rhp * prp;
   G4Vector3D beta  = rhp * dh;
   G4Vector3D gamma = oh - oc;
   G4double   T     = 1.0  +  ta * ta;
   G4double   gc    = gamma * ac;
   G4double   bc    = beta * ac;
   
   //  General approximate solution for std::sin(s)-->s and std::cos(s)-->1-s**2/2,
   //  keeping only terms to second order in s
   G4double A = gamma * alpha - T * ( gc * alpha * ac - bc * bc ) +
                beta * beta;
   G4double B = 2.0 * ( gamma * beta - gc * bc * T );
   G4double C = gamma * gamma - gc * gc * T;
   
   //  Solution for no quadratic term
   if ( std::fabs( A ) < kCarTolerance ) 
   {
     if ( B == 0.0 )
       return Dist;
     else
       s[0] = -C / B;
   }

   //  General quadratic solutions
   else {
   G4double radical = B * B - 4.0 * A * C;
   if ( radical < 0.0 )
   //  Radical is less than zero, either there is no intersection, or the
   //  approximation doesn't hold, so try a cruder technique to find a 
   //  possible intersection point using the gropeAlongHelix function.
   s[0] = gropeAlongHelix( hx );
   //  Normal non-negative radical solutions
   else {
   G4double root = std::sqrt( radical );
   s[0] = ( -B + root ) / ( 2.0 * A );
   s[1] = ( -B - root ) / ( 2.0 * A );
   if ( rh < 0.0 ) {
   s[0] = -s[0];
   s[1] = -s[1];
   }
   s[2] = s[0] + 2.0 * pi;
   s[3] = s[1] + 2.0 * pi;
   }
   }
   //
   //  Order the possible solutions by increasing turning angle
   //  (G4Sorting routines are in support/G4Sort.h).
   G4Sort_double( s, isoln, maxsoln-1 );
   //
   //  Now loop over each positive solution, keeping the first one (smallest
   //  distance along the Helix) which is within the boundary of the sub-shape.
   for ( isoln = 0; isoln < maxsoln; isoln++ ) {
   if ( s[isoln] >= 0.0 ) {
   //  Calculate distance along Helix and position and G4Vector3D vectors.
   Dist = s[isoln] * std::fabs( rhp );
   p = hx->position( Dist );
   G4Vector3D d = hx->direction( Dist );
   if ( exact == 0 ) {  //  only for approximate solns
   //  Now do approximation to get remaining distance to correct this solution.
   //  Iterate it until the accuracy is below the user-set surface precision.
   G4double delta = 0.;  
   G4double delta0 = kInfinity;
   G4int dummy = 1;
   G4int iter = 0;
   G4int in0 = Inside( hx->position() );
   G4int in1 = Inside( p );
   G4double sc = Scale();
   while ( dummy ) {
   iter++;
   //  Terminate loop after 50 iterations and Reset distance to large number,
   //  indicating no intersection with G4ConicalSurface.
   //  This generally occurs if the Helix curls too tightly to Intersect it.
   if ( iter > 50 ) {
   Dist = kInfinity;
   p = lv;
   break;
   }
   //  Find distance from the current point along the above-calculated
   //  G4Vector3D using a Ray.
   //  The G4Vector3D of the Ray and the Sign of the distance are determined
   //  by whether the starting point of the Helix is Inside or outside of
   //  the G4ConicalSurface.
   in1 = Inside( p );
   if ( in1 ) {  //  current point Inside
   if ( in0 ) {  //  starting point Inside
   Ray* r = new Ray( p, d );
   delta = 
   distanceAlongRay( 1, r, p );
   delete r;
   }
   else {       //  starting point outside
   Ray* r = new Ray( p, -d );
   delta = 
   -distanceAlongRay( 1, r, p );
   delete r;
   }
   }
   else {        //  current point outside
   if ( in0 ) {  //  starting point Inside
   Ray* r = new Ray( p, -d );
   delta = 
   -distanceAlongRay( -1, r, p );
   delete r;
   }
   else {        //  starting point outside
   Ray* r = new Ray( p, d );
   delta = 
   distanceAlongRay( -1, r, p );
   delete r;
   }
   }
   //  Test if distance is less than the surface precision, if so Terminate loop.
   if ( std::fabs( delta / sc ) <= SURFACE_PRECISION )
   break;
   //  If delta has not changed sufficiently from the previous iteration, 
   //  skip out of this loop.
   if ( std::fabs( ( delta - delta0 ) / sc ) <=
   SURFACE_PRECISION )
   break;
   //  If delta has increased in absolute value from the previous iteration
   //  either the Helix doesn't Intersect the G4ConicalSurface or the approximate solution
   //  is too far from the real solution.  Try groping for a solution.  If not
   //  found, Reset distance to large number, indicating no intersection with
   //  the G4ConicalSurface.
   if ( std::fabs( delta ) > std::fabs( delta0 ) ) {
   Dist = std::fabs( rhp ) * 
   gropeAlongHelix( hx );
   if ( Dist < 0.0 ) {
   Dist = kInfinity;
   p = lv;
   }
   else
   p = hx->position( Dist );
   break;
   }
   //  Set old delta to new one.
   delta0 = delta;
   //  Add distance to G4ConicalSurface to distance along Helix.
   Dist += delta;
   //  Negative distance along Helix means Helix doesn't Intersect G4ConicalSurface.
   //  Reset distance to large number, indicating no intersection with G4ConicalSurface.
   if ( Dist < 0.0 ) {
   Dist = kInfinity;
   p = lv;
   break;
   }
   //  Recalculate point along Helix and the G4Vector3D.
   p = hx->position( Dist );
   d = hx->direction( Dist );
   }  //  end of while loop
   }  //  end of exact == 0 condition
   //  Now have best value of distance along Helix and position for this
   //  solution, so test if it is within the boundary of the sub-shape
   //  and require that it point in the correct G4Vector3D with respect to
   //  the Normal to the G4ConicalSurface.
   if ( ( Dist < kInfinity ) &&
   ( ( hx->direction( Dist ) * Normal( p ) *
   which_way ) >= 0.0 ) &&
   ( WithinBoundary( p ) == 1 ) )
   return Dist;
   }  // end of if s[isoln] >= 0.0 condition
   }  // end of for loop over solutions
   //  If one gets here, there is no solution, so set distance along Helix
   //  and position to large numbers.
   Dist = kInfinity;
   p = lv;
   return Dist;
   }
*/


G4Vector3D G4ConicalSurface::SurfaceNormal( const G4Point3D& p ) const
{  
  //  return the Normal unit vector to the G4ConicalSurface at a point p 
  //  on (or nearly on) the G4ConicalSurface

  G4Vector3D s    = G4Vector3D( p - origin );
  G4double   smag = s.mag2();
  
  //  if the point happens to be at the origin, calculate a unit vector Normal
  //  to the axis, with zero z component
  //
  if ( smag == 0.0 )
  {
    G4double ax = axis.x();
    G4double ay = axis.y();
    G4double ap = std::sqrt( ax * ax  +  ay * ay );

    if ( ap == 0.0 )
      { return G4Vector3D( 1.0, 0.0, 0.0 ); }
    else
      { return G4Vector3D( ay / ap, -ax / ap, 0.0 ); }
  }
  else  // otherwise do the calculation of the Normal to the conical surface
  {
    G4double l = s * axis;
    s = s*(1/smag);
    G4Vector3D q    = G4Vector3D( origin  +  l * axis );
    G4Vector3D v    = G4Vector3D( p - q );
    G4double   sl   = v.mag2() * std::sin( angle );
    G4Vector3D n    = G4Vector3D( v - sl * s );
    G4double   nmag = n.mag2(); 

    if ( nmag != 0.0 )
    {
      n=n*(1/nmag);
    }
    return n;
  }
}


G4int G4ConicalSurface::Inside ( const G4Vector3D& x ) const
{ 
  // Return 0 if point x is outside G4ConicalSurface, 1 if Inside.
  // Outside means that the distance to the G4ConicalSurface would be negative.
  // Use the HowNear function to calculate this distance.

  if ( HowNear( x ) >= -0.5*kCarTolerance )
    { return 1; }
  else
    { return 0; }
}


G4int G4ConicalSurface::WithinBoundary( const G4Vector3D& x ) const
{  
  //  return 1 if point x is on the G4ConicalSurface, otherwise return zero
  //  base this on the surface precision factor

  if ( std::fabs( HowNear( x ) / Scale() ) <= SURFACE_PRECISION )
    { return 1; }
  else
    { return 0; }
}

G4double G4ConicalSurface::Scale() const
{
  return 1.0;
}

void G4ConicalSurface::SetAngle( G4double e )
{
  //  Reset the angle of the G4ConicalSurface
  //  Require angle to range from 0 to PI/2

  if ( (e > 0.0) && (e <= ( 0.5 * pi )) )
  {
    angle = e;
  }
  else   // use old value (do not change angle) if out of the range, 
  {      // but print warning message
    std::ostringstream message;
    message << "Angle out of range." << G4endl
            << "Asked for angle out of allowed range of 0 to "
	    << 0.5*pi << " (PI/2): " << e << G4endl
	    << "Default angle of " << angle << " is used.";    
    G4Exception("G4ConicalSurface::SetAngle()", "GeomSolids1001",
                JustWarning, message);
  }
}

