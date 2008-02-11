#ifndef cxxtools_Slot_h
#define cxxtools_Slot_h

#include <cxxtools/api.h>
#include <cxxtools/void.h>

namespace cxxtools {

    class Connection;

    class Slot {
        public:
            virtual ~Slot() {}

            virtual Slot* clone() const = 0;

            virtual const void* callable() const = 0;

            virtual bool opened(const Connection& c) = 0;

            virtual void closed(const Connection& c) = 0;
    };

#include <cxxtools/slot.tpp>

}

#endif


