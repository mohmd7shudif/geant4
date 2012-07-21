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
// $Id: G4StackManager.cc,v 1.15 2010-12-15 22:15:07 asaim Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
//  Last Modification : 09/Dec/96 M.Asai
//

#include "G4StackManager.hh"
#include "G4StackingMessenger.hh"
#include "G4VTrajectory.hh"
#include "evmandefs.hh"
#include "G4ios.hh"

G4StackManager::G4StackManager()
:userStackingAction(0),verboseLevel(0),numberOfAdditionalWaitingStacks(0)
{
  theMessenger = new G4StackingMessenger(this);
#ifdef G4_USESMARTSTACK
  urgentStack = new G4SmartTrackStack;
  G4cout<<"+++ G4StackManager uses G4SmartTrackStack. +++"<<G4endl;
#else
  urgentStack = new G4TrackStackDQ;
//  G4cout<<"+++ G4StackManager uses ordinary G4TrackStack. +++"<<G4endl;
#endif
  waitingStack = new G4TrackStackDQ;
  postponeStack = new G4TrackStackDQ;
}

G4StackManager::~G4StackManager()
{
  if(userStackingAction) delete userStackingAction;

  if(verboseLevel>0)
  {
    G4cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << G4endl;
    G4cout << " Maximum number of tracks in the urgent stack : " << urgentStack->GetMaxNTrack() << G4endl;
    G4cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << G4endl;
  }
  delete urgentStack;
  delete waitingStack;
  delete postponeStack;
  delete theMessenger;
  if(numberOfAdditionalWaitingStacks>0) {
    for(int i=0;i<numberOfAdditionalWaitingStacks;i++) {
      delete additionalWaitingStacks[i];
    }
  }
}

const G4StackManager & G4StackManager::operator=
(const G4StackManager &) { return *this; }
G4int G4StackManager::operator==(const G4StackManager &) 
const{ return false; }
G4int G4StackManager::operator!=(const G4StackManager &) 
const{ return true; }

G4int G4StackManager::PushOneTrack(G4Track *newTrack,G4VTrajectory *newTrajectory)
{
  G4ClassificationOfNewTrack classification;
  if(userStackingAction) 
  { classification = userStackingAction->ClassifyNewTrack( newTrack ); }
  else
  { classification = DefaultClassification( newTrack ); }

  if(classification==fKill)   // delete newTrack without stacking
  {
#ifdef G4VERBOSE
    if( verboseLevel > 1 )
    {
      G4cout << "   ---> G4Track " << newTrack << " (trackID "
	 << newTrack->GetTrackID() << ", parentID "
	 << newTrack->GetParentID() << ") is not to be stored." << G4endl;
    }
#endif
    delete newTrack;
    delete newTrajectory;
  }
  else
  {
    G4StackedTrack * newStackedTrack = new G4StackedTrack( newTrack, newTrajectory );
    switch (classification)
    {
      case fUrgent:
        urgentStack->PushToStack( newStackedTrack );
        break;
      case fWaiting:
        waitingStack->PushToStack( newStackedTrack );
        break;
      case fPostpone:
        postponeStack->PushToStack( newStackedTrack );
        break;
      default:
        G4int i = classification - 10;
        if(i<1||i>numberOfAdditionalWaitingStacks) {
          G4ExceptionDescription ED;
          ED << "invalid classification " << classification << G4endl;
          G4Exception("G4StackManager::PushOneTrack","Event0051",
          FatalException,ED);
        } else {
          additionalWaitingStacks[i-1]->PushToStack( newStackedTrack );
        }
        break;
    }
  }

  return GetNUrgentTrack();
}


