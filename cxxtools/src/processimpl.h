#if !defined(CXXTOOLS_ProcessImpl_h)
#define CXXTOOLS_ProcessImpl_h

#include "cxxtools/process.h"
#include "cxxtools/systemerror.h"
#include <cstdlib>
#include <sstream>
#include <unistd.h>

namespace cxxtools {

class ProcessImpl
{
    public:
        ProcessImpl(const ProcessInfo& procInfo);

        ~ProcessImpl();

        const ProcessInfo& procInfo() const
        { return _procInfo; }

        Process::State state() const
        { return _state; }

        void start();

        void kill();

        int wait();

        bool tryWait(int& status);

        static void setEnvVar(const std::string& name, const std::string& value);

        static void unsetEnvVar(const std::string& name);

        static std::string getEnvVar(const std::string& name);

        static void sleep(size_t msecs);

        static unsigned long usedMemory();

    private:
        pid_t m_pid;
        Process::State _state;
        ProcessInfo _procInfo;
};

} // namespace cxxtools

#endif // CXXTOOLS_ProcessImpl_h
