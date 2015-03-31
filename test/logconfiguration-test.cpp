/*
 * Copyright (C) 2015 Tommi Maekitalo
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "cxxtools/log.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include <sstream>
#include <cxxtools/properties.h>

log_define("cxxtools.test.logconfiguration")

class LogconfigurationTest : public cxxtools::unit::TestSuite
{
  public:
    LogconfigurationTest()
    : cxxtools::unit::TestSuite("logconfiguration")
    {
      registerMethod("logLevelTest", *this, &LogconfigurationTest::logLevelTest);
      registerMethod("logLevelWithTraceTest", *this, &LogconfigurationTest::logLevelWithTraceTest);
      registerMethod("logSingleFlagTest", *this, &LogconfigurationTest::logSingleFlagTest);
      registerMethod("logFlagsTest", *this, &LogconfigurationTest::logFlagsTest);
      registerMethod("rootLevelTest", *this, &LogconfigurationTest::rootLevelTest);
      registerMethod("hierachicalTest", *this, &LogconfigurationTest::hierachicalTest);
    }

    void logLevelTest();
    void logLevelWithTraceTest();
    void logSingleFlagTest();
    void logFlagsTest();
    void rootLevelTest();
    void hierachicalTest();
};

void LogconfigurationTest::logLevelTest()
{
  std::istringstream properties(
    "rootlogger=WARN\n"
    "logger.fatal=FATAL\n"
    "logger.error=ERROR\n"
    "logger.warn=WARN\n"
    "logger.info=INFO\n"
    "logger.debug=DEBUG\n"
    "logger.fine=FINE\n"
    "logger.finer=FINER\n"
    "logger.finest=FINEST\n"
    "logger.trace=TRACE\n");

  cxxtools::LogConfiguration config;
  properties >> cxxtools::Properties(config);

  cxxtools::Logger rootLogger("", config.rootFlags());
  CXXTOOLS_UNIT_ASSERT(rootLogger.isEnabled(cxxtools::Logger::LOG_WARN));
  CXXTOOLS_UNIT_ASSERT(rootLogger.isEnabled(cxxtools::Logger::LOG_ERROR));
  CXXTOOLS_UNIT_ASSERT(!rootLogger.isEnabled(cxxtools::Logger::LOG_INFO));

  cxxtools::Logger fatalLogger("fatal", config.logFlags("fatal"));
  CXXTOOLS_UNIT_ASSERT(fatalLogger.isEnabled(cxxtools::Logger::LOG_FATAL));
  CXXTOOLS_UNIT_ASSERT(!fatalLogger.isEnabled(cxxtools::Logger::LOG_ERROR));

  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("fatal"),  0x01);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("error"),  0x03);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("warn"),   0x07);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("info"),   0x0f);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("debug"),  0x1f);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("fine"),   0x1f);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("finer"),  0x3f);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("finest"), 0x7f);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("trace"),  0xff);
}

void LogconfigurationTest::logLevelWithTraceTest()
{
  std::istringstream properties(
    "rootlogger=WARN\n"
    "logger.fatal=TFATAL\n"
    "logger.error=TERROR\n"
    "logger.warn=TWARN\n"
    "logger.info=TINFO\n"
    "logger.debug=TDEBUG\n"
    "logger.fine=TFINE\n"
    "logger.finer=TFINER\n"
    "logger.finest=TFINEST\n");

  cxxtools::LogConfiguration config;
  properties >> cxxtools::Properties(config);

  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("fatal"),  0x81);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("error"),  0x83);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("warn"),   0x87);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("info"),   0x8f);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("debug"),  0x9f);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("fine"),   0x9f);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("finer"),  0xbf);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("finest"), 0xff);
}

void LogconfigurationTest::logSingleFlagTest()
{
  std::istringstream properties(
    "rootlogger=WARN\n"
    "logger.fatal=|FATAL\n"
    "logger.error=|ERROR\n"
    "logger.warn=|WARN\n"
    "logger.info=|INFO\n"
    "logger.debug=|DEBUG\n"
    "logger.fine=|FINE\n"
    "logger.finer=|FINER\n"
    "logger.finest=|FINEST\n"
    "logger.trace=|TRACE\n");

  cxxtools::LogConfiguration config;
  properties >> cxxtools::Properties(config);

  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("fatal"),  0x01);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("error"),  0x02);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("warn"),   0x04);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("info"),   0x08);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("debug"),  0x10);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("fine"),   0x10);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("finer"),  0x20);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("finest"), 0x40);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("trace"),  0x80);
}

void LogconfigurationTest::logFlagsTest()
{
  std::istringstream properties(
    "rootlogger=WARN\n"
    "logger.cat1=WARN|INFO\n"
    "logger.cat2=TRACE|D\n"
    "logger.cat3=I|T\n");

  cxxtools::LogConfiguration config;
  properties >> cxxtools::Properties(config);

  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("cat1"), cxxtools::Logger::LOG_WARN | cxxtools::Logger::LOG_INFO);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("cat2"), cxxtools::Logger::LOG_DEBUG | cxxtools::Logger::LOG_TRACE);
  CXXTOOLS_UNIT_ASSERT_EQUALS(config.logFlags("cat3"), cxxtools::Logger::LOG_INFO | cxxtools::Logger::LOG_TRACE);
}

void LogconfigurationTest::rootLevelTest()
{
  std::istringstream properties(
    "rootlogger=WARN\n"
    "logger.cat1=INFO\n");

  cxxtools::LogConfiguration config;
  properties >> cxxtools::Properties(config);

  cxxtools::Logger cat1("cat1", config.logFlags("cat1"));
  cxxtools::Logger cat2("cat2", config.logFlags("cat2"));

  CXXTOOLS_UNIT_ASSERT(cat1.isEnabled(cxxtools::Logger::LOG_WARN));
  CXXTOOLS_UNIT_ASSERT(cat1.isEnabled(cxxtools::Logger::LOG_INFO));
  CXXTOOLS_UNIT_ASSERT(!cat1.isEnabled(cxxtools::Logger::LOG_DEBUG));

  CXXTOOLS_UNIT_ASSERT(cat2.isEnabled(cxxtools::Logger::LOG_WARN));
  CXXTOOLS_UNIT_ASSERT(!cat2.isEnabled(cxxtools::Logger::LOG_INFO));
  CXXTOOLS_UNIT_ASSERT(!cat2.isEnabled(cxxtools::Logger::LOG_DEBUG));
}

void LogconfigurationTest::hierachicalTest()
{
  std::istringstream properties(
    "rootlogger=WARN\n"
    "logger.cat1=INFO\n"
    "logger.cat1.sub1=ERROR\n");

  cxxtools::LogConfiguration config;
  properties >> cxxtools::Properties(config);

  cxxtools::Logger cat1("cat1", config.logFlags("cat1"));
  cxxtools::Logger sub1("sub1", config.logFlags("cat1.sub1"));
  cxxtools::Logger sub2("sub2", config.logFlags("cat1.sub2"));

  CXXTOOLS_UNIT_ASSERT(cat1.isEnabled(cxxtools::Logger::LOG_WARN));
  CXXTOOLS_UNIT_ASSERT(cat1.isEnabled(cxxtools::Logger::LOG_INFO));
  CXXTOOLS_UNIT_ASSERT(!cat1.isEnabled(cxxtools::Logger::LOG_DEBUG));

  CXXTOOLS_UNIT_ASSERT(!sub1.isEnabled(cxxtools::Logger::LOG_WARN));
  CXXTOOLS_UNIT_ASSERT(!sub1.isEnabled(cxxtools::Logger::LOG_INFO));
  CXXTOOLS_UNIT_ASSERT(!sub1.isEnabled(cxxtools::Logger::LOG_DEBUG));

  CXXTOOLS_UNIT_ASSERT(sub2.isEnabled(cxxtools::Logger::LOG_WARN));
  CXXTOOLS_UNIT_ASSERT(!sub2.isEnabled(cxxtools::Logger::LOG_INFO));
  CXXTOOLS_UNIT_ASSERT(!sub2.isEnabled(cxxtools::Logger::LOG_DEBUG));

}


cxxtools::unit::RegisterTest<LogconfigurationTest> register_LogconfigurationTest;
