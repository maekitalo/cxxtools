/*
 * Copyright (C) 2024 by Tommi Maekitalo
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

#include <cxxtools/net/bufferedsocket.h>
#include <cxxtools/hexdump.h>
#include <cxxtools/log.h>
#include <cstring>

log_define("cxxtools.net.bufferedsocket")

namespace cxxtools
{
namespace net
{

static const unsigned defaultBufferSize = 8192;

BufferedSocket::BufferedSocket()
    : _bufferSize(defaultBufferSize)
{ }

BufferedSocket::BufferedSocket(SelectorBase& selector)
    : _bufferSize(defaultBufferSize)
{
    setSelector(&selector);
    cxxtools::connect(IODevice::inputReady, *this, &BufferedSocket::onInput);
    cxxtools::connect(IODevice::outputReady, *this, &BufferedSocket::onOutput);
}

BufferedSocket::BufferedSocket(SelectorBase& selector, const TcpServer& server, unsigned flags)
    : TcpSocket(server, flags),
      _bufferSize(defaultBufferSize)
{
    setSelector(&selector);
    cxxtools::connect(IODevice::inputReady, *this, &BufferedSocket::onInput);
    cxxtools::connect(IODevice::outputReady, *this, &BufferedSocket::onOutput);
}

BufferedSocket::BufferedSocket(SelectorBase& selector, const AddrInfo& addrinfo)
    : TcpSocket(addrinfo),
      _bufferSize(defaultBufferSize)
{
    setSelector(&selector);
    cxxtools::connect(IODevice::inputReady, *this, &BufferedSocket::onInput);
    cxxtools::connect(IODevice::outputReady, *this, &BufferedSocket::onOutput);
}

BufferedSocket::BufferedSocket(SelectorBase& selector, const std::string& ipaddr, unsigned short int port)
    : BufferedSocket(selector, AddrInfo(ipaddr, port))
{ }

BufferedSocket::~BufferedSocket()
{
    if (_sentry)
        _sentry->detach();
}

size_t BufferedSocket::onEndRead(bool& eof)
{
    auto count = IODevice::onEndRead(eof);

    log_debug("onEndRead; " << count << " bytes received");
    if (_inputBuffer.empty())
    {
        _inputBuffer.swap(_inputBufferCurrent);
        _inputBuffer.resize(count);
    }
    else
    {
        _inputBuffer.insert(_inputBuffer.end(), _inputBufferCurrent.begin(), _inputBufferCurrent.begin() + count);
    }

    log_debug(_inputBuffer.size() << " bytes available");

    return count;
}

void BufferedSocket::onInput(IODevice&)
{
    inputReady(*this);
}

void BufferedSocket::beginRead()
{
    // try to reuse capacity
    if (_inputBufferCurrent.capacity() < _bufferSize && _inputBuffer.empty() && _inputBuffer.capacity() >= _bufferSize)
        _inputBufferCurrent.swap(_inputBuffer);

    _inputBufferCurrent.resize(_bufferSize);

    log_debug("beginRead " << _inputBufferCurrent.size());
    TcpSocket::beginRead(_inputBufferCurrent.data(), _inputBufferCurrent.size());
}

unsigned BufferedSocket::read()
{
    // try to reuse capacity
    if (_inputBufferCurrent.capacity() < _bufferSize && _inputBuffer.empty() && _inputBuffer.capacity() >= _bufferSize)
        _inputBufferCurrent.swap(_inputBuffer);

    _inputBufferCurrent.resize(_bufferSize);

    log_debug("beginRead " << _inputBufferCurrent.size());
    return TcpSocket::read(_inputBufferCurrent.data(), _inputBufferCurrent.size());
}

void BufferedSocket::onOutput(IODevice&)
{
    DestructionSentry sentry(_sentry);

    try
    {
        auto count = endWrite();
        log_finer("written\n" << cxxtools::hexDump(_outputBuffer.begin(), _outputBuffer.begin() + count));
        _outputBuffer.erase(_outputBuffer.begin(), _outputBuffer.begin() + count);

        if (_outputBuffer.empty())
        {
            if (_outputBufferNext.empty())
                outputBufferEmpty(*this);
            else
                _outputBuffer.swap(_outputBufferNext);
        }
        else if (!_outputBufferNext.empty())
        {
            _outputBuffer.insert(_outputBuffer.end(), _outputBufferNext.begin(), _outputBufferNext.end());
            _outputBufferNext.clear();
        }

        if (!sentry.deleted() && !writing() && !_outputBuffer.empty())
            beginWrite();
    }
    catch (const std::exception& e)
    {
        if (!sentry.deleted())
            outputFailed(*this, e);
    }
}

BufferedSocket& BufferedSocket::write(const char* buffer, size_t n, bool begin)
{
    if (writing())
    {
        _outputBufferNext.insert(_outputBufferNext.end(), buffer, buffer + n);
    }
    else
    {
        _outputBuffer.insert(_outputBuffer.end(), buffer, buffer + n);
        if (begin)
            beginWrite();
    }

    return *this;
}

BufferedSocket& BufferedSocket::write(const char* str, bool begin)
{
    return write(str, std::strlen(str), begin);
}

BufferedSocket& BufferedSocket::putc(char ch)
{
    if (writing())
        _outputBufferNext.push_back(ch);
    else
        _outputBuffer.push_back(ch);

    return *this;
}

BufferedSocket& BufferedSocket::beginWrite()
{
    if (!writing() && !_outputBuffer.empty())
        TcpSocket::beginWrite(_outputBuffer.data(), _outputBuffer.size());

    return *this;
}

BufferedSocket& BufferedSocket::flush()
{
    log_debug("flush " << _outputBuffer.size() << '+' << _outputBufferNext.size());

    if (writing())
        endWrite();

    while (!_outputBuffer.empty())
    {
        beginWrite();
        auto count = endWrite();
        log_finer("written\n" << cxxtools::hexDump(_outputBuffer.begin(), _outputBuffer.begin() + count));
        _outputBuffer.erase(_outputBuffer.begin(), _outputBuffer.begin() + count);
    }

    _outputBuffer.swap(_outputBufferNext);

    while (!_outputBuffer.empty())
    {
        beginWrite();
        auto count = endWrite();
        log_finer("written\n" << cxxtools::hexDump(_outputBuffer.begin(), _outputBuffer.begin() + count));
        _outputBuffer.erase(_outputBuffer.begin(), _outputBuffer.begin() + count);
    }

    return *this;
}

void BufferedSocket::cancel()
{
    IODevice::cancel();
    _inputBuffer.clear();
    _outputBuffer.clear();
    _outputBufferNext.clear();
}

}
}
