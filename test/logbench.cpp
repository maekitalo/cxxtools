/*
 * Copyright (C) 2003 Tommi Maekitalo
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

#include <cxxtools/arg.h>
#include <cxxtools/timespan.h>
#include <cxxtools/clock.h>
#include <cxxtools/log.h>

#include <iostream>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <memory>
#include <thread>

#include <sys/time.h>

namespace bench
{
    log_define("bench")

    class Logtester
    {
            std::thread _thread;
            unsigned long _count;
            unsigned long _loops;
            bool _enabled;

        public:
            Logtester(unsigned long count,
                                unsigned long loops,
                                unsigned long enabled)
                : _thread(&Logtester::run, this),
                  _count(count),
                  _loops(loops),
                  _enabled(enabled)
                  { }

            void join()
            { _thread.join(); }

            void setCount(unsigned long count)     { _count = count; }
            void setLoops(unsigned long loops)     { _loops = loops; }
            void setEnabled(bool sw = true)         { _enabled = sw; }

            void run();
    };

    void Logtester::run()
    {
        for (unsigned long l = 0; l < _loops; ++l)
        {
            if (_enabled)
                for (unsigned long i = 0; i < _count; ++i)
                    log_info("info message");
            else
                for (unsigned long i = 0; i < _count; ++i)
                    log_debug("debug message");
        }
    }
}

int main(int argc, char* argv[])
{
    try
    {
        cxxtools::Arg<cxxtools::Seconds> total(argc, argv, 'T', 5.0); // minimum runtime
        cxxtools::Arg<long> loops(argc, argv, 'l', 1000);
        cxxtools::Arg<unsigned> numthreads(argc, argv, 't', 1);

        cxxtools::Arg<bool> enable(argc, argv, 'e');
        cxxtools::Arg<bool> consolelog(argc, argv, 'c');
        cxxtools::Arg<unsigned short> udpport(argc, argv, 'u');
        cxxtools::Arg<std::string> logfile(argc, argv, 'f', "/dev/null");
        cxxtools::Arg<bool> norollingfile(argc, argv, 'r');

        cxxtools::LogConfiguration logConfiguration;
        logConfiguration.setRootLevel(cxxtools::Logger::LOG_LEVEL_INFO);

        unsigned settingCount = 0;
        if (consolelog)
            ++settingCount;

        if (logfile.isSet())
            ++settingCount;

        if (udpport.isSet())
        {
            ++settingCount;
            logConfiguration.setLoghost("", udpport);
        }

        if (settingCount > 1)
        {
            std::cerr << "only one of -c, -u or -f must be specified" << std::endl;
            return -1;
        }

        if (logfile.isSet() || settingCount == 0)
        {
            if (norollingfile || !logfile.isSet())
                logConfiguration.setFile(logfile);
            else
                logConfiguration.setFile(logfile, 1024*1024, 0);
        }

        log_init(logConfiguration);

        unsigned long count = 1;

        typedef std::vector<std::unique_ptr<bench::Logtester> > Threads;
        Threads threads;
        for (unsigned t = 0; t < numthreads; ++t)
            threads.emplace_back(new bench::Logtester(count, loops.getValue() / numthreads.getValue(), enable));

        while (count > 0)
        {
            std::cout << "count=" << (count * loops) << '\t' << std::flush;

            for (Threads::iterator it = threads.begin(); it != threads.end(); ++it)
                (*it)->setCount(count);

            cxxtools::Clock cl;
            cl.start();

            if (threads.size() == 1)
            {
                (*threads.begin())->run();
            }
            else
            {
                for (Threads::iterator it = threads.begin(); it != threads.end(); ++it)
                    (*it)->join();
            }

            cxxtools::Seconds T = cl.stop();

            std::cout.precision(6);
            std::cout << " T=" << T << '\t' << std::setprecision(12) << (count / T * loops)
                << " msg/s" << std::endl;

            if (T >= total)
                break;

            count <<= 1;
        }

    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}

