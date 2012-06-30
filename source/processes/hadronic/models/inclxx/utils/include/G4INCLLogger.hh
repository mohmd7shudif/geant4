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
// INCL++ intra-nuclear cascade model
// Pekka Kaitaniemi, CEA and Helsinki Institute of Physics
// Davide Mancusi, CEA
// Alain Boudard, CEA
// Sylvie Leray, CEA
// Joseph Cugnon, University of Liege
//
// INCL++ revision: v5.0_rc3
//
#define INCLXX_IN_GEANT4_MODE 1

#include "globals.hh"

#ifndef G4INCLLogger_hh
#define G4INCLLogger_hh 1

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

namespace G4INCL {

  /**
   * Verbosity scale from 0 (fatal errors only) to 10 (prG4int everything)
   */
  enum MessageType { InfoMsg = 1,
    FatalMsg = 2,
    ErrorMsg = 3,
    WarningMsg = 4,
    DebugMsg = 7,
    DataBlockMsg = 10,
    ZeroMsg = 0 };

#ifdef INCL_DEBUG_LOG

  class LoggerSlave {
    public:
      // By default, log fatal errors, errors and warnings
      LoggerSlave(std::string const &logFileName) : logStream(0), verbosityLevel(4) {
        if(logFileName=="-") {
#ifdef INCLXX_IN_GEANT4_MODE
	  logStream = &(G4cout);
#else
          logStream = &(std::cout);
#endif
          logToStdout = true;
        }
        else {
          logToStdout = false;
          logStream = new std::ofstream(logFileName.c_str());
          if(!logStream)
          {
            std::cerr << "Fatal error: couldn't open log file " << logFileName << std::endl;
            std::exit(EXIT_FAILURE);
          }
        }

#ifndef INCLXX_IN_GEANT4_MODE
        std::log(InfoMsg, __FILE__,__LINE__) << "# Logging enabled!" << std::endl;
#endif
      };
      ~LoggerSlave() {
        if(!logToStdout)
          delete logStream;
      };

      /**
       * Set the verbosity level
       */
      void setVerbosityLevel(G4int lvl) { verbosityLevel = lvl; }

      /**
       * Get the verbosity level
       */
      G4int getVerbosityLevel() { return verbosityLevel; }

      /// \brief Write the log message.
      LoggerSlave const &log(const MessageType type, const std::string &fileName, const G4int lineNumber) const;

      /// \brief Log a data block.
      void logDataBlock(const std::string &block, const std::string &fileName, const G4int lineNumber) const;

      typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
      typedef CoutType& (*StandardEndLine)(CoutType&);
      /// \brief Overload << operator to support std::endl.
      LoggerSlave const &operator<<(StandardEndLine const &manip) const {
        manip(*logStream);
        return *this;
      }

      /// \brief Overloaded << operator to provide a stream-like API.
      template<typename T>
        LoggerSlave const &operator<<(const T &t) const {
          (*logStream) << t;
          return *this;
        }

    private:
      std::ostream *logStream;
      G4int verbosityLevel;
      G4bool logToStdout;
  };

  class Logger {
    public:
      /// \brief Log a message.
      static LoggerSlave const &log(const MessageType type, const std::string &fileName, const G4int lineNumber) {
        return theLoggerSlave->std::log(type, fileName, lineNumber);
      }

      /// \brief Log a data block.
      static void dataBlock(const std::string &block, const std::string &fileName, const G4int lineNumber) {
        theLoggerSlave->logDataBlock(block, fileName, lineNumber);
      }

      /// \brief Set the slave Logger.
      static void setLoggerSlave(LoggerSlave * const logger) { theLoggerSlave = logger; }

      /// \brief Set the verbosity of the slave Logger.
      static void setVerbosityLevel(G4int lvl) { theLoggerSlave->setVerbosityLevel(lvl); }

      /// \brief Get the verbosity of the slave Logger.
      static G4int getVerbosityLevel() { return theLoggerSlave->getVerbosityLevel(); }

      /// \brief Delete the slave Logger.
      static void deleteLoggerSlave() { delete theLoggerSlave; }

    private:
      static LoggerSlave *theLoggerSlave;
  };

  // Macro definitions for line numbering in log files!
#define FATAL(x) \
  if(G4INCL::FatalMsg <= G4INCL::Logger::getVerbosityLevel())\
  G4INCL::Logger::log(G4INCL::FatalMsg, __FILE__,__LINE__) << x;\
  else (void)0
#define ERROR(x) \
  if(G4INCL::ErrorMsg <= G4INCL::Logger::getVerbosityLevel())\
  G4INCL::Logger::log(G4INCL::ErrorMsg, __FILE__,__LINE__) << x;\
  else (void)0
#define WARN(x) \
  if(G4INCL::WarningMsg <= G4INCL::Logger::getVerbosityLevel())\
  G4INCL::Logger::log(G4INCL::WarningMsg, __FILE__,__LINE__) << x;\
  else (void)0
#define INFO(x) \
  if(G4INCL::InfoMsg <= G4INCL::Logger::getVerbosityLevel())\
  G4INCL::Logger::log(G4INCL::InfoMsg, __FILE__,__LINE__) << x;\
  else (void)0
#define DEBUG(x) \
  if(G4INCL::DebugMsg <= G4INCL::Logger::getVerbosityLevel())\
  G4INCL::Logger::log(G4INCL::DebugMsg, __FILE__,__LINE__) << x;\
  else (void)0
#define DATABLOCK(x) \
  if(G4INCL::DataBlockMsg <= G4INCL::Logger::getVerbosityLevel())\
  G4INCL::Logger::dataBlock(x,__FILE__,__LINE__);\
  else (void)0

#else
  // Empty logger for normal (production) use:
  class LoggerSlave {
  public:
    LoggerSlave(std::string const &) {};
    LoggerSlave() {};
    ~LoggerSlave() {};
    void setVerbosityLevel(G4int) {};
  };

  class Logger {
  public:
    Logger() {};
    ~Logger() {};
    static void setVerbosityLevel(G4int) {};
    static void setLoggerSlave(LoggerSlave * const slave) { theLoggerSlave = slave; }
    static void deleteLoggerSlave() { delete theLoggerSlave; }
  private:
    static LoggerSlave *theLoggerSlave;
  };

#define FATAL(x);
#define ERROR(x);
#define WARN(x);
#define INFO(x);
#define DEBUG(x);
#define DATABLOCK(x);

#endif
}
#endif