G4Track * G4StackManager::PopNextTrack(G4VTrajectory**newTrajectory)
{
#ifdef G4VERBOSE
  if( verboseLevel > 1 )
  {
    G4cout << "### pop requested out of " 
         << GetNUrgentTrack() << " stacked tracks." << G4endl;
  }
#endif

  while( GetNUrgentTrack() == 0 )
  {
#ifdef G4VERBOSE
    if( verboseLevel > 1 ) G4cout << "### " << GetNWaitingTrack()
                      << " waiting tracks are re-classified to" << G4endl;
#endif
    waitingStack->TransferTo(urgentStack);
    if(numberOfAdditionalWaitingStacks>0) {
      for(int i=0;i<numberOfAdditionalWaitingStacks;i++) {
        if(i==0) {
          additionalWaitingStacks[0]->TransferTo(waitingStack);
        } else {
          additionalWaitingStacks[i]->TransferTo(additionalWaitingStacks[i-1]);
        }
      }
    }
    if(userStackingAction) userStackingAction->NewStage();
#ifdef G4VERBOSE
    if( verboseLevel > 1 ) G4cout << "     " << GetNUrgentTrack()
                      << " urgent tracks and " << GetNWaitingTrack()
                      << " waiting tracks." << G4endl;
#endif
    if( ( GetNUrgentTrack()==0 ) && ( GetNWaitingTrack()==0 ) ) return 0;
  }

  G4StackedTrack * selectedStackedTrack = urgentStack->PopFromStack();
  G4Track * selectedTrack = selectedStackedTrack->GetTrack();
  *newTrajectory = selectedStackedTrack->GetTrajectory();

#ifdef G4VERBOSE
  if( verboseLevel > 2 )
  {
    G4cout << "Selected G4StackedTrack : " << selectedStackedTrack 
         << " with G4Track " << selectedStackedTrack->GetTrack()
	 << " (trackID " << selectedStackedTrack->GetTrack()->GetTrackID()
	 << ", parentID " << selectedStackedTrack->GetTrack()->GetParentID()
	 << ")" << G4endl;
  }
#endif

  delete selectedStackedTrack;
  return selectedTrack;
}

void G4StackManager::ReClassify()
{
  G4StackedTrack * aStackedTrack;
  G4TrackStackDQ tmpStack;

  if( !userStackingAction ) return;
  if( GetNUrgentTrack() == 0 ) return;

  urgentStack->TransferTo(&tmpStack);
  while( (aStackedTrack=tmpStack.PopFromStack()) != 0 )
  {
    G4ClassificationOfNewTrack classification = 
      userStackingAction->ClassifyNewTrack( aStackedTrack->GetTrack() );
    switch (classification)
    {
      case fKill:
        delete aStackedTrack->GetTrack();
        delete aStackedTrack->GetTrajectory();
        delete aStackedTrack;
        break;
      case fUrgent:
        urgentStack->PushToStack( aStackedTrack );
        break;
      case fWaiting:
        waitingStack->PushToStack( aStackedTrack );
        break;
      case fPostpone:
        postponeStack->PushToStack( aStackedTrack );
        break;
      default:
        G4int i = classification - 10;
        if(i<1||i>numberOfAdditionalWaitingStacks) {
          G4ExceptionDescription ED;
          ED << "invalid classification " << classification << G4endl;
          G4Exception("G4StackManager::ReClassify","Event0052",
          FatalException,ED);
        } else {
          additionalWaitingStacks[i-1]->PushToStack( aStackedTrack );
        }
        break;
    }
  }
}

G4int G4StackManager::PrepareNewEvent()
{
  if(userStackingAction) userStackingAction->PrepareNewEvent();

  G4int n_passedFromPrevious = 0;

  if( GetNPostponedTrack() > 0 )
  {
#ifdef G4VERBOSE
    if( verboseLevel > 1 )
    {
      G4cout << GetNPostponedTrack() 
           << " postponed tracked are now shifted to the stack." << G4endl;
    }
#endif

    G4StackedTrack * aStackedTrack;
    G4TrackStackDQ tmpStack;

    postponeStack->TransferTo(&tmpStack);

    while( (aStackedTrack=tmpStack.PopFromStack()) != 0 )
    {
      G4Track* aTrack = aStackedTrack->GetTrack();
      aTrack->SetParentID(-1);
      G4ClassificationOfNewTrack classification;
      if(userStackingAction) 
      { classification = userStackingAction->ClassifyNewTrack( aTrack ); }
      else
      { classification = DefaultClassification( aTrack ); }

      if(classification==fKill)
      {
        delete aTrack;
        delete aStackedTrack->GetTrajectory();
        delete aStackedTrack;
      }
      else
      {
        aTrack->SetTrackID(-(++n_passedFromPrevious));
        switch (classification)
        {
          case fUrgent:
            urgentStack->PushToStack( aStackedTrack );
            break;
          case fWaiting:
            waitingStack->PushToStack( aStackedTrack );
            break;
          case fPostpone:
            postponeStack->PushToStack( aStackedTrack );
            break;
          default:
            G4int i = classification - 10;
            if(i<1||i>numberOfAdditionalWaitingStacks) {
              G4ExceptionDescription ED;
              ED << "invalid classification " << classification << G4endl;
              G4Exception("G4StackManager::PrepareNewEvent","Event0053",
              FatalException,ED);
            } else {
              additionalWaitingStacks[i-1]->PushToStack( aStackedTrack );
            }
            break;
        }
      }
    }
  }

  return n_passedFromPrevious;
}

