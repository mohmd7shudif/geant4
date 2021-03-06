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
// $Id: G4OpenGLStoredXViewer.cc,v 1.27 2010-06-03 08:26:55 allison Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// Andrew Walkden  7th February 1997
// Class G4OpenGLStoredXViewer : a class derived from G4OpenGLXViewer and
//                             G4OpenGLStoredViewer.

#ifdef G4VIS_BUILD_OPENGLX_DRIVER

#include "G4OpenGLStoredXViewer.hh"

#include "G4ios.hh"

G4OpenGLStoredXViewer::
G4OpenGLStoredXViewer (G4OpenGLStoredSceneHandler& sceneHandler,
		 const G4String& name)
 : G4VViewer (sceneHandler, sceneHandler.IncrementViewCount (), name),
   G4OpenGLViewer (sceneHandler),
   G4OpenGLXViewer (sceneHandler),
   G4OpenGLStoredViewer (sceneHandler)
{
  if (fViewId < 0) return;  // In case error in base class instantiation.

  if (!vi_stored) {
    fViewId = -1;  // This flags an error.
    G4cerr << "G4OpenGLStoredXViewer::G4OpenGLStoredXViewer -"
      " G4OpenGLXViewer couldn't get a visual." << G4endl;
    return;
  }
}

G4OpenGLStoredXViewer::~G4OpenGLStoredXViewer () {}

void G4OpenGLStoredXViewer::Initialise () {

#ifdef G4DEBUG_VIS_OGL
  printf("G4OpenGLStoredXViewer::Initialise\n");
#endif
  CreateGLXContext (vi_stored);
  CreateMainWindow ();
  CreateFontLists ();

  InitializeGLView ();

  glDrawBuffer (GL_BACK);
}

void G4OpenGLStoredXViewer::ShowView () {
  // Some X servers fail to draw all trajectories, particularly Mac
  // XQuartz.  Revisit this at a future date.  Meanwhile, issue an
  // extra...
  ClearView();
  DrawView();
  // ..then call parent method.
  G4OpenGLXViewer::ShowView();
}

void G4OpenGLStoredXViewer::DrawView () {

#ifdef G4DEBUG_VIS_OGL
  printf("G4OpenGLStoredXViewer::DrawView\n");
#endif
  //Make sure current viewer is attached and clean...
  glXMakeCurrent (dpy, win, cx);

  G4ViewParameters::DrawingStyle style = GetViewParameters().GetDrawingStyle();

  //See if things have changed from last time and remake if necessary...
  // The fNeedKernelVisit flag might have been set by the user in
  // /vis/viewer/rebuild, but if not, make decision and set flag only
  // if necessary...
  if (!fNeedKernelVisit) KernelVisitDecision ();
  G4bool kernelVisitWasNeeded = fNeedKernelVisit; // Keep (ProcessView resets).
  ProcessView ();

  if(style!=G4ViewParameters::hlr &&
     haloing_enabled) {

    HaloingFirstPass ();
    DrawDisplayLists ();
#ifdef G4DEBUG_VIS_OGL
  printf("G4OpenGLStoredXViewer::DrawView flush \n");
#endif
    glFlush ();

    HaloingSecondPass ();

    DrawDisplayLists ();
    FinishView ();

  } else {

    // If kernel visit was needed, drawing and FinishView will already
    // have been done, so...
    if (!kernelVisitWasNeeded) {
#ifdef G4DEBUG_VIS_OGL
      printf("G4OpenGLStoredXViewer::DrawView NO need kernel visit\n");
#endif
      DrawDisplayLists ();
      FinishView ();
    } else {
#ifdef G4DEBUG_VIS_OGL
      printf("G4OpenGLStoredXViewer::DrawView NEED kernel visit\n");
#endif
    // However, union cutaways are implemented in DrawDisplayLists, so make
    // an extra pass...
      if (fVP.IsCutaway() &&
	  fVP.GetCutawayMode() == G4ViewParameters::cutawayUnion) {
	ClearView();
	DrawDisplayLists ();
	FinishView ();
      } else { // ADD TO AVOID KernelVisit=1 and nothing to display
        DrawDisplayLists ();
        FinishView ();
      }
    }
  }
}

void G4OpenGLStoredXViewer::FinishView () {
#ifdef G4DEBUG_VIS_OGL
  printf("G4OpenGLStoredXViewer::FinishView\n");
#endif
  glXWaitGL (); //Wait for effects of all previous OpenGL commands to
                //be propogated before progressing.

#ifdef G4DEBUG_VIS_OGL
  printf("G4OpenGLStoredXViewer::FinishView flush \n");
#endif
  glFlush (); //FIXME

  GLint renderMode;
  glGetIntegerv(GL_RENDER_MODE, &renderMode);
  if (renderMode == GL_RENDER) glXSwapBuffers (dpy, win);  
}

#endif
