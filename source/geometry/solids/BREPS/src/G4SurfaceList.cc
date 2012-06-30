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
// $Id: G4SurfaceList.cc,v 1.8 2008-03-13 14:18:57 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// ----------------------------------------------------------------------
// GEANT 4 class source file
//
// G4SurfaceList.cc
//
// ----------------------------------------------------------------------

#include "G4SurfaceList.hh"

G4SurfaceList::G4SurfaceList()
{
  first = index = last = next = temp = (G4Surface*)0;
  number_of_elements=0; 
}


G4SurfaceList::~G4SurfaceList()
{
  EmptyList();
}

void G4SurfaceList::MoveToFirst(G4Surface* srf)
{
  if(number_of_elements)
  {
    RemovePointer();
    srf->SetNextNode(first);  
    first = srf;
    index=first;
    number_of_elements++;  
  }
}


void G4SurfaceList::AddSurface(G4Surface* srf)
{
  if(first == (G4Surface*)0)
  {
    index = srf;
    first = srf;
    last = srf;
    first->SetNextNode(0);
  }
  else
  {
    srf->SetNextNode(last->GetNextNode());  
    last->SetNextNode(srf);
    last = last->GetNextNode();
  }
  
  number_of_elements++;  
  index=first;
}


G4Surface* G4SurfaceList::GetSurface()
{
  return index;
}


const G4Surface* G4SurfaceList::GetSurface(G4int number)
{
  index = first;
  for(G4int a=0;a<number;a++)
    Step();
    
  return index;
}


const G4Surface* G4SurfaceList::GetLastSurface() const 
{
  return last;
}


void G4SurfaceList::RemoveSurface(G4Surface* srf)
{
  if(srf!=(G4Surface*)0)
  {
    number_of_elements--;  
    temp = first;
    
    if(srf == first)
    {
      first=first->GetNextNode();
      index = first;
      if(number_of_elements == 0)last = first;
      delete srf;
      return;
    }
    else	
    {
      while(temp->GetNextNode() != srf) temp = temp->GetNextNode();
      index = srf->GetNextNode();
      temp->SetNextNode(index);
      if(srf == last) last = temp;
      index = first;
      delete srf;
    }
  }
}


void G4SurfaceList::RemovePointer()
{
  // Remove the current pointer from the List
  // Do not delete the object itself
  if(number_of_elements)
  {
    if(first != index)
    {
      temp = first;
      
      // Find previous
      while(temp->GetNextNode() != index) temp = temp->GetNextNode();
      
      // Hop over the one to be removed
      temp->SetNextNode(index->GetNextNode());
	
      // Correct the index pointer
      index = temp->GetNextNode();
    }
    else
    {
      // Hop over the first
      first = first->GetNextNode();
      index = first;
    }
  }

  number_of_elements--;
}


void G4SurfaceList::EmptyList()
{
  //Deletes all surfaces in List
  while (first != (G4Surface*)0)
  {
    temp  = first;
    first = first->GetNextNode();
    delete temp;
    number_of_elements--;
  }
  
  last = index = first;
}


void G4SurfaceList::MoveToFirst()
{
  index = first;
}


void G4SurfaceList::Step()
{
  if(index!=(G4Surface*)0)
    index = index->GetNextNode();
}


void G4SurfaceList::G4SortList()
{
  if(number_of_elements == 1) return;
  
  // First create a vector of the surface distances
  // to the ray origin
  G4Surface** distances = new G4Surface*[number_of_elements];
  G4int x = 0;
  MoveToFirst();

  // Copy surface pointers to vector
  if(number_of_elements > 1)
  {
    while(x < number_of_elements)
    {
      distances[x] = index;
      index = index->GetNextNode();
      x++;
    }
    
    MoveToFirst();

    // Sort List of pointers using quick G4Sort
    QuickG4Sort( distances, 0, number_of_elements-1 );
    
    // Organize the linked List of surfaces according
    // to the quickG4Sorted List.
    x = 0;
    first = distances[x];
    last = first;
    x++;	

    while (x < number_of_elements)
    {
      last->SetNextNode(distances[x]);
      last = last->GetNextNode();
      x++;
    }
	
    last->SetNextNode(0);
    MoveToFirst();
  }
  
  delete[] distances;
}


void G4SurfaceList::QuickG4Sort(G4Surface** Dist, G4int left, G4int right)
{
  register G4int i=left;
  register G4int j=right;
  
  G4Surface* elem1;
  G4Surface* elem2 = Dist[(left+right)/2];
  
  do
  {
    while ( (Dist[i]->GetDistance() < elem2->GetDistance())  &&  (i < right) ) 
      i++;
    
    while ( (elem2->GetDistance() < Dist[j]->GetDistance())  &&  (j > left))
      j--;

    if(i<=j)
    {
      elem1   = Dist[i];
      Dist[i] = Dist[j];
      Dist[j] = elem1;
      i++;
      j--;
    }
  } while (i<=j); 
  
  if( left < j  ) 
    QuickG4Sort(Dist, left, j );

  if( i < right ) 
    QuickG4Sort(Dist, i, right);    
}
