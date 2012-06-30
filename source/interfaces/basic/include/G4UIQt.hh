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
// $Id: G4UIQt.hh,v 1.23 2010-06-10 15:37:13 lgarnier Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
#ifndef G4UIQt_h
#define G4UIQt_h 

#if defined(G4UI_BUILD_QT_SESSION) || defined(G4UI_USE_QT)

#include <map>

#include "G4VBasicShell.hh"
#include "G4VInteractiveSession.hh"

#include <qobject.h>
#include <qmap.h>
#include <qstringlist.h>
#include <qtabwidget.h>

class QMainWindow;
class QLineEdit;
class G4UIsession;
class QListWidget;
class QTreeWidget;
class QTreeWidgetItem;
class QTextEdit;
class QLabel;
class QResizeEvent;
class QToolBox;
class QStringList;
class QSplitter;

// Class description :
//
//  G4UIQt : class to handle a Qt interactive session.
// G4UIQt is the Qt version of G4UIterminal.
//
//  A command box is at disposal for entering/recalling Geant4 commands.
//  A menubar could be customized through the AddMenu, AddButton methods.
//  Note that there are corresponding Geant4 commands to add a 
// menus in the menubar and add buttons in a menu.
//  Ex : 
//    /gui/addMenu   test Test
//    /gui/addButton test Init /run/initialize
//    /gui/addButton test "Set gun" "/control/execute gun.g4m"
//    /gui/addButton test "Run one event" "/run/beamOn 1"
//
//  Command completion, by typing "tab" key, is available on the 
// command line.
//
// Class description - end :

class G4QTabWidget : public QTabWidget {
public :
  G4QTabWidget();
  G4QTabWidget(QSplitter*&);
  void paintEvent  ( QPaintEvent * event );
  inline void setTabSelected(bool a) { tabSelected = a; };
  inline void setLastTabCreated(int a) { lastCreated = a; };
  inline bool isTabSelected() { return tabSelected; };
  bool tabSelected;
  int lastCreated;
};

class G4UIQt : public QObject, public G4VBasicShell, public G4VInteractiveSession {
  Q_OBJECT

public: // With description
  G4UIQt(int,char**);
  // (argv, argc) or (0, NULL) had to be given.
  G4UIsession* SessionStart();
  // To enter interactive X loop ; waiting/executing command,...
  void AddMenu(const char*,const char*);
  // To add a pulldown menu in the menu bar. 
  // First argument is the name of the menu.
  // Second argument is the label of the cascade button.
  // Ex : AddMenu("my_menu","My menu")
  void AddButton(const char*,const char*,const char*);
  // To add a push button in a pulldown menu.
  // First argument is the name of the menu.
  // Second argument is the label of the button.
  // Third argument is the Geant4 command executed when the button is fired.
  // Ex : AddButton("my_menu","Run","/run/beamOn 1"); 

  bool AddTabWidget(QWidget*,QString,int,int);
  // To add a tab for vis openGL Qt driver
  
  QWidget* GetViewComponentsTBWidget();
  // Get the viewComponent

  bool IsSplitterReleased();

public:
  ~G4UIQt();
  void Prompt(G4String);
  void SessionTerminate();
  void PauseSessionStart(G4String);
  G4int ReceiveG4cout(G4String);
  G4int ReceiveG4cerr(G4String);
  //   G4String GetCommand(Widget);

private:
  void SecondaryLoop(G4String); // a VIRER
  void CreateHelpWidget();
  void InitHelpTree();
  void FillHelpTree();
  void ExitHelp();

  void CreateChildTree(QTreeWidgetItem*,G4UIcommandTree*);
  QTreeWidgetItem* FindTreeItem(QTreeWidgetItem *,const QString&);

  QString GetCommandList(const G4UIcommand*);

  G4bool GetHelpChoice(G4int&) ;// have to be implemeted because we heritate from G4VBasicShell
  bool eventFilter(QObject*,QEvent*);
  void ActivateCommand(G4String);
  QMap<int,QString> LookForHelpStringInChildTree(G4UIcommandTree *,const QString&);

  void CreateVisParametersTBWidget();
  void CreateHelpTBWidget();
  void CreateCoutTBWidget();
  void CreateHistoryTBWidget();
  void OpenHelpTreeOnCommand(const QString &);
  QString GetShortCommandPath(QString);
  QString GetLongCommandPath(QTreeWidgetItem*);

private:

  //  Widget form,shell,command,menuBar,text;
  //  std::map<QAction*,G4String, std::less<QAction*> > commands;
  //  static void commandEnteredCallback(Widget,XtPointer,XtPointer);
  //  static void keyHandler(Widget,XtPointer,XEvent*,Boolean*);
  QMainWindow * fMainWindow;
  QLabel *fCommandLabel;
  QLineEdit * fCommandArea;
  QTextEdit *fCoutTBTextArea;
  QTextEdit *fHelpArea;
  QToolBox* fToolBox;
  QStringList fG4cout;
  QLineEdit * fCoutFilter;

  QListWidget *fHistoryTBTableList;
  QTreeWidget *fHelpTreeWidget;
  QWidget* fHelpTBWidget;
  QWidget* fHistoryTBWidget;
  QWidget* fCoutTBWidget;
  QWidget* fVisParametersTBWidget;
  QWidget* fViewComponentsTBWidget;
  QLineEdit* fHelpLine;
  G4QTabWidget* fTabWidget;
  QString fCoutText;
  QLabel *fEmptyViewerTabLabel;
  QSplitter * fMyVSplitter;
  QSplitter * fHelpVSplitter;
  int fLastQTabSizeX;
  int fLastQTabSizeY;

private Q_SLOTS :
  void ExitSession();
  void ClearButtonCallback();
  void CommandEnteredCallback();
  void ButtonCallback(const QString&);
  void HelpTreeClicCallback();
  void HelpTreeDoubleClicCallback();
  void ShowHelpCallback();
  void CommandHistoryCallback();
  void LookForHelpStringCallback();
  void UpdateTabWidget(int);
  void ResizeTabWidget( QResizeEvent* );
  void CoutFilterCallback(const QString&);
  void TabCloseCallback(int);
  void ToolBoxActivated(int);
};

#endif

#endif

