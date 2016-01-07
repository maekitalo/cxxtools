/*
 * Copyright (C) 2015 Tommi Maekitalo
 *
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <cxxtools/mime.h>
#include <cxxtools/json.h>
#include <cxxtools/log.h>

int main(int argc, char* argv[])
{
    try
    {
        log_init();

        std::ostringstream m;

        if (argc > 1)
        {
            std::ifstream in(argv[1]);
            m << in.rdbuf();
        }
        else
        {
            m << std::cin.rdbuf();
        }

        cxxtools::MimeMultipart mp(m.str());

        std::cout << cxxtools::Json(mp).beautify(true);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

