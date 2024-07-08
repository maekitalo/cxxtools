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
        cxxtools::Milliseconds msValue;
        cxxtools::DateTime dtValue;
    };

    static const std::string intValue = "intValue";
    static const std::string stringValue = "stringValue";
    static const std::string doubleValue = "doubleValue";
    static const std::string boolValue = "boolValue";
    static const std::string msValue = "msValue";
    static const std::string dtValue = "dtValue";
    static const std::string typeName = "TestObject";

    void operator>>= (const cxxtools::SerializationInfo& si, TestObject& obj)
    {
        si.getMember(intValue) >>= obj.intValue;
        si.getMember(stringValue) >>= obj.stringValue;
        si.getMember(doubleValue) >>= obj.doubleValue;
        si.getMember(boolValue) >>= obj.boolValue;
        si.getMember(msValue) >>= obj.msValue;
        si.getMember(dtValue) >>= obj.dtValue;
    }

    void operator<<= (cxxtools::SerializationInfo& si, const TestObject& obj)
    {
        si.addMember(intValue) <<= obj.intValue;
        si.addMember(stringValue) <<= obj.stringValue;
        si.addMember(doubleValue) <<= obj.doubleValue;
        si.addMember(boolValue) <<= obj.boolValue;
        si.addMember(msValue) <<= obj.msValue;
        si.addMember(dtValue) <<= obj.dtValue;
        si.setTypeName(typeName);
    }

    bool runXml = true;
    bool runJson = true;
    bool runBin = true;
}

// Function, which calls the serializer.
//
// Since the json serializer do not have a root node name we create a function
// and specialize it for the json serializer then.
template <typename T, typename Serializer>
void serialize(Serializer& serializer, const T& data)
{
    serializer.serialize(data, "d");
}

// This is the specialization for json.
template <typename T>
void serialize(cxxtools::JsonSerializer& serializer, const T& data)
{
    serializer.serialize(data);
}

// Measure the duration to serialize and deserialize a object and output the result.
template <typename T, typename Serializer, typename Deserializer>
void benchSerialization(const T& d, const char* fname = 0)
{
    std::stringstream data;
    Serializer serializer(data);

    // serialize
    cxxtools::Clock clock;
    clock.start();

    serialize(serializer, d);
    serializer.finish();

    cxxtools::Timespan ts = clock.stop();

    // optionally output serialized data to file
    if (fname)
    {
        std::ofstream f(fname);
        f << data.str();
    }

    // deserialization
    T v2;
    clock.start();

    Deserializer deserializer(data);
    deserializer.deserialize(v2);

    cxxtools::Timespan td = clock.stop();

    std::cout << "\tserialization: " << ts << "\n"
                 "\tdeserialization: " << td << "\n"
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
    benchSerialization<T, cxxtools::JsonSerializer, cxxtools::JsonDeserializer>(d, fname);
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

    if (runXml)
    {
        std::cout << "xml:" << std::endl;
        benchXmlSerialization(v, fileoutput ? (std::string("vector-") + typeName + ".xml").c_str() : 0);
    }

    if (runJson)
    {
        std::cout << "json:" << std::endl;
        benchJsonSerialization(v, fileoutput ? (std::string("vector-") + typeName + ".json").c_str() : 0);
    }

    if (runBin)
    {
        std::cout << "bin:" << std::endl;
        benchBinSerialization(v, fileoutput ? (std::string("vector-") + typeName + ".bin").c_str() : 0);
    }
}

int main(int argc, char* argv[])
{
    try
    {
        log_init(argc, argv);

        cxxtools::Arg<unsigned> nn(argc, argv, 'n', 100000);

        cxxtools::Arg<unsigned> I(argc, argv, 'I', nn);
        cxxtools::Arg<unsigned> D(argc, argv, 'D', nn);
        cxxtools::Arg<unsigned> C(argc, argv, 'C', nn);

        cxxtools::Arg<bool> fileoutput(argc, argv, 'f');

        runXml  = cxxtools::Arg<bool>(argc, argv, 'x');
        runJson = cxxtools::Arg<bool>(argc, argv, 'j');
        runBin  = cxxtools::Arg<bool>(argc, argv, 'b');

        std::cout << "size of SerializationInfo: " << sizeof(cxxtools::SerializationInfo) << std::endl;

        if (!runXml && !runJson && !runBin)
        {
            runXml  = runJson = runBin  = true;
        }

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
                obj.msValue = n;
                obj.dtValue = cxxtools::DateTime(1900+n, 1+n%12, 1+n%28, n%24, n%60, n%60);
                v.push_back(obj);
            }

            if (runXml)
            {
                std::cout << "xml:" << std::endl;
                benchXmlSerialization(v, fileoutput ? "custobject.xml" : 0);
            }

            if (runJson)
            {
                std::cout << "json:" << std::endl;
                benchJsonSerialization(v, fileoutput ? "custobject.json" : 0);
            }

            if (runBin)
            {
                std::cout << "bin:" << std::endl;
                benchBinSerialization(v, fileoutput ? "custobject.bin" : 0);
            }
        }

    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

