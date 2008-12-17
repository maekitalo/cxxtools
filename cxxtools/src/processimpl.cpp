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
#include "processimpl.h"
#include "iodeviceimpl.h"

#include <cstdlib>
#include <vector>
#include <signal.h>
#include <errno.h>
#include <cstring> // strerror()
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

namespace cxxtools {

ProcessImpl::ProcessImpl(const ProcessInfo& procInfo)
: _state(Process::Ready)
, _procInfo(procInfo)
{
}


ProcessImpl::~ProcessImpl()
{
}


void ProcessImpl::start()
{
    _state = Process::Running;
    m_pid = fork();

    if( m_pid < 0 )
    {
        m_pid = -1;
        _state = Process::Failed;
        throw SystemError( CXXTOOLS_ERROR_MSG("fork failed") );
    }

    if( m_pid == 0) // child Process
    {
        if( _procInfo.stdInputClosed() )
        {
            fclose(stdin);
        }
        else if(_procInfo.stdInput() )
        {
            dup2(_procInfo.stdInput()->ioimpl().fd(), STDIN_FILENO);
        }

        if( _procInfo.stdOutputClosed() )
        {
            fclose( stdout);
        }
        else if( _procInfo.stdOutput() )
        {
            dup2(_procInfo.stdOutput()->ioimpl().fd(), STDOUT_FILENO);
        }

        if( _procInfo.stdErrorClosed() )
        {
            fclose(stderr);
        }
        else if( _procInfo.stdError() )
        {
            dup2(_procInfo.stdError()->ioimpl().fd(), STDERR_FILENO);
        }

        std::vector< std::vector<char> > args;

        const std::string& c = _procInfo.command();
        std::vector<char> cmd( c.begin(), c.end() );
        cmd.push_back('\0');
        args.push_back(cmd);

        for( unsigned i = 0; i < _procInfo.argCount(); i++)
        {
            const std::string& a = _procInfo.arg(i);
            std::vector<char> arg(a.begin(), a.end());
            arg.push_back('\0');
            args.push_back(arg);
        }

        std::vector<char*> argptrs;
        for( unsigned n = 0; n < args.size(); n++)
        {
            std::vector<char>& a = args[n];
            argptrs.push_back( &a[0] );
        }
        argptrs.push_back( 0 );

        if( 0 > execvp(argptrs[0], &argptrs[0]))
        {
            std::exit(-1);
        }
    }

    return;
}


void ProcessImpl::kill()
{
    if( 0 > ::kill(m_pid,SIGINT) )
    {
        throw SystemError(std::strerror(errno),CXXTOOLS_SOURCEINFO);
    }
    if( m_pid != ::wait(NULL) )
    {
        throw SystemError( CXXTOOLS_ERROR_MSG("kill failed") );
    }
}


int ProcessImpl::wait()
{
    int iStatus;
    if( 0 > waitpid(m_pid,&iStatus,WUNTRACED) )
    {
        _state = Process::Failed;
        throw SystemError( CXXTOOLS_ERROR_MSG("waitpid failed") );
    }

    _state = Process::Finished;
    return iStatus;
}


bool ProcessImpl::tryWait(int& status)
{
    pid_t ret = waitpid(m_pid, &status, WUNTRACED|WNOHANG);
    if( 0 > ret)
    {
        _state = Process::Failed;
        throw SystemError(std::strerror(errno), CXXTOOLS_SOURCEINFO);
    }

    if(ret == 0)
        return false;

    _state = Process::Finished;
    return true;
}


void ProcessImpl::setEnvVar(const std::string& name, const std::string& value)
{
    if( 0 > setenv(name.c_str(),value.c_str(),1) )
    {
        throw SystemError( CXXTOOLS_ERROR_MSG("setenv failed") );
    }
}


void ProcessImpl::unsetEnvVar(const std::string& name)
{
    unsetenv( name.c_str() );
}


std::string ProcessImpl::getEnvVar(const std::string& name)
{
    std::string ret;
    const char* cp = std::getenv(name.c_str());
    if( NULL == cp )
    {
        return ret;
    }
    ret = cp;
    return ret;
}


void ProcessImpl::sleep(size_t milliSec)
{
    usleep(milliSec*1000);
}


unsigned long ProcessImpl::usedMemory()
{
    struct rusage usage;
    int r =  getrusage(RUSAGE_SELF, &usage);
    if( r == -1)
        throw SystemError( CXXTOOLS_ERROR_MSG("getrusage failed") );

    return usage.ru_idrss;
}

} // namespace cxxtools

