/* log.cpp
   Copyright (C) 2003 Tommi Maekitalo

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

#include <cxxtools/log.h>

#ifdef CXXTOOLS_USE_LOG4CXX

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/helpers/stringhelper.h>
#include <log4cxx/xml/domconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/logmanager.h>

void log_init()
{
  log4cxx::BasicConfigurator::configure();
}

void log_init(log4cxx::LevelPtr level)
{
  log4cxx::BasicConfigurator::configure();
  log4cxx::LoggerPtr rootLogger = log4cxx::Logger::getRootLogger();

  log4cxx::LogManager::getLoggerRepository()->setThreshold(level);
}

void log_init(const std::string& configFileName)
{
  if (log4cxx::helpers::StringHelper::endsWith(configFileName, _T(".xml")))
  {
    ::log4cxx::xml::DOMConfigurator::configure(configFileName);
  }
  else
  {
    ::log4cxx::PropertyConfigurator::configure(configFileName);
  }
}

#endif

#ifdef CXXTOOLS_USE_LOG4CPLUS

#include <log4cplus/configurator.h>
#include <log4cplus/consoleappender.h>

void log_init()
{
  char* LOG4CPLUS = ::getenv("LOG4CPLUS");
  if (LOG4CPLUS)
    log_init(LOG4CPLUS);
  else
    log_init(log4cplus::ERROR_LOG_LEVEL);
}

void log_init(log4cplus::LogLevel level)
{
  log4cplus::SharedAppenderPtr appender(new log4cplus::ConsoleAppender(true, true));
  appender->setName("Main");

  log4cplus::Logger root = log4cplus::Logger::getRoot();
  root.addAppender(appender);
  root.setLogLevel(level);
}

void log_init(const std::string& propertyfilename)
{
  log4cplus::PropertyConfigurator logconfig(propertyfilename);
  logconfig.configure();
}

#endif

#ifdef CXXTOOLS_USE_LOGSTDOUT

#include <cxxtools/loginit.h>

namespace cxxtools
{
  log_level_type log_level = INFO_LOG_LEVEL;
}

#endif
