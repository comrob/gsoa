/*
 * File name: logging.h
 * Date:      2005/10/09 15:09
 * Author:    Jan Faigl	
 */

#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <log4cxx/logger.h>

namespace crl {

typedef log4cxx::Logger CLogger;
typedef log4cxx::LoggerPtr CLoggerPtr;

#define INFO(str) LOG4CXX_INFO(logger, str);
#define WARN(str) LOG4CXX_WARN(logger, str);
#define ERROR(str) LOG4CXX_ERROR(logger, str);
#define DEBUG(str) LOG4CXX_DEBUG(logger, str);

#define INFO_M(logger, str) LOG4CXX_INFO(logger, str);
#define WARN_M(logger, str) LOG4CXX_WARN(logger, str);
#define ERROR_M(logger, str) LOG4CXX_ERROR(logger, str);
#define DEBUG_M(logger, str) LOG4CXX_DEBUG(logger, str);

#define MEM_CHECK(ptr, logger)                                                 \
   {                                                                           \
      if (!ptr) {                                                              \
         LOG4CXX_FATAL(logger, "Couldn't allocate memory");                    \
         exit(-2);                                                             \
      }                                                                        \
   }

#define PTR_CHECK(ptr, logger)                                                 \
   {                                                                           \
      if (!ptr) {                                                              \
         LOG4CXX_FATAL(logger, "Null pointer");                                \
         exit(-2);                                                             \
      }                                                                        \
   }

inline void warn(CLoggerPtr logger, std::string msg)
{
   LOG4CXX_WARN(logger, msg);
}

inline void error(CLoggerPtr logger, std::string msg)
{
   LOG4CXX_WARN(logger, msg);
}

void initLogger(const std::string &loggerName);

void initLogger(const char *loggerName);

void initLogger(const char *loggerName, const char *configName);

CLoggerPtr getLogger(const char *loggerName);

void shutdownLogger(void);

// global available logger
extern CLoggerPtr logger;

} // namespace crl

#endif

/* end of logging.h */