void G4StackManager::SetNumberOfAdditionalWaitingStacks(G4int iAdd)
{
  if(iAdd > numberOfAdditionalWaitingStacks)
  {
    for(int i=numberOfAdditionalWaitingStacks;i<iAdd;i++)
    {
      G4TrackStackDQ* newStack = new G4TrackStackDQ;
      additionalWaitingStacks.push_back(newStack);
    }
    numberOfAdditionalWaitingStacks = iAdd;
  }
  else if (iAdd < numberOfAdditionalWaitingStacks)
  {
    for(int i=numberOfAdditionalWaitingStacks;i>iAdd;i--)
    {
      delete additionalWaitingStacks[i];
    }
  }
}

void G4StackManager::TransferStackedTracks(G4ClassificationOfNewTrack origin, G4ClassificationOfNewTrack destination)
{
  if(origin==destination) return;
  if(origin==fKill) return;
  G4TrackStackDQ* originStack = 0;
  switch(origin)
  {
    case fUrgent:
      originStack = 0;
      break;
    case fWaiting:
      originStack = waitingStack;
      break;
    case fPostpone:
      originStack = postponeStack;
      break;
    default:
      int i = origin - 10;
      if(i<=numberOfAdditionalWaitingStacks) originStack = additionalWaitingStacks[i-1];
      break;
  }

  if(destination==fKill)
  {
    if(originStack)
    { originStack->clear(); }
    else
    { urgentStack->clear(); }
  } 
  else
  {
    G4TrackStackDQ* targetStack = 0;
    switch(destination)
    {
      case fUrgent:
        targetStack = 0;
        break;
      case fWaiting:
        targetStack = waitingStack;
        break;
      case fPostpone:
        targetStack = postponeStack;
        break;
      default:
        int i = destination - 10;
        if(i<=numberOfAdditionalWaitingStacks) targetStack = additionalWaitingStacks[i-1];
        break;
    }
    if(originStack)
    {
      if(targetStack)
      { originStack->TransferTo(targetStack); }
      else
      { originStack->TransferTo(urgentStack); }
    }
    else
    { urgentStack->TransferTo(targetStack); }
  }
  return;
}

void G4StackManager::TransferOneStackedTrack(G4ClassificationOfNewTrack origin, G4ClassificationOfNewTrack destination)
{
  if(origin==destination) return;
  if(origin==fKill) return;
  G4TrackStackDQ* originStack = 0;
  switch(origin)
  {
    case fUrgent:
      originStack = 0;
      break;
    case fWaiting:
      originStack = waitingStack;
      break;
    case fPostpone:
      originStack = postponeStack;
      break;
    default:
      int i = origin - 10;
      if(i<=numberOfAdditionalWaitingStacks) originStack = additionalWaitingStacks[i-1];
      break;
  }

  G4StackedTrack * aStackedTrack = 0;
  if(destination==fKill)
  {
    if(originStack)
    { aStackedTrack = originStack->PopFromStack(); }
    else
    { aStackedTrack = urgentStack->PopFromStack(); }
    if(aStackedTrack)
    {
      delete aStackedTrack->GetTrack();
      delete aStackedTrack->GetTrajectory();
      delete aStackedTrack;
    }
  } 
  else
  {
    G4TrackStackDQ* targetStack = 0;
    switch(destination)
    {
      case fUrgent:
        targetStack = 0;
        break;
      case fWaiting:
        targetStack = waitingStack;
        break;
      case fPostpone:
        targetStack = postponeStack;
        break;
      default:
        int i = destination - 10;
        if(i<=numberOfAdditionalWaitingStacks) targetStack = additionalWaitingStacks[i-1];
        break;
    }
    if(originStack)
    { aStackedTrack = originStack->PopFromStack(); }
    else
    { aStackedTrack = urgentStack->PopFromStack(); }
    if(targetStack)
    { targetStack->PushToStack(aStackedTrack); }
    else
    { urgentStack->PushToStack(aStackedTrack); }
  }
  return;
}





