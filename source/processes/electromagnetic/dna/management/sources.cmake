#------------------------------------------------------------------------------
# sources.cmake
# Module : G4emlowenergy
# Package: Geant4.src.G4processes.G4electromagnetic.G4emlowenergy
#
# Sources description for a library.
# Lists the sources and headers of the code explicitely.
# Lists include paths needed.
# Lists the internal granular and global dependencies of the library.
# Source specific properties should be added at the end.
#
# Generated on : 24/9/2010
#
# $Id: sources.cmake,v 1.4 2010-11-15 08:24:43 gcosmo Exp $
#
#------------------------------------------------------------------------------

# List external includes needed.
include_directories(${CLHEP_INCLUDE_DIRS})

# List internal includes needed.
include_directories(${CMAKE_SOURCE_DIR}/source/global/HEPGeometry/include)
include_directories(${CMAKE_SOURCE_DIR}/source/global/HEPRandom/include)
include_directories(${CMAKE_SOURCE_DIR}/source/global/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/geometry/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/geometry/magneticfield/include)
include_directories(${CMAKE_SOURCE_DIR}/source/geometry/navigation/include)
include_directories(${CMAKE_SOURCE_DIR}/source/geometry/volumes/include)
include_directories(${CMAKE_SOURCE_DIR}/source/particles/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/cuts/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/electromagnetic/utils/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/track/include)
include_directories(${CMAKE_SOURCE_DIR}/source/intercoms/include)
include_directories(${CMAKE_SOURCE_DIR}/source/materials/include)

#
# Define the Geant4 Module.
#
include(Geant4MacroDefineModule)
GEANT4_DEFINE_MODULE(NAME G4emdna-man
    HEADERS
        AddClone_def.hh
        G4AllITManager.hh
        G4ITBox.hh
        G4IT.hh
        G4ITManager.hh
        G4ITManager.icc
        G4ITModelHandler.hh
        G4ITModelManager.hh
        G4ITModelProcessor.hh
        G4ITReactionChange.hh
        G4ITReactionTable.hh
        G4ITStepManager.hh
        G4ITStepProcessor.hh
        G4ITTrackingManager.hh
        G4ITTransportation.hh
        G4ITTransportation.icc
        G4ITType.hh
        G4KDNode.hh
        G4KDTree.hh
        G4KDTreeResult.hh
        G4ReferenceCast.hh
        G4TrackingInformation.hh
        G4TrackList.hh
        G4UserReactionAction.hh
        G4VITModel.hh
        G4VITProcess.hh
        G4VITReactionProcess.hh
        G4VITRestDiscreteProcess.hh
        G4VITRestProcess.hh
        G4VITTimeStepper.hh
    SOURCES
        G4AllITManager.cc
        G4ITBox.cc
        G4IT.cc
        G4ITManager_allbox_iterator.cc
        G4ITManager.cc
        G4ITManager_const_iterator.cc
        G4ITManager_iterator.cc
        G4ITModelHandler.cc
        G4ITModelManager.cc
        G4ITModelProcessor.cc
        G4ITReactionChange.cc
        G4ITReactionTable.cc
        G4ITStepManager.cc
        G4ITStepProcessor2.cc
        G4ITStepProcessor.cc
        G4ITTrackingManager.cc
        G4ITTransportation.cc
        G4ITType.cc
        G4KDNode.cc
        G4KDTree.cc
        G4KDTreeResult.cc
        G4TrackingInformation.cc
        G4TrackList.cc
        G4UserReactionAction.cc
        G4VITModel.cc
        G4VITProcess.cc
        G4VITReactionProcess.cc
        G4VITRestDiscreteProcess.cc
        G4VITRestProcess.cc
        G4VITTimeStepper.cc
    GRANULAR_DEPENDENCIES
        G4detector
        G4geometrymng
        G4magneticfield
        G4globman
        G4intercoms
        G4materials
        G4navigation
        G4volumes
        G4partman
        G4procman
        G4cuts
        G4emutils
    GLOBAL_DEPENDENCIES
        G4geometry
        G4global
        G4intercoms
        G4materials
        G4particles
        G4track
    LINK_LIBRARIES
)

# List any source specific properties here

