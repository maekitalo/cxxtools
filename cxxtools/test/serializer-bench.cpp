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
#include <cxxtools/binserializer.h>
#include <cxxtools/bindeserializer.h>
#include <cxxtools/arg.h>
#include <cxxtools/clock.h>
#include <cxxtools/convert.h>
#include <cxxtools/tee.h>

namespace
{
    struct TestObject
    {
        int intValue;
        std::string stringValue;
        double doubleValue;
    };

    void operator>>= (const cxxtools::SerializationInfo& si, TestObject& obj)
    {
        si.getMember("intValue") >>= obj.intValue;
        si.getMember("stringValue") >>= obj.stringValue;
        si.getMember("doubleValue") >>= obj.doubleValue;
    }

    void operator<<= (cxxtools::SerializationInfo& si, const TestObject& obj)
    {
        si.addMember("intValue") <<= obj.intValue;
        si.addMember("stringValue") <<= obj.stringValue;
        si.addMember("doubleValue") <<= obj.doubleValue;
        si.setTypeName("TestObject");
    }
}

template <typename T, typename Serializer, typename Deserializer>
void benchSerialization(const T& d)
{
    std::stringstream data;
    std::ofstream o(typeid(Serializer).name());
    cxxtools::Tee tee(data, o);
    Serializer serializer(tee);
    Deserializer deserializer(data);

    cxxtools::Clock clock;
    clock.start();
    serializer.serialize(d, "d");
    serializer.finish();
    cxxtools::Timespan ts = clock.stop();

    T v2;
    clock.start();
    deserializer.deserialize(v2);
    cxxtools::Timespan td = clock.stop();

    std::cout << "\tserialization: " << ts.toUSecs() / 1e6 << " sec\n"
                 "\tdeserialization: " << td.toUSecs() / 1e6 << " sec\n"
                 "\tsize: " << data.str().size() << " bytes" << std::endl;
}

template <typename T>
void benchXmlSerialization(const T& d)
{
    benchSerialization<T, cxxtools::xml::XmlSerializer, cxxtools::xml::XmlDeserializer>(d);
}

template <typename T>
void benchBinSerialization(const T& d)
{
    benchSerialization<T, cxxtools::BinSerializer, cxxtools::BinDeserializer>(d);
}

int main(int argc, char* argv[])
{
    try
    {
        cxxtools::Arg<unsigned> N(argc, argv, 'n', 10000);

        // fill
/*
        std::vector<std::string> v;
        for (unsigned n = 0; n < N; ++n)
            v.push_back(cxxtools::convert<std::string>(n));
            */

        TestObject obj;
        std::vector<TestObject> v;
        for (unsigned n = 0; n < N; ++n)
        {
            obj.intValue = n;
            obj.stringValue = cxxtools::convert<std::string>(n);
            obj.doubleValue = sqrt(n);
            v.push_back(obj);
        }

        std::cout << "xml:" << std::endl;
        benchXmlSerialization(v);

        std::cout << "bin:" << std::endl;
        benchBinSerialization(v);

    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

