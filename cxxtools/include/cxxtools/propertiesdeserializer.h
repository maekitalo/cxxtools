/*
 * Copyright (C) 2012 Tommi Maekitalo
 *
 */

#ifndef CXXTOOLS_PROPERTIESDESERIALIZER_H
#define CXXTOOLS_PROPERTIESDESERIALIZER_H

#include <cxxtools/composer.h>
#include <cxxtools/serializationinfo.h>
#include <cxxtools/serializationerror.h>
#include <cxxtools/deserializer.h>
#include <cxxtools/textstream.h>

namespace cxxtools
{
    class CXXTOOLS_API PropertiesDeserializer : public Deserializer
    {
        class Ev;
        friend class Ev;

    public:
        PropertiesDeserializer(std::istream& in, TextCodec<Char, char>* codec = 0);

        PropertiesDeserializer(TextIStream& in);

        ~PropertiesDeserializer();

    private:
        void doDeserialize();

        TextIStream* _ts;
        TextIStream& _in;
    };
}

#endif // CXXTOOLS_PROPERTIESDESERIALIZER_H

