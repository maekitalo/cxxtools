/*
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
#ifndef cxxtools_xml_EntityResolver_h
#define cxxtools_xml_EntityResolver_h

#include <cxxtools/xml/api.h>
#include <cxxtools/string.h>
#include <map>

namespace cxxtools
{

namespace xml
{

/**
 * @brief Entity resolver class which associates entities to resolved entity values.
 *
 * Entities can be added to this class using the method addEntity(). This method takes
 * the entity and the resolved entity value. To resolve the resolves value for an entity
 * the method resolveEntity() can be used.
 */
class CXXTOOLS_XML_API EntityResolver
{

    public:
        /**
         * @brief Constructs a new Resolver object and initializes the entity list using the XML default entities.
         *
         * The constructor calls clear() which clears the entity list and adds the XML default entities.
         */
        EntityResolver();

        //! Empty destructor.
        virtual ~EntityResolver();

        /**
         * @brief Resets the entity list to the XML default entities.
         *
         * The default entities are all entities from HTML4
         */
        void clear();

        /**
         * @brief Adds the given entity and the given resolved entity value (token) to the entity list.
         *
         * To determine the resolved entity value of a entity the method resolveEntity() can be used.
         *
         * @param entity A list entry for this entity is created and associated with the also given token.
         * @param token The resolved entity value that is associated with the also given entity.
         */
        void addEntity(const String& entity, const String& token);

        /**
         * @brief Returns the resolved entity value (token) for the given entity.
         *
         * If the entity is not in the list or an dec or hex entity is invalid an exception is thrown.
         *
         * @param entity The resolved entity value for this entity is returned.
         * @return The resolved entity.
         * @throws XmlError if the entity is not in the list.
         *
         * TODO add const specifier
         */
        String resolveEntity(const String& entity);

    private:
        //! Entity map containing entities which are associated to their resolved entity value.
        typedef std::map<String, String> EntityMap;
        std::map<String, String> _entityMap;
};

}

}

#endif
