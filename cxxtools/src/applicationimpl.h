#ifndef CXXTOOLS_APPLICATION_IMPL_H
#define CXXTOOLS_APPLICATION_IMPL_H

#include <cxxtools/api.h>

namespace cxxtools {

class SelectorBase;

class ApplicationImpl
{
    public:
        ApplicationImpl();

        virtual ~ApplicationImpl();

        void init(SelectorBase& s);

        bool catchSystemSignal(int sig);

        bool raiseSystemSignal(int sig);

        //int signalFd() const;

};

} // namespace cxxtools

#endif
