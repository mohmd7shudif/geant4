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
// $Id: G4VBasicShell.cc,v 1.16 2010-06-08 04:39:22 kmura Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//

#include "G4VBasicShell.hh"
#include "G4StateManager.hh"
#include "G4UIcommandTree.hh"
#include "G4UIcommand.hh"
#include "G4UIcommandStatus.hh"
#include "G4UImanager.hh"
#include <vector>
#include <sstream>

G4VBasicShell::G4VBasicShell()
:currentDirectory("/")
{
}

G4VBasicShell::~G4VBasicShell() 
{;}

G4String G4VBasicShell::ModifyToFullPathCommand(const char* aCommandLine)
{
  G4String rawCommandLine = (G4String)aCommandLine;
  if(rawCommandLine.isNull()||rawCommandLine(0)=='\0') return rawCommandLine;
  G4String commandLine = (G4String)rawCommandLine.strip(G4String::both);
  G4String commandString;
  G4String parameterString;
  size_t i = commandLine.index(" ");
  if( i != std::string::npos )
  {
    commandString = (G4String)commandLine(0,i);
    parameterString = " ";
    parameterString += (G4String)commandLine(i+1,commandLine.length()-(i+1));
  }
  else
  { commandString = commandLine; }

  G4String fullPathCommandLine
    = ModifyPath( commandString )+parameterString;
  return fullPathCommandLine;
}

G4String G4VBasicShell::GetCurrentWorkingDirectory()
{
  return currentDirectory;
}

G4bool G4VBasicShell::ChangeDirectory(const char* newDir)
{
  G4String aNewPrefix = (G4String)newDir;
  G4String newPrefix = (G4String)aNewPrefix.strip(G4String::both);
  G4String newDirectory = ModifyPath( newPrefix );
  if( newDirectory( newDirectory.length() - 1 ) != '/' )
  { newDirectory += "/"; }
  if( FindDirectory( (const char*)newDirectory ) == NULL )
  { return false; }
  currentDirectory = newDirectory;
  return true;
}

G4UIcommandTree* G4VBasicShell::FindDirectory(const char* dirName)
{
  G4String aDirName = (G4String)dirName;
  G4String theDir = (G4String)aDirName.strip(G4String::both);
  G4String targetDir = ModifyPath( theDir );
  if( targetDir( targetDir.length()-1 ) != '/' )
  { targetDir += "/"; }
  G4UIcommandTree* comTree = G4UImanager::GetUIpointer()->GetTree();
  if( targetDir == "/" )
  { return comTree; }
  size_t idx = 1;
  while( idx < targetDir.length()-1 )
  {
    size_t i = targetDir.index("/",idx);
    comTree = comTree->GetTree((G4String)targetDir(0,i+1));
    if( comTree == NULL ) 
    { return NULL; }
    idx = i+1;
  }
  return comTree;
}

G4UIcommand* G4VBasicShell::FindCommand(const char* commandName)
{
  G4String rawCommandLine = (G4String)commandName;
  G4String commandLine = (G4String)rawCommandLine.strip(G4String::both);
  G4String commandString;
  size_t i = commandLine.index(" ");
  if( i != std::string::npos )
  { commandString = (G4String)commandLine(0,i); }
  else
  { commandString = commandLine; }

  G4String targetCom = ModifyPath(commandString);
  return G4UImanager::GetUIpointer()->GetTree()->FindPath(targetCom);
}

