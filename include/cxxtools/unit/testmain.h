/*
 * Copyright (C) 2005-2006 by Dr. Marc Boris Duerner
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
#ifndef CXXTOOLS_UNIT_TESTMAIN_H
#define CXXTOOLS_UNIT_TESTMAIN_H

#include <cxxtools/arg.h>
#include <cxxtools/unit/reporter.h>
#include <cxxtools/unit/jsonreporter.h>
#include <cxxtools/unit/application.h>
#include <cxxtools/json.h>
#include <cxxtools/regex.h>
#include <cxxtools/log.h>
#include <fstream>

int main(int argc, char** argv)
{
    log_init(argc, argv);

    cxxtools::unit::Application app;

    cxxtools::Arg<bool> help(argc, argv, 'h');
    cxxtools::Arg<bool> list(argc, argv, 'l');
    cxxtools::Arg<bool> json(argc, argv, "--json");
    cxxtools::Arg<bool> json1(argc, argv, 'j');
    cxxtools::Arg<bool> summary(argc, argv, 's');

    if( help || list )
    {
        std::cerr << "Usage: " << argv[0] << " {options} {testname}\n"
                     "Options:\n"
                     " -f <logfile>     output results into file\n"
                     " -s               print statistic summary\n"
                     " -j, --json       print output in json format instead of standard\n"
                     "Available Tests:\n";
        for (std::list<cxxtools::unit::Test*>::const_iterator it = app.tests().begin();
             it != app.tests().end();
             ++it)
        {
            std::cerr << "  - " << (*it)->name() << '\n';
        }

        return 0;
    }

    cxxtools::unit::BriefReporter consoleReporter;
    cxxtools::unit::JsonReporter jsonReporter;

    if (json || json1)
        app.attachReporter(jsonReporter);
    else
        app.attachReporter(consoleReporter);

    cxxtools::Arg<const char*> file(argc, argv, 'f');
    std::ofstream logFile;
    cxxtools::unit::BriefReporter fileReporter;

    if (file.isSet())
    {
        logFile.open(file);
        fileReporter.setOutput(logFile);
        app.attachReporter(fileReporter);
    }

    try
    {
        if (argc <= 1)
        {
            app.run();
        }
        else
        {
            for (int a = 1; a < argc; ++a)
            {
                cxxtools::Regex testName(argv[a]);
                app.run(testName);
            }
        }

        if (json || json1)
            std::cout << cxxtools::Json(jsonReporter).beautify(true);

        if (summary)
        {
            std::cout << "success: " << app.success() << "\n"
                         "errors:  " << app.errors() << "\n"
                         "skipped: " << app.skipped() << "\n";
        }

        return app.errors();
    }
    catch(const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }

    return 1;
}

#endif// CXXTOOLS_UNIT_TESTMAIN_H
