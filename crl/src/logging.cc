/*
 * File name: logging.cc
 * Date:      2005/10/09 15:18
 * Author:    Jan Faigl
 */

#include <cstdlib>
#include <iostream>

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/propertyconfigurator.h>

#include <crl/logging.h>

namespace crl {

CLoggerPtr logger = 0;

void initLogger(const std::string &loggerName)
{
   initLogger(loggerName.c_str());
}
void initLogger(const char *loggerName)
{
   if (logger) {
      std::cerr << "main logger already configured" << std::endl;
   } else {
      logger = getLogger(loggerName);
      try {
         log4cxx::BasicConfigurator::configure();
      }
      catch (log4cxx::helpers::Exception &e) {
         std::cerr << "Error : log4cxx::BasicConfigurator" << std::endl;
      }
      atexit(shutdownLogger);
   }
}

void initLogger(const char *loggerName, const char *configName)
{
   // same as above but use config file
   logger = getLogger(loggerName);
   log4cxx::PropertyConfigurator::configure(configName);
}

CLoggerPtr getLogger(const char *loggerName)
{
   CLoggerPtr logger = log4cxx::Logger::getLogger(loggerName);
   return logger;
}

void shutdownLogger(void) { log4cxx::LogManager::shutdown(); }

} // namespace crl

/* end of logging.cc */
