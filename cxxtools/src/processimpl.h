#if !defined(CXXTOOLS_ProcessImpl_h)
#define CXXTOOLS_ProcessImpl_h

#include "cxxtools/process.h"
#include "cxxtools/systemerror.h"
#include "cxxtools/noncopyable.h"
#include "cxxtools/pipe.h"
#include <cstdlib>
#include <sstream>
#include <unistd.h>

namespace cxxtools {

class ProcessImpl : private NonCopyable
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

        IODevice* stdInput()
        { return _stdInput; }

        IODevice* stdOutput()
        { return _stdOutput; }

        IODevice* stdError()
        { return _stdError; }

        static void setEnvVar(const std::string& name, const std::string& value);

        static void unsetEnvVar(const std::string& name);

        static std::string getEnvVar(const std::string& name);

        static void sleep(size_t msecs);

        static unsigned long usedMemory();

    private:
        pid_t _pid;
        Process::State _state;
        ProcessInfo _procInfo;

        IODevice* _stdInput;
        IODevice* _stdOutput;
        IODevice* _stdError;

        Pipe* _stdinPipe;
        Pipe* _stdoutPipe;
        Pipe* _stderrPipe;
};

} // namespace cxxtools

#endif // CXXTOOLS_ProcessImpl_h
