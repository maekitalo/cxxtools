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
#include <cxxtools/properties.h>
#include <cxxtools/json.h>
#include <cxxtools/ini.h>
#include <cxxtools/query_params.h>
#include <cxxtools/settings.h>
#include <cxxtools/textstream.h>
#include <cxxtools/utf8codec.h>
#include <cxxtools/log.h>
#include <cxxtools/serializationinfo.h>
#include <cxxtools/xml/xml.h>

#include <limits>
#include <iostream>

struct Usage { };

class Siconvert
{
        bool inputBin;
        bool inputXml;
        bool inputJson;
        bool inputIni;
        bool inputCsv;
        bool inputQparams;
        bool inputSettings;

        bool outputBin;
        bool outputXml;
        bool outputXmlCompact;
        bool outputJson;
        bool outputJsongrep;
        bool outputIni;
        bool outputCsv;
        bool outputProperties;

        bool outputCount;
        bool beautify;

        unsigned skip;
        unsigned num;
        unsigned count;

        std::string xmlRootNodeName;
        std::string path;

    public:
        Siconvert(int& argc, char* argv[]);
        void convert(std::istream& in, std::ostream& out);
        bool docontinue() const  { return num > 0; }
        void finish() const;
};

Siconvert::Siconvert(int& argc, char* argv[])
    : inputBin(cxxtools::Arg<bool>(argc, argv, 'b')),
      inputXml(cxxtools::Arg<bool>(argc, argv, 'x')),
      inputJson(cxxtools::Arg<bool>(argc, argv, 'j')),
      inputIni(cxxtools::Arg<bool>(argc, argv, 'i')),
      inputCsv(cxxtools::Arg<bool>(argc, argv, 'c')),
      inputQparams(cxxtools::Arg<bool>(argc, argv, 'q')),
      inputSettings(cxxtools::Arg<bool>(argc, argv, 's')),

      outputBin(cxxtools::Arg<bool>(argc, argv, 'B')),
      outputXml(cxxtools::Arg<bool>(argc, argv, 'X')),
      outputXmlCompact(cxxtools::Arg<bool>(argc, argv, 'Y')),
      outputJson(cxxtools::Arg<bool>(argc, argv, 'J')),
      outputJsongrep(cxxtools::Arg<bool>(argc, argv, 'G')),
      outputIni(cxxtools::Arg<bool>(argc, argv, 'I')),
      outputCsv(cxxtools::Arg<bool>(argc, argv, 'C')),
      outputProperties(cxxtools::Arg<bool>(argc, argv, 'P')),

      outputCount(cxxtools::Arg<bool>(argc, argv, 'N')),
      beautify(cxxtools::Arg<bool>(argc, argv, 'd')),
      skip(cxxtools::Arg<unsigned>(argc, argv, "--skip")),
      num(cxxtools::Arg<unsigned>(argc, argv, "--num", std::numeric_limits<unsigned>::max())),
      count(0),
      xmlRootNodeName(cxxtools::Arg<std::string>(argc, argv, 'R', "root")),
      path(cxxtools::Arg<std::string>(argc, argv, "--path"))
{
    unsigned c;

    c = 0;
    if (inputBin)
        ++c;
    if (inputXml)
        ++c;
    if (inputJson)
        ++c;
    if (inputIni)
        ++c;
    if (inputCsv)
        ++c;
    if (inputQparams)
        ++c;
    if (inputSettings)
        ++c;

    if (c != 1)
    {
        std::cerr << "One input format must be specified." << std::endl;
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
    if (outputJsongrep)
        ++c;
    if (outputIni)
        ++c;
    if (outputCsv)
        ++c;
    if (outputProperties)
        ++c;
    if (outputCount)
        ++c;

    if (c != 1)
    {
        std::cerr << "One output format must be specified." << std::endl;
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
    else if (inputIni)
        in >> cxxtools::Ini(si);
    else if (inputCsv)
        in >> cxxtools::Csv(si);
    else if (inputQparams)
    {
        cxxtools::QueryParams q;
        in >> q;
        si <<= q;
    }
    else if (inputSettings)
    {
        cxxtools::Settings settings;
        cxxtools::TextIStream tin(in, new cxxtools::Utf8Codec());
        settings.load(tin);
        si = settings;
    }

    if (!path.empty())
        si = si.path(path);

    if (skip == 0)
    {
        if (outputBin)
            out << cxxtools::bin::Bin(si);
        else if (outputXml)
            out << cxxtools::xml::Xml(si, xmlRootNodeName).beautify(beautify);
        else if (outputXmlCompact)
            out << cxxtools::xml::Xml(si, xmlRootNodeName).beautify(beautify).useAttributes(false);
        else if (outputJson)
            out << cxxtools::Json(si).beautify(beautify);
        else if (outputJsongrep)
            out << cxxtools::Json(si).beautify(beautify) << '\n';
        else if (outputIni)
            out << cxxtools::Ini(si);
        else if (outputCsv)
            out << cxxtools::Csv(si);
        else if (outputProperties)
            out << cxxtools::Properties(si);

        if (num > 0 && num != std::numeric_limits<unsigned>::max())
            --num;

        ++count;
    }
    else
    {
        --skip;
    }
}

void Siconvert::finish() const
{
    if (outputCount)
        std::cout << count << std::endl;
}

int main(int argc, char* argv[])
{
    try
    {
        log_init(argc, argv);

        cxxtools::ArgOut out(argc, argv, 'o');
        cxxtools::Arg<bool> verbose(argc, argv, 'v');

        Siconvert app(argc, argv);

        if (argc > 1)
        {
            for (int a = 1; a < argc; ++a)
            {
                if (verbose)
                    std::cerr << "process " << a << " <" << argv[a] << '>' << std::endl;

                std::ifstream in(argv[a]);

                do
                {
                    app.convert(in, out);
                } while (app.docontinue() && in.peek() != std::char_traits<char>::eof());
            }
        }
        else
        {
            do
            {
                app.convert(std::cin, out);
            } while (app.docontinue() && std::cin.peek() != std::char_traits<char>::eof());
        }

        app.finish();
    }
    catch (Usage)
    {
        std::cerr << R"USAGE(
Usage: siconvert {options} [inputfiles...]

Description:

    Converts data using cxxtools serialization from one format to another.
    When no inputfile is given, data is read from stdin.\n

Options for input format:

    -b              read binary data
    -x              read xml data
    -j              read json data
    -i              read ini data
    -c              read csv data
    -q              read url query string
    -s              read settings data

Options for output format:

    -B              output binary data
    -X              output xml data
    -Y              output xml data without attributes
    -J              output json data
    -G              output grep friendly json (unformatted but newline after each message)
    -I              output ini data
    -C              output csv data
    -P              output properties data
    -N              output number of objects
    -d              beautify output (xml, json)
    -R <name>       name for root node in xml format (default <root>)

Other options:

    --skip <n>      skip <n> objects
    --num <n>       read <n> objects (default unlimited)
    --path <path>   select node from input
    -v              verbose - output filename to stderr when processing
    -o <file>       output to file

Path:

    $           root element
    .member     access member
    [n]         array index
    {n}         nth occurence of member
    member{}    all occurences as array
    ::size      size
    ::count     count of members
    ::type      typename
    ::isnull    true if null, false otherwise

    Root element can be omitted. Also the starting '.'

    `$.store.book` can be written as `store.book`
    `$[2]` can be written as `[2]`

)USAGE";
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
