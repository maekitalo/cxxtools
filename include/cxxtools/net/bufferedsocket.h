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

#ifndef CXXTOOLS_NET_BUFFEREDSOCKET_H
#define CXXTOOLS_NET_BUFFEREDSOCKET_H

#include <cxxtools/net/tcpsocket.h>
#include <cxxtools/connectable.h>
#include <cxxtools/destructionsentry.h>
#include <vector>
#include <exception>

namespace cxxtools
{
namespace net
{
/** @brief tcp socket with buffered reading and writing

    Reading and writing should be made in larger chunks for better performance.
    This class helps implementing it. It manages a read and write buffer, which
    holds the data to be written or which is read.

    The BufferedSocket must run in a event loop to make sure, it can handle
    reading and writing accordingly.

    As with IODevice finished reading and writing are signaled with cxxtools::Signal.

    The signal inputReady is sent, when the input buffer contains data. The user
    should then process the data. He may choose to keep the data until more data
    arrives. The data is not discarded after signalling inputReady.

    Writing is done in the background. More data can be added always if needed.

    Note that the bufferes are not discarded when the socket is disconnected.
    After connecting the write operation is not resumed but must be manually
    restarted if desired.
 */
class BufferedSocket : public TcpSocket, public Connectable
{
    unsigned _bufferSize;

    std::vector<char> _inputBufferCurrent;
    std::vector<char> _inputBuffer;
    std::vector<char> _outputBuffer;
    std::vector<char> _outputBufferNext;
    DestructionSentryPtr _sentryPtr;
    std::exception_ptr _inputException;

    void onInput(IODevice&);
    size_t onEndRead(bool& eof) override;
    void onOutput(IODevice&);


public:
    BufferedSocket();

    /// Constructor; creates a BufferedSocket, which is not connected.
    explicit BufferedSocket(SelectorBase& selector);

    /// Accepts a pending connection from a TcpServer.
    BufferedSocket(SelectorBase& selector, const TcpServer& server, unsigned flags = 0);

    /// Connects to a peer.
    BufferedSocket(SelectorBase& selector, const AddrInfo& addrinfo);

    /// Connects to a peer.
    BufferedSocket(SelectorBase& selector, const std::string& ipaddr, unsigned short int port);

    /// returns the current input buffer size.
    unsigned bufferSize() const         { return _bufferSize; }
    /// sets the current input buffer size.
    void bufferSize(unsigned b)         { _bufferSize = b; }

    /** Adds data to the output buffer.
        
        Calling this method adds data to the output buffer.  Writing is not
        started. In asynchronous mode the user needs to call beginWrite to
        trigger writing. In synchronous mode flush writes the data to the
        socket.

     */
    BufferedSocket& put(const char* buffer, size_t n);

    /** Adds a null terminated string to the output buffer.
     */
    BufferedSocket& put(const char* str);

    /** Adds data to the output buffer.
     */
    BufferedSocket& put(const std::string& buffer);

    /** Adds a single character to the output buffer.
     */
    BufferedSocket& put(char ch)
        { outputBuffer().push_back(ch); return *this; }

    /** Adds data to the output buffer and starts writing.

        Calling this method adds data to the output buffer and starts writing
        if not already pending.  When a write operation is finished and data is
        left in the output buffer writing continues until the write buffer
        is empty.  The user may always add more data to the output buffer.
        The class takes care, that all data from the output buffer is
        written.

     */
    BufferedSocket& write(const char* buffer, size_t n)
        { put(buffer, n); beginWrite(); return *this; }

    /** Adds a null terminated string to the output buffer and starts writing.
     */
    BufferedSocket& write(const char* str)
        { put(str); beginWrite(); return *this; }

    /** Adds data to the output buffer and starts writing.
     */
    BufferedSocket& write(const std::string& str)
        { put(str); beginWrite(); return *this; }

    /** Returns a output buffer, where user can add data to.

        This gives direct access to the buffer, which may reduce copy operations.
     */
    std::vector<char>& outputBuffer() { return writing() ? _outputBufferNext : _outputBuffer; }

    /** Initiates write operation if not already pending.

        The user can choose to fill the buffer without starting the write
        operation and call beginWrite when the output buffer is filled.
        Calling beginWrite while a write operation is already running
        does nothing.

        When writing fails, the signal outpuFailed is issued from within the
        catch block.
     */
    BufferedSocket& beginWrite();

    /** Initiates write operation if not already pending.
     */
    void beginRead();

    unsigned read();

    /// Cancels reading and writing operations and discards buffers.
    void cancel();

    /// write output buffer with possibly blocking
    BufferedSocket& flush();

    /// Returns the number of bytes in the output buffer, which are not yet written
    unsigned outputSize() const         { return _outputBuffer.size() + _outputBufferNext.size(); }

    /** Returns the input buffer.

        The user is responsible to process the data in the input buffer and also
        empty it. For this the inputReady signal is used to tell, that the user
        needs to collect the received data.

        When the input buffer is not explicitly emptied, the data is kept and
        new data is appended to the buffer. The user can even remove just a
        part of the input buffer when needed.
     */
    std::vector<char>& inputBuffer()    { return _inputBuffer; }

    /// Signals, that the input buffer is filled with new data.
    Signal<BufferedSocket&> inputReady;

    /// Signals, that all data from the output buffer is written.
    Signal<BufferedSocket&> outputBufferEmpty;

    /** Signals, that output failed with a exception

        The signal is issued from within the catch block so the user
        may rethrow the exception when desired.
     */
    Signal<BufferedSocket&, const std::exception&> outputFailed;
};
}
}

#endif
