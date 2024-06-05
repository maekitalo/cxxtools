#include <iostream>
#include <cxxtools/eventloop.h>
#include <cxxtools/timer.h>
#include <cxxtools/timespan.h>
#include <cxxtools/clock.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

log_define("timertest")

int main(int argc, char* argv[])
{
  try
  {
    // the command line switch -d enables debug log for the selector
    cxxtools::Arg<bool> debug(argc, argv, 'd');

    // with -l <filename> we may pass a own logging configuration
    // the configuration may be xml or properties format
    cxxtools::Arg<std::string> logfile(argc, argv, 'l');

    // initialize logging
    if (logfile.isSet())
    {
      log_init(logfile);
    }
    else
    {
      // when no logfile is passed, we configure the logging without it
      cxxtools::LogConfiguration logConfiguration;
      logConfiguration.setRootLevel(cxxtools::Logger::LOG_LEVEL_INFO);
      logConfiguration.setLogLevel("cxxtools.timer", cxxtools::Logger::LOG_LEVEL_DEBUG);
      if (debug)
        logConfiguration.setLogLevel("cxxtools.selector.impl", cxxtools::Logger::LOG_LEVEL_DEBUG);
      log_init(logConfiguration);
    }

    // we need a event loop which controls the timers
    cxxtools::EventLoop loop;

    // timer to tick just once after 1500 ms
    cxxtools::Timer oneShotTimer(&loop);
    oneShotTimer.after(cxxtools::Milliseconds(1500));
    cxxtools::connect(oneShotTimer.timeout, [](){
        log_info("one shot");
    });

    // timer tick once per second
    cxxtools::Timer intervalTimer(&loop);
    intervalTimer.start(cxxtools::Seconds(1));
    cxxtools::connect(intervalTimer.timeout, [] {
        static unsigned count = 0;
        ++count;
        log_info("interval " << count);
    });

    // timer tick once per second but start at full second
    cxxtools::Timer intervalFullSecondTimer(&loop);
    intervalFullSecondTimer.start(cxxtools::DateTime(1970, 1, 1, 0, 0, 0), cxxtools::Seconds(1));
    cxxtools::connect(intervalFullSecondTimer.timeout, []() {
        static unsigned count = 0;
        ++count;
        log_info("interval full second " << count);
    });

    // timer tick once per second but start after 5 seconds
    cxxtools::Timer intervalFutureTimer(&loop);
    intervalFutureTimer.start(cxxtools::Clock::getLocalTime() + cxxtools::Seconds(5), cxxtools::Seconds(1));
    cxxtools::connect(intervalFutureTimer.timeout, []() {
        static unsigned count = 0;
        ++count;
        log_info("delayed start interval second " << count);
    });

    // timer tick once per second but start after 2 seconds at full second
    cxxtools::Timer intervalFutureFullSecondTimer(&loop);

    cxxtools::DateTime dt = cxxtools::Clock::getLocalTime() + cxxtools::Seconds(2);
    int year;
    unsigned month, day, hour, min, sec, msec;
    dt.get(year, month, day, hour, min, sec, msec);
    dt.set(year, month, day, hour, min, sec, 0);

    intervalFutureFullSecondTimer.start(dt, cxxtools::Seconds(1));
    cxxtools::connect(intervalFutureFullSecondTimer.timeoutts, [](cxxtools::DateTime ts) {
        static unsigned count = 0;
        ++count;
        log_info("future full second interval " << count << ' ' << ts.toString());
    });

    // timer, which was started in the past - it just starts ticking immediately
    cxxtools::Timer intervalPastTimer(&loop);
    intervalPastTimer.start(cxxtools::DateTime(1990, 1, 1, 8, 0, 0), cxxtools::Seconds(1));
    cxxtools::connect(intervalPastTimer.timeoutts, [](cxxtools::DateTime ts) {
        static unsigned count = 0;
        ++count;
        log_info("late start " << count << ' ' << ts.toString());
    });
    cxxtools::connect(intervalPastTimer.timeoutUtc, [](cxxtools::DateTime ts) {
        static unsigned count = 0;
        ++count;
        log_info("utc " << ts.toString());
    });

    // now we start the event loop to execute the timers
    // this will loop forever and call the timer callback functions at suitable times
    log_info("start event loop");
    loop.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

