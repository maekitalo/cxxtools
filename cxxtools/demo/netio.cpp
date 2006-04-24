/* netio.cpp
   network-benchmark
   Copyright (C) 2003-2005 Tommi Maekitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#include <cxxtools/tcpstream.h>
#include <cxxtools/arg.h>
#include <cxxtools/dynbuffer.h>
#include <cxxtools/loginit.h>
#include <sys/time.h>
#include <stdlib.h>

const unsigned BUFSIZE = 65536;

////////////////////////////////////////////////////////////////////////
// Server
//
int server(int argc, char* argv[])
{
  cxxtools::Arg<unsigned short> port(argc, argv, 'p', 1234);
  cxxtools::Arg<const char*> ip(argc, argv, 'i', "0.0.0.0");
  cxxtools::Arg<bool> verbose(argc, argv, 'v');

  cxxtools::net::Server server(ip.getValue(), port);

  while (1)
  {
    cxxtools::net::Stream worker(server);

    std::cout << "connection accepted" << std::endl;

    char buffer[BUFSIZE];
    cxxtools::net::Stream::size_type count = 0;
    cxxtools::net::Stream::size_type n = 0;
    while ( (n = worker.read(buffer, BUFSIZE)) > 0)
    {
      count += n;
      if (verbose)
        std::cout << '.' << std::flush;
    }

    if (verbose)
      std::cout << std::endl
                << (count / 1024) << " Kbytes received" << std::endl;
    std::cout << "test ready " << (count / 1024) << " Kbytes received" << std::endl;
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////
// Client
//
void run_test(cxxtools::net::Stream& conn, unsigned bs, const char* buffer, unsigned secs)
{
  std::cout << "bs " << bs << std::flush;

  timeval start;
  gettimeofday(&start, 0);
  timeval end = start;
  end.tv_sec += secs;

  timeval current;

  cxxtools::net::Stream::size_type count = 0;
  while (1)
  {
    gettimeofday(&current, 0);
    if (current.tv_sec > end.tv_sec
      || current.tv_sec == end.tv_sec
        && current.tv_usec >= end.tv_usec)
      break;

    count += conn.write(buffer, bs);
  }

  double dstart = start.tv_sec + start.tv_usec / 1e6;
  double dend = current.tv_sec + current.tv_usec / 1e6;
  double dur = dend - dstart;
  double bps = count / dur;

  std::cout << " kBps=" << static_cast<unsigned>(bps / 1024) << std::endl;
}

int client(int argc, char* argv[])
{
  cxxtools::Arg<unsigned short> port(argc, argv, 'p', 1234);
  cxxtools::Arg<const char*> ip(argc, argv, 'i', "127.0.0.1");
  cxxtools::Arg<unsigned> secs(argc, argv, 't', 1);
  cxxtools::Arg<unsigned> bufsize(argc, argv, 't', BUFSIZE);
  cxxtools::Arg<unsigned> B(argc, argv, 'B', 0);

  cxxtools::net::Stream conn(ip.getValue(), port);

  cxxtools::Dynbuffer<char> buffer(bufsize);
  std::generate(buffer.begin(), buffer.end(), rand);

  std::cout << "test" << std::endl;

  if (B.isSet())
    run_test(conn, B, buffer.data(), secs);
  else
  {
    for (unsigned bs=256; bs <= bufsize.getValue(); bs <<= 1)
      run_test(conn, bs, buffer.data(), secs);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////
// main
//
int main(int argc, char* argv[])
{
  try
  {
    log_init();
    cxxtools::Arg<bool> isServer(argc, argv, 's');

    if (isServer)
      return server(argc, argv);
    else
      return client(argc, argv);
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}
