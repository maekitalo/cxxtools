/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#include <iostream>
#include <fstream>
#include <math.h>
#include <cxxtools/xml/xmlserializer.h>
#include <cxxtools/xml/xmldeserializer.h>
#include <cxxtools/jsonserializer.h>
#include <cxxtools/jsondeserializer.h>
#include <cxxtools/bin/serializer.h>
#include <cxxtools/bin/deserializer.h>
#include <cxxtools/arg.h>
#include <cxxtools/clock.h>
#include <cxxtools/convert.h>
#include <cxxtools/tee.h>
#include <cxxtools/log.h>

namespace
{
    struct TestObject
    {
        int intValue;
        std::string stringValue;
        double doubleValue;
        bool boolValue;
    };

    void operator>>= (const cxxtools::SerializationInfo& si, TestObject& obj)
    {
        si.getMember("intValue") >>= obj.intValue;
        si.getMember("stringValue") >>= obj.stringValue;
        si.getMember("doubleValue") >>= obj.doubleValue;
        si.getMember("boolValue") >>= obj.boolValue;
    }

    void operator<<= (cxxtools::SerializationInfo& si, const TestObject& obj)
    {
        si.addMember("intValue") <<= obj.intValue;
        si.addMember("stringValue") <<= obj.stringValue;
        si.addMember("doubleValue") <<= obj.doubleValue;
        si.addMember("boolValue") <<= obj.boolValue;
        si.setTypeName("TestObject");
    }

    class JsonSerializer2 : public cxxtools::JsonSerializer
    {
        public:
            JsonSerializer2() { }

            explicit JsonSerializer2(std::basic_ostream<cxxtools::Char>& ts)
                : cxxtools::JsonSerializer(ts)
                { }

            explicit JsonSerializer2(std::ostream& os,
                cxxtools::TextCodec<cxxtools::Char, char>* codec = 0)
                : cxxtools::JsonSerializer(os, codec)
                { }

            template <typename T>
            JsonSerializer2& serialize(const T& v, const std::string& name)
            {
                cxxtools::JsonSerializer::serialize(v);
                return *this;
            }
    };
}

template <typename T, typename Serializer, typename Deserializer>
void benchSerialization(const T& d, const char* fname = 0)
{
    std::stringstream data;
    Serializer serializer(data);
    Deserializer deserializer(data);

    cxxtools::Clock clock;
    clock.start();
    serializer.serialize(d, "d");
    serializer.finish();
    cxxtools::Timespan ts = clock.stop();
    if (fname)
    {
        std::ofstream f(fname);
        f << data.str();
    }

    T v2;
    clock.start();
    deserializer.deserialize(v2);
    cxxtools::Timespan td = clock.stop();

    std::cout << "\tserialization: " << ts << " sec\n"
                 "\tdeserialization: " << td << " sec\n"
                 "\tsize: " << data.str().size() << " bytes" << std::endl;
}

template <typename T>
void benchXmlSerialization(const T& d, const char* fname = 0)
{
    benchSerialization<T, cxxtools::xml::XmlSerializer, cxxtools::xml::XmlDeserializer>(d, fname);
}

template <typename T>
void benchJsonSerialization(const T& d, const char* fname = 0)
{
    benchSerialization<T, JsonSerializer2, cxxtools::JsonDeserializer>(d, fname);
}

template <typename T>
void benchBinSerialization(const T& d, const char* fname = 0)
{
    benchSerialization<T, cxxtools::bin::Serializer, cxxtools::bin::Deserializer>(d, fname);
}

template <typename T>
void benchVector(const char* typeName, unsigned N, T increment, bool fileoutput)
{
    std::cout << "vector of " << typeName << " values:" << std::endl;

    std::vector<T> v;
    T value = 0;
    for (unsigned n = 0; n < N; ++n, value += increment)
        v.push_back(value);

    std::cout << "xml:" << std::endl;
    benchXmlSerialization(v, fileoutput ? (std::string("vector-") + typeName + ".xml").c_str() : 0);

    std::cout << "json:" << std::endl;
    benchJsonSerialization(v, fileoutput ? (std::string("vector-") + typeName + ".json").c_str() : 0);

    std::cout << "bin:" << std::endl;
    benchBinSerialization(v, fileoutput ? (std::string("vector-") + typeName + ".bin").c_str() : 0);
}

int main(int argc, char* argv[])
{
    try
    {
        log_init();

        cxxtools::Arg<unsigned> nn(argc, argv, 'n', 100000);
        cxxtools::Arg<unsigned> I(argc, argv, 'I', nn);
        cxxtools::Arg<unsigned> D(argc, argv, 'D', nn);
        cxxtools::Arg<unsigned> C(argc, argv, 'C', nn);
        cxxtools::Arg<bool> fileoutput(argc, argv, 'f');

        std::cout << "benchmark serializer with " << I.getValue() << " int vector " << D.getValue() << " double vector and " << C.getValue() << " custom vector iterations\n\n"
                     "options:\n"
                     "   -n <number>       specify number of default iterations\n"
                     "   -I <number>       specify number of iterations for int vector\n"
                     "   -D <number>       specify number of iterations for double vector\n"
                     "   -C <number>       specify number of iterations for custom object\n"
                     "   -f                write serialized output to files\n" << std::endl;

        if (I.getValue() > 0)
            benchVector<int>("int", I, 1, fileoutput);

        if (D.getValue() > 0)
            benchVector<double>("double", D, 0.25, fileoutput);

        if (C.getValue() > 0)
        {
            std::cout << "vector of custom objects:" << std::endl;

            TestObject obj;
            std::vector<TestObject> v;
            for (unsigned n = 0; n < C; ++n)
            {
                obj.intValue = n;
                obj.stringValue = cxxtools::convert<std::string>(n);
                obj.doubleValue = sqrt(static_cast<double>(n));
                obj.boolValue = n&1;
                v.push_back(obj);
            }

            std::cout << "xml:" << std::endl;
            benchXmlSerialization(v, fileoutput ? "custobject.xml" : 0);

            std::cout << "json:" << std::endl;
            benchJsonSerialization(v, fileoutput ? "custobject.json" : 0);

            std::cout << "bin:" << std::endl;
            benchBinSerialization(v, fileoutput ? "custobject.bin" : 0);
        }

    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

