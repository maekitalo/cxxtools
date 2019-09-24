#include <cxxtools/tz.h>
#include <cxxtools/log.h>
#include <cxxtools/arg.h>

#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        log_init();

        cxxtools::Arg<bool> help(argc, argv, '?');
        if (help || argc == 1)
        {
            std::cerr << "Usage: " << argv[0] << " {options} {times}\n"
                         "\n"
                         "Synopsis:\n"
                         "current time in time zone Pacific/Nauru\n"
                         " " << argv[0] << " -c -z Pacific/Nauru\n"
                         "dump information about the current time zone\n"
                         " " << argv[0] << " -d\n"
                         "output some GMT times in the time zone\n"
                         " " << argv[0] << " '2017-03-26 01:00:00' '2017-03-26 00:59:59'\n"
                         "\n"
                         "Options:\n"
                         " -a             list all valid time zones\n"
                         " -z <timezone>  specify time zone (default: system time zone)\n"
                         " -Z             output time zone of the system\n"
                         " -d             dump time zone informations\n"
                         " -c             output current time in the specified time zone\n"
                         " -u             print current utc time\n"
                         " -r             reverse\n"
                         " -e             early flag (with -r take earlier time if ambiguous)\n"
                         " -l             late flag (with -r take later time if ambiguous)\n"
                         " -v             output previous and next change of dst\n";
        }

        cxxtools::Arg<std::string> timeZone(argc, argv, 'z', cxxtools::Tz::currentZone());
        cxxtools::Tz tz(timeZone);

        cxxtools::Arg<bool> dump(argc, argv, 'd');
        cxxtools::Arg<bool> listAll(argc, argv, 'a');
        cxxtools::Arg<bool> currentTime(argc, argv, 'c');
        cxxtools::Arg<bool> utc(argc, argv, 'u');
        cxxtools::Arg<bool> currentTimeZone(argc, argv, 'Z');
        cxxtools::Arg<bool> reverse(argc, argv, 'r');
        cxxtools::Arg<bool> early(argc, argv, 'e');
        cxxtools::Arg<bool> late(argc, argv, 'l');
        cxxtools::Arg<bool> verbose(argc, argv, 'v');  // show previous and next change

        if (dump)
        {
            std::cout << timeZone.getValue() << '\n';
            tz.dump(std::cout);
        }

        if (listAll)
        {
            auto l = cxxtools::Tz::getTimeZones();
            for (auto z: l)
                std::cout << z << '\n';
        }

        if (currentTime)
        {
            std::cout << tz.toLocal(cxxtools::DateTime::gmtime()).toString() << '\n';
        }

        if (utc)
        {
            std::cout << cxxtools::DateTime::gmtime().toString() << '\n';
        }

        if (currentTimeZone)
        {
            std::cout << cxxtools::Tz::currentZone() << '\n';
        }

        for (int a = 1; a < argc; ++a)
        {
            cxxtools::DateTime dt(argv[a]);
            if (reverse)
            {
                cxxtools::UtcDateTime dtz;
                if (early)
                    dtz = tz.toUtc(cxxtools::LocalDateTime(dt), 1);
                else if (late)
                    dtz = tz.toUtc(cxxtools::LocalDateTime(dt), 0);
                else
                    dtz = tz.toUtc(cxxtools::LocalDateTime(dt));
                std::cout << dtz.toString() << '\n';
            }
            else
            {
                auto dtz = tz.toLocal(cxxtools::UtcDateTime(dt));
                std::cout << dtz.toString() << ' ' << dtz.tzName() << ' ' << dtz.isdst();
                if (verbose)
                    std::cout << " previous change: " << tz.previousChange(dtz).toString()
                              << " next change: " << tz.nextChange(dtz).toString();
                std::cout << '\n';
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
