/*
 * Copyright (C) 2005-2006 by Marc Boris Duerner
 * Copyright (C)      2006 by Aloysius Indrayanto
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
#ifndef cxxtools_Singleton_h
#define cxxtools_Singleton_h

#include <cxxtools/noncopyable.h>

#include <memory>
#include <cstdlib>

namespace cxxtools {

    /** @brief Singleton class template
       @ingroup cxxtools

        @param T Type of the singleton
        @param A Allocator for type T

        The Singleton class template can be used to easily implement the Singleton
        design pattern. It can either be used directly or as a base class. An
        allocator can be used to control how the singleton instance will be
        allocated.

        The follwing example shows how to use the singleton as a base class:
        @code
              class MySingleton : public Singleton<MySingleton>
              {
                   friend class Singleton<MySingleton>;

                   // ...
               };
        @endcode
     */
    template <typename T, typename A = std::allocator<T> >
    class Singleton : private NonCopyable
    {
        public:
            typedef A Allocator;

        public:
            /** @brief Returns the instance of the singleton type

                    When called for the first time, the singleton instance will be
                    created with the specified alloctaor. All subsequent calls wikk
                    return a reference to the previously created instance.

                @return The singleton instance
             */
            static T& instance()
            {
                if(!_instance)
                {
                    try
                    {
                        _instance = (T*)_allocator.allocate(1);
                        new (_instance) T();
                        std::atexit(&atExit);
                    }
                    catch( const std::bad_alloc& e )
                    {
                        throw e;
                    }
                    catch(...)
                    {
                        _allocator.deallocate(_instance, 1);
                        _instance = 0;
                        throw;
                    }
                }

                return *_instance;
            }

        protected:
            /**  @brief Contructor
             */
            Singleton()
            { }

            /**  @brief Destructor
             */
            ~Singleton()
            { }

        private:
            /** @brief Exit handler

                This function is set as the program exit handler and will destroy
                the singleton instance at the end of the program using the
                specified allocator.
             */
            static void atExit()
            {
                _instance->~T();
                _allocator.deallocate(_instance, 1);
                _instance = 0;
            }

        private:
            static A  _allocator;
            static T* _instance;
    };


    template <typename T, typename A>
    A Singleton<T, A>::_allocator;


    template <typename T, typename A>
    T* Singleton<T, A>::_instance = 0;

} // namespace cxxtools

#endif
