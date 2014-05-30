/*
 * Copyright (C) 2013 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include <iostream>
#include <cxxtools/eventloop.h>
#include <cxxtools/timer.h>
#include <cxxtools/timespan.h>
#include <cxxtools/clock.h>
#include <cxxtools/log.h>

log_define("timertest")

void onInterval()
{
  static unsigned count = 0;
  ++count;
  log_info("interval " << count);
}

void onIntervalFullSecond()
{
  static unsigned count = 0;
  ++count;
  log_info("interval full second " << count);
}

void onIntervalFuture()
{
  static unsigned count = 0;
  ++count;
  log_info("delayed start interval second " << count);
}

void onIntervalFutureFullSecond()
{
  static unsigned count = 0;
  ++count;
  log_info("future full second interval " << count);
}

void onIntervalPast()
{
  static unsigned count = 0;
  ++count;
  log_info("late start " << count);
}

void onOneShot()
{
  log_info("one shot");
}

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::LogConfiguration logConfiguration;
    logConfiguration.setRootLevel(cxxtools::Logger::info);
    logConfiguration.setLogLevel("cxxtools.timer", cxxtools::Logger::debug);
    log_init(logConfiguration);

    cxxtools::EventLoop loop;

    cxxtools::Timer oneShotTimer(&loop);

    // timer tick once per second
    cxxtools::Timer intervalTimer(&loop);
    intervalTimer.start(1000);
    cxxtools::connect(intervalTimer.timeout, onInterval);

    // timer tick once per second but start at full second
    cxxtools::Timer intervalFullSecondTimer(&loop);
    intervalFullSecondTimer.start(cxxtools::DateTime(1970, 1, 1, 0, 0, 0), 1000);
    cxxtools::connect(intervalFullSecondTimer.timeout, onIntervalFullSecond);

    // timer tick once per second but start after 5 seconds
    cxxtools::Timer intervalFutureTimer(&loop);
    intervalFutureTimer.start(cxxtools::Clock::getLocalTime() + cxxtools::Timespan(5, 0), 1000);
    cxxtools::connect(intervalFutureTimer.timeout, onIntervalFuture);

    // timer tick once per second but start after 2 seconds at full second
    cxxtools::Timer intervalFutureFullSecondTimer(&loop);

    cxxtools::DateTime dt = cxxtools::Clock::getLocalTime() + cxxtools::Timespan(2, 0);
    int year;
    unsigned month, day, hour, min, sec, msec;
    dt.get(year, month, day, hour, min, sec, msec);
    dt.set(year, month, day, hour, min, sec, 0);

    intervalFutureFullSecondTimer.start(dt, 1000);
    cxxtools::connect(intervalFutureFullSecondTimer.timeout, onIntervalFutureFullSecond);

    // timer, which was started in the past
    cxxtools::Timer intervalPastTimer(&loop);
    intervalPastTimer.start(cxxtools::DateTime(1990, 1, 1, 8, 0, 0), 1000);
    cxxtools::connect(intervalPastTimer.timeout, onIntervalPast);

    // tick just once after 1500 ms
    oneShotTimer.after(1500);
    cxxtools::connect(oneShotTimer.timeout, onOneShot);

    log_info("start event loop");
    loop.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

