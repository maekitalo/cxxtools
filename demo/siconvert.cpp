/*
 * Copyright (C) 2018 Tommi Maekitalo
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
#include <cxxtools/argout.h>
#include <cxxtools/bin/bin.h>
#include <cxxtools/csv.h>
#include <cxxtools/json.h>
#include <cxxtools/log.h>
#include <cxxtools/serializationinfo.h>
#include <cxxtools/xml/xml.h>

#include <iostream>

struct Usage { };

class Siconvert
{
        bool inputBin;
        bool inputXml;
        bool inputJson;
        bool inputCsv;

        bool outputBin;
        bool outputXml;
        bool outputXmlCompact;
        bool outputJson;
        bool outputCsv;

        bool outputCount;
        bool beautify;

    public:
        Siconvert(int& argc, char* argv[]);
        void convert(std::istream& in, std::ostream& out);
};

Siconvert::Siconvert(int& argc, char* argv[])
    : inputBin(cxxtools::Arg<bool>(argc, argv, 'b')),
      inputXml(cxxtools::Arg<bool>(argc, argv, 'x')),
      inputJson(cxxtools::Arg<bool>(argc, argv, 'j')),
      inputCsv(cxxtools::Arg<bool>(argc, argv, 'c')),

      outputBin(cxxtools::Arg<bool>(argc, argv, 'B')),
      outputXml(cxxtools::Arg<bool>(argc, argv, 'X')),
      outputXmlCompact(cxxtools::Arg<bool>(argc, argv, 'Y')),
      outputJson(cxxtools::Arg<bool>(argc, argv, 'J')),
      outputCsv(cxxtools::Arg<bool>(argc, argv, 'C')),

      outputCount(cxxtools::Arg<bool>(argc, argv, 'n')),
      beautify(cxxtools::Arg<bool>(argc, argv, 'd'))
{
    unsigned c;

    c = 0;
    if (inputBin)
        ++c;
    if (inputXml)
        ++c;
    if (inputJson)
        ++c;
    if (inputCsv)
        ++c;

    if (c != 1)
    {
        std::cerr << "one input format must be specified" << std::endl;
        throw Usage();
    }

    c = 0;
    if (outputBin)
        ++c;
    if (outputXml)
        ++c;
    if (outputXmlCompact)
        ++c;
    if (outputJson)
        ++c;
    if (outputCsv)
        ++c;
    if (outputCount)
        ++c;

    if (c != 1)
    {
        std::cerr << "one output format must be specified" << std::endl;
        throw Usage();
    }
}

void Siconvert::convert(std::istream& in, std::ostream& out)
{
    cxxtools::SerializationInfo si;
    if (inputBin)
        in >> cxxtools::bin::Bin(si);
    else if (inputXml)
        in >> cxxtools::xml::Xml(si);
    else if (inputJson)
        in >> cxxtools::Json(si);
    else if (inputCsv)
        in >> cxxtools::Csv(si);

    if (outputCount)
        out << si.memberCount() << std::endl;
    else if (outputBin)
        out << cxxtools::bin::Bin(si);
    else if (outputXml)
        out << cxxtools::xml::Xml(si, "root").beautify(beautify);
    else if (outputXmlCompact)
        out << cxxtools::xml::Xml(si, "root").beautify(beautify).useAttributes(false);
    else if (outputJson)
        out << cxxtools::Json(si).beautify(beautify);
    else if (outputCsv)
        out << cxxtools::Csv(si);
}

int main(int argc, char* argv[])
{
    try
    {
        log_init();

        cxxtools::ArgOut out(argc, argv, 'o');
        cxxtools::Arg<bool> verbose(argc, argv, 'v');

        Siconvert app(argc, argv);

        if (argc > 1)
        {
            for (unsigned a = 1; a < argc; ++a)
            {
                if (verbose)
                    std::cerr << "process " << a << " <" << argv[a] << '>' << std::endl;

                std::ifstream in(argv[a]);
                app.convert(in, out);
            }
        }
        else
        {
            app.convert(std::cin, out);
        }
    }
    catch (Usage)
    {
        std::cerr << "Usage: " << argv[0] << " {options} [inputfiles...]\n\n"
                     "Description:\n"
                     "  Converts data using cxxtools serialization from one format to another.\n"
                     "  When no inputfile is given, data is read from stdin.\n\n"
                     "Options:\n"
                     " -b         read binary data\n"
                     " -x         read xml data\n"
                     " -j         read json data\n"
                     " -c         read csv data\n"
                     " -B         output binary data\n"
                     " -X         output xml data\n"
                     " -Y         output xml data without attributes\n"
                     " -J         output json data\n"
                     " -C         output csv data\n"
                     " -n         output just number of nodes on first level\n"
                     " -d         beautify output (xml, json)\n"
                     " -v         verbose - output filename to stderr when processing\n"
                     " -o <file>  output to file\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
