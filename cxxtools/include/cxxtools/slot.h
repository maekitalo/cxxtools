#ifndef cxxtools_slot_h
#define cxxtools_slot_h

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


    template < typename R,
               typename A1 = cxxtools::Void,
               typename A2 = cxxtools::Void,
               typename A3 = cxxtools::Void,
               typename A4 = cxxtools::Void,
               typename A5 = cxxtools::Void,
               typename A6 = cxxtools::Void,
               typename A7 = cxxtools::Void,
               typename A8 = cxxtools::Void >
    class BasicSlot : public Slot {
        public:
            virtual Slot* clone() const = 0;
    };

} //cxxtools


#endif


