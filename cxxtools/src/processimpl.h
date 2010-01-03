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
#if !defined(CXXTOOLS_ProcessImpl_h)
#define CXXTOOLS_ProcessImpl_h

#include "cxxtools/process.h"
#include "cxxtools/systemerror.h"
#include "cxxtools/noncopyable.h"
#include "cxxtools/posix/pipe.h"
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

        posix::Pipe* _stdinPipe;
        posix::Pipe* _stdoutPipe;
        posix::Pipe* _stderrPipe;
};

} // namespace cxxtools

#endif // CXXTOOLS_ProcessImpl_h
