/*
 * Copyright (C) 2017 Tommi Maekitalo
 *
 */

#include <iostream>
#include <cxxtools/eventloop.h>
#include <cxxtools/iostream.h>
#include <cxxtools/stddevice.h>
#include <cxxtools/log.h>

log_define("insrv")

cxxtools::EventLoop loop;

void onInput(cxxtools::StreamBuffer& sb)
{
    try
    {
        sb.endRead();
    }
    catch (const std::exception& e)
    {
        log_info("read failed with error: " << e.what());
        loop.exit();
        return;
    }

    log_debug("onInput; in_avail=" << sb.in_avail());

    if (sb.in_avail() == 0 || sb.device()->eof())
    {
        log_debug("exit loop");
        loop.exit();
        return;
    }

    while (sb.in_avail())
    {
        char ch = sb.sbumpc();
        std::cout << ch;
    }

    std::cout.flush();

    log_debug("read");
    sb.beginRead();
}

int main(int argc, char* argv[])
{
    try
    {
        log_init();

        cxxtools::StdinDevice indev;
        indev.setSelector(&loop);
        cxxtools::IOStream in(indev);
        in.buffer().beginRead();

        cxxtools::connect(in.buffer().inputReady, onInput);

        loop.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

