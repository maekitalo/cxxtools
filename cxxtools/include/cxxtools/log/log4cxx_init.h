/* cxxtools/log/log4cxx_init.h
   Copyright (C) 2004 Tommi Maekitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef CXXTOOLS_LOG_LOG4CXX_INIT_H
#define CXXTOOLS_LOG_LOG4CXX_INIT_H

#include <cxxtools/log/log4cxx.h>

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/helpers/stringhelper.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/xml/domconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/logmanager.h>

#define log_init_fatal()   log_init(::log4cxx::Level::FATAL)
#define log_init_error()   log_init(::log4cxx::Level::ERROR)
#define log_init_warn()    log_init(::log4cxx::Level::WARN)
#define log_init_info()    log_init(::log4cxx::Level::INFO)
#define log_init_debug()   log_init(::log4cxx::Level::DEBUG)
#define log_init_trace()   log_init(::log4cxx::Level::DEBUG)

inline void log_init(const std::string& configFileName)
{
#ifdef LOG4CXX_HAVE_XML
  if (log4cxx::helpers::StringHelper::endsWith(configFileName, ".xml"))
  {
    ::log4cxx::xml::DOMConfigurator::configure(configFileName);
  }
  else
#endif
  {
    ::log4cxx::PropertyConfigurator::configure(configFileName);
  }
}

inline void log_init(const char* filename)
{
  log_init(std::string(filename));
}

inline void log_init()
{
  char* LOGPROPERTIES = ::getenv("LOGPROPERTIES");
  if (LOGPROPERTIES)
    log_init(LOGPROPERTIES);
  else
  {
    try
    {
      log_init("log4j.properties");
    }
    catch (const log4cxx::helpers::IOException&)
    {
      log4cxx::BasicConfigurator::configure();
    }
  }
}

inline void log_init(log4cxx::LevelPtr level)
{
  log4cxx::BasicConfigurator::configure();
  log4cxx::LoggerPtr rootLogger = log4cxx::Logger::getRootLogger();
  rootLogger->setLevel(level);
}

#endif // CXXTOOLS_LOG_LOG4CXX_INIT_H
