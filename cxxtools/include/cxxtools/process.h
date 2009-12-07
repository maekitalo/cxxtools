/*
 * Copyright (C) 2006-2008 by Marc Boris Duerner
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

#ifndef CXXTOOLS_PROCESS_H
#define CXXTOOLS_PROCESS_H

#include <cxxtools/api.h>
#include <cxxtools/iodevice.h>
#include <cxxtools/noncopyable.h>
#include <string>
#include <vector>
#include <stdexcept>

namespace cxxtools {

//! ProcessFailed exception
// thrown when a process does not terminate normally in wait()
class ProcessFailed : public std::runtime_error
{
    public:
        ProcessFailed()
            : std::runtime_error("process failed")
            { }
};


//! Process parameters
class ProcessInfo
{
    public:
        enum IODeviceMode
        {
            Close   = 0,
            Keep    = 1,
            Capture = 2,
            Combine = 3  // combine stderr with stdout; only valid for stderr
        };

        //! process info can contain at least the command
        ProcessInfo(const std::string& command);

        const std::string& command() const;

        /** @brief Adds an argument to the list of arguments
        */
        ProcessInfo& addArg(const std::string& argument);

        unsigned argCount() const;

        const std::string& arg(unsigned idx) const;


        bool detach() const
        { return _detach; }

        void detach(bool sw)
        { _detach = sw; }


        void setStdInput(IODeviceMode mode)
        { _stdinMode = mode; }

        void setStdInput(IODevice* dev)
        { _stdin = dev; _stdinMode = Keep; }

        IODevice* stdInput() const
        { return _stdin; }

        IODeviceMode stdInputMode() const
        { return _stdinMode; }


        void setStdOutput(IODeviceMode mode)
        { _stdoutMode = mode; }

        void setStdOutput(IODevice* dev)
        { _stdout = dev; _stdoutMode = Keep; }

        IODevice* stdOutput() const
        { return _stdout; }

        IODeviceMode stdOutputMode() const
        { return _stdoutMode; }


        void setStdError(IODeviceMode mode)
        { _stderrMode = mode; }

        void setStdError(IODevice* dev)
        { _stderr = dev; _stderrMode = Keep; }

        IODevice* stdError() const
        { return _stderr; }

        IODeviceMode stdErrorMode() const
        { return _stderrMode; }

    private:
        std::string _command;
        std::vector<std::string> _args;
        bool _detach;

        IODeviceMode _stdinMode;
        IODevice* _stdin;

        IODeviceMode _stdoutMode;
        IODevice* _stdout;

        IODeviceMode _stderrMode;
        IODevice* _stderr;
};

//! Process Environment
class CXXTOOLS_API Process : private NonCopyable
{
    public:
        enum State
        {
            Ready    = 0,
            Running  = 1,
            Finished = 2,
            Failed   = 3
        };

    public:
        //! Constructs a Process with a command including its arguments
        /**
            @param command Name of the executable along with its arguments
        */
        explicit Process(const std::string& commandline);

        //! Constructs a Process with a process info structure
        explicit Process(const ProcessInfo& procInfo);

        //! Dtor
        ~Process();

        const ProcessInfo& procInfo() const;

        State state() const;

        //! Start/Create the Process
        /**
            @throw SystemError
        */
        void start();

        //! Kills the Process
        /**
            @throw SystemError
        */
        void kill();

        //! Waits until the Process ends
        /**
            @throw SystemError
        */
        int wait();

        IODevice* stdInput();

        IODevice* stdOutput();

        IODevice* stdError();

        //! Set environment variable
        /**
            @throw SystemError
        */
        static void setEnvVar(const std::string& name, const std::string& value);

        //! Unset environment variable
        /**
            @throw SystemError
        */
        static void unsetEnvVar(const std::string& name);

        //! Get environment variable
        /**
            @throw SystemError
        */
        static std::string getEnvVar(const std::string& name);

        //! Called Process Sleeps milliSec Seconds
        /**
            @throw SystemError
        */
        static void sleep(size_t milliSec);

        static unsigned long usedMemory();

    private:
        class ProcessImpl *_impl;
};


inline ProcessInfo::ProcessInfo(const std::string& command)
: _command(command)
, _detach(false)
, _stdinMode(Close)
, _stdin(0)
, _stdoutMode(Keep)
, _stdout(0)
, _stderrMode(Keep)
, _stderr(0)
{
}


inline const std::string& ProcessInfo::command() const
{
    return _command;
}


inline ProcessInfo& ProcessInfo::addArg(const std::string& argument)
{
    _args.push_back(argument);
    return *this;
}


inline unsigned ProcessInfo::argCount() const
{
    return _args.size();
}


inline const std::string& ProcessInfo::arg(unsigned idx) const
{
    return _args.at(idx);
}

}

#endif // CXXTOOLS_PROCESS_H