G4String G4VBasicShell::ModifyPath(G4String tempPath)
{
  G4String newPath = currentDirectory;

  if( tempPath.length()>0 )
  {

  if( tempPath(0) == '/' )   // full path is given
  { newPath = tempPath; }
  else if( tempPath(0) != '.' ) // add current prefix
  { newPath += tempPath; }
  else if( tempPath(0,2) == "./" ) // add current prefix
  { newPath += (G4String)tempPath(2,tempPath.length()-2); }
  else                       // swim up with ".."
  {
    while( 1 )
    {
      if( tempPath(0,2) == ".." )
      {
        if( newPath != "/" )
        { 
	  G4String tmpString = (G4String)newPath(0,newPath.length()-1);
          newPath = (G4String)newPath(0,tmpString.last('/')+1); 
        }
        if( tempPath == ".." || tempPath == "../" )
        { break; }
        tempPath = (G4String)tempPath(3,tempPath.length()-3);
      }
      else
      {
        newPath += tempPath;
        break;
      }
    }
  }

  }

  return newPath;
}
////////////////////////////////////////////
// Method used for command completion //////
////////////////////////////////////////////
G4String G4VBasicShell::Complete(G4String commandName)
{
  G4String rawCommandLine = commandName;
  G4String commandLine = rawCommandLine.strip(G4String::both);
  size_t i = commandLine.index(" ");
  if( i != std::string::npos ) return rawCommandLine; // Already entering parameters, 
                                            // assume command path is correct.
  G4String commandString = commandLine; 
  G4String targetCom = ModifyPath(commandString);
  G4UIcommandTree* tree = G4UImanager::GetUIpointer()->GetTree();
  G4String value = FindMatchingPath(tree,targetCom);
  if(value=="") return rawCommandLine;
  return value;
}
G4String G4VBasicShell::FindMatchingPath(
 G4UIcommandTree* aTree
,G4String aCommandPath
)
{
  return aTree->CompleteCommandPath(aCommandPath);
}
////////////////////////////////////////////
// Method involving an interactive G4cout //
////////////////////////////////////////////
/***************************************************************************/
void G4VBasicShell::ExecuteCommand (
 G4String aCommand
)
/***************************************************************************/
// Should be put in G4VBasicShell.
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  if(aCommand.length()<2) return;
  G4UImanager* UI = G4UImanager::GetUIpointer();
  if(UI==NULL) return;
  G4int commandStatus = UI->ApplyCommand(aCommand);
  switch(commandStatus) {
  case fCommandSucceeded:
    break;
  case fCommandNotFound:
    G4cerr << "command not found" << G4endl;
    break;
  case fIllegalApplicationState:
    G4cerr << "illegal application state -- command refused" << G4endl;
    break;
  case fParameterOutOfRange:
  case fParameterUnreadable:
  case fParameterOutOfCandidates:
  default:
    G4cerr << "command refused (" << commandStatus << ")" << G4endl;
  }
}
/***************************************************************************/
void G4VBasicShell::ApplyShellCommand (
 G4String a_string
,G4bool& exitSession
,G4bool& exitPause
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  G4UImanager* UI = G4UImanager::GetUIpointer();
  if(UI==NULL) return;

  G4String command = a_string.strip(G4String::leading);
  if( command(0) == '#' ) { 

    G4cout << command << G4endl; 

  } else if( command == "ls" || (G4String)command(0,3) == "ls " ) {

    ListDirectory( command );

  } else if( command == "pwd" ) { 

    G4cout << "Current Working Directory : " 
       << GetCurrentWorkingDirectory() << G4endl; 

  } else if( command == "cd" || (G4String)command(0,3) == "cd ") {

    ChangeDirectoryCommand ( command );

  } else if( command == "help" || (G4String)command(0,5) == "help ") {

    TerminalHelp( command ); 

  } else if( command(0) == '?' ) { 

    ShowCurrent( command );

  } else if( command == "hist" || command == "history") {

    G4int nh = UI->GetNumberOfHistory();
    for(G4int i=0;i<nh;i++) { 
      G4cout << i << ": " << UI->GetPreviousCommand(i) << G4endl; 
    }

  } else if( command(0) == '!' ) {

    G4String ss = command(1,command.length()-1);
    G4int vl;
    const char* tt = ss;
    std::istringstream is(tt);
    is >> vl;
    G4int nh = UI->GetNumberOfHistory();
    if(vl>=0 && vl<nh) { 
      G4String prev = UI->GetPreviousCommand(vl); 
      G4cout << prev << G4endl;
      ExecuteCommand (ModifyToFullPathCommand(prev));
    } else { 
      G4cerr << "history " << vl << " is not found." << G4endl; 
    }

  } else if( command == "exit" ) { 

    if( exitPause == false) { //In a secondary loop.
      G4cout << "You are now processing RUN." << G4endl;
      G4cout << "Please abort it using \"/run/abort\" command first" << G4endl;
      G4cout << " and use \"continue\" command until the application" << G4endl;
      G4cout << " becomes to Idle." << G4endl;
    } else {
      exitSession = true;
    }

  } else if( command == "cont" || command == "continue"){

    exitPause = true;

  } else {

    ExecuteCommand (ModifyToFullPathCommand(a_string));

  }
}
void G4VBasicShell::ShowCurrent(G4String newCommand)
{
  G4UImanager* UI = G4UImanager::GetUIpointer();
  if(UI==NULL) return;
  G4String comString = newCommand(1,newCommand.length()-1);
  G4String theCommand = ModifyToFullPathCommand(comString);
  G4String curV = UI->GetCurrentValues(theCommand);
  if( ! curV.isNull() ) { 
    G4cout << "Current value(s) of the parameter(s) : " << curV << G4endl; 
  }
}
void G4VBasicShell::ChangeDirectoryCommand(G4String newCommand)
{
  G4String prefix;
  if( newCommand.length() <= 3 ) { 
    prefix = "/"; 
  } else {
    G4String aNewPrefix = newCommand(3,newCommand.length()-3);
    prefix = aNewPrefix.strip(G4String::both);
  }
  if(!ChangeDirectory(prefix)) { 
    G4cout << "directory <" << prefix << "> not found." << G4endl; 
  }
}
void G4VBasicShell::ListDirectory(G4String newCommand)
{
  G4String targetDir;
  if( newCommand.length() <= 3 ) { 
    targetDir = GetCurrentWorkingDirectory();
  } else {
    G4String newPrefix = newCommand(3,newCommand.length()-3);
    targetDir = newPrefix.strip(G4String::both);
  }
  G4UIcommandTree* commandTree = FindDirectory( targetDir );
  if( commandTree == NULL ) { 
    G4cout << "Directory <" << targetDir << "> is not found." << G4endl; 
  } else { 
    commandTree->ListCurrent(); 
  }
}
void G4VBasicShell::TerminalHelp(G4String newCommand)
{
  G4UImanager* UI = G4UImanager::GetUIpointer();
  if(UI==NULL) return;
  G4UIcommandTree * treeTop = UI->GetTree();
  size_t i = newCommand.index(" ");
  if( i != std::string::npos )
  {
    G4String newValue = newCommand(i+1,newCommand.length()-(i+1));
    newValue.strip(G4String::both);
    G4String targetCom = ModifyToFullPathCommand( newValue );
    G4UIcommand* theCommand = treeTop->FindPath( targetCom );
    if( theCommand != NULL ) 
    { 
      theCommand->List();
      return;
    }
    else
    {
      G4cout << "Command <" << newValue << " is not found." << G4endl;
      return;
    }
  }

  G4UIcommandTree * floor[10];
  floor[0] = treeTop;
  size_t iFloor = 0;
  size_t prefixIndex = 1;
  G4String prefix = GetCurrentWorkingDirectory();
  while( prefixIndex < prefix.length()-1 )
  {
    size_t ii = prefix.index("/",prefixIndex);
    floor[iFloor+1] = 
      floor[iFloor]->GetTree(G4String(prefix(0,ii+1)));
    prefixIndex = ii+1;
    iFloor++;
  }
  floor[iFloor]->ListCurrentWithNum();
  // 1998 Oct 2 non-number input
  while(1){
   //G4cout << G4endl << "Type the number ( 0:end, -n:n level back ) : "<<std::flush;
    G4cout << G4endl << "Type the number ( 0:end, -n:n level back ) : "<<G4endl;
    G4int i;
    if(!GetHelpChoice(i)){
      G4cout << G4endl << "Not a number, once more" << G4endl; 
      continue;
    } else if( i < 0 ){
      if( iFloor < (size_t)-i ) iFloor = 0;
      else iFloor += i;
      //iFloor += i;
      //if( iFloor < 0 ) iFloor = 0;
      floor[iFloor]->ListCurrentWithNum(); 
      continue;
    } else if(i == 0) { 
      break;
    } else if( i > 0 ) {
      G4int n_tree = floor[iFloor]->GetTreeEntry();
      if( i > n_tree )
      { 
        if( i <= n_tree + floor[iFloor]->GetCommandEntry() )
        { 
          floor[iFloor]->GetCommand(i-n_tree)->List(); 
        }
      }
      else
      {
        floor[iFloor+1] = floor[iFloor]->GetTree(i);
        iFloor++;
        floor[iFloor]->ListCurrentWithNum();
      }
    }
  }
  G4cout << "Exit from HELP." << G4endl << G4endl;
  //G4cout << G4endl;
  ExitHelp();
}
