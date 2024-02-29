/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#include <cxxtools/log.h>
#include <cxxtools/arg.h>
#include <cxxtools/json.h>
#include <cxxtools/serializationinfo.h>
#include <cxxtools/http/server.h>
#include <cxxtools/http/service.h>
#include <cxxtools/http/responder.h>
#include <cxxtools/http/reply.h>
#include <cxxtools/eventloop.h>

#include <thread>
#include <mutex>
#include <iostream>
#include <fstream>

struct ProcStat
{
    ProcStat()
        : user(0),
            nice(0),
            system(0),
            idle(0),
            iowait(0),
            irq(0),
            softirq(0)
            { }

    std::string cpu;
    unsigned user;
    unsigned nice;
    unsigned system;
    unsigned idle;
    unsigned iowait;
    unsigned irq;
    unsigned softirq;
};

std::istream& operator>> (std::istream& in, ProcStat& p)
{
    in >> p.cpu >> p.user >> p.nice >> p.system >> p.idle >> p.iowait >> p.irq >> p.softirq;
    return in;
}

std::ostream& operator<< (std::ostream& out, ProcStat& p)
{
    out << p.cpu << ' ' << p.user << ' ' << p.nice << ' ' << p.system << ' ' << p.idle << ' ' << p.iowait << ' ' << p.irq << ' ' << p.softirq;
    return out;
}

void operator<<= (cxxtools::SerializationInfo& si, const ProcStat& p)
{
    si.addMember("cpu") <<= p.cpu;
    si.addMember("user") <<= p.user;
    si.addMember("nice") <<= p.nice;
    si.addMember("system") <<= p.system;
    si.addMember("idle") <<= p.idle;
    si.addMember("iowait") <<= p.iowait;
    si.addMember("irq") <<= p.irq;
    si.addMember("softirq") <<= p.softirq;
}

ProcStat operator- (const ProcStat& p1, const ProcStat& p2)
{
    ProcStat p;
    p.cpu             = p1.cpu;
    p.user            = p1.user        - p2.user;
    p.nice            = p1.nice        - p2.nice;
    p.system        = p1.system    - p2.system;
    p.idle            = p1.idle        - p2.idle;
    p.iowait        = p1.iowait    - p2.iowait;
    p.irq             = p1.irq         - p2.irq;
    p.softirq     = p1.softirq - p2.softirq;
    return p;
}

ProcStat currentStat;
std::mutex statMutex;

void statThread()
{
    ProcStat p0;
    {
        std::ifstream f("/proc/stat");
        f >> p0;
    }

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::ifstream f("/proc/stat");
        ProcStat p1;
        f >> p1;
        std::lock_guard<std::mutex> lock(statMutex);
        currentStat = p1 - p0;
        p0 = p1;
    }
}

class MainResponder : public cxxtools::http::Responder
{
    public:
        explicit MainResponder(cxxtools::http::Service& service)
            : cxxtools::http::Responder(service)
            { }

        virtual void reply(std::ostream&, cxxtools::http::Request& request, cxxtools::http::Reply& reply);
};

void MainResponder::reply(std::ostream& out, cxxtools::http::Request& request, cxxtools::http::Reply& reply)
{
    reply.addHeader("Content-Type", "text/html");
    out << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n"
                 "<html>\n"
                 " <head>\n"
                 "    <script type='text/javascript'>\n"
                 "        function getRequest()\n"
                 "        {\n"
                 "            try { return new XMLHttpRequest();                                     } catch (e) { }\n"
                 "            try { return new ActiveXObject(\"Msxml2.XMLHttp\");        } catch (e) { }\n"
                 "            try { return new ActiveXObject(\"Microsoft.XMLHTTP\"); } catch (e) { }\n"
                 "            return null;\n"
                 "        }\n"
                 "\n"
                 "        function ajaxGet(url, fn)\n"
                 "        {\n"
                 "            request = getRequest();\n"
                 "            request.open('GET', url);\n"
                 "            request.onreadystatechange = function () {\n"
                 "                    if (request.readyState == 4)\n"
                 "                    {\n"
                 "                        if (request.status == 200)\n"
                 "                            fn(request);\n"
                 "                    }\n"
                 "                }\n"
                 "            request.send(null);\n"
                 "        }\n"
                 "\n"
                 "        function updateStat()\n"
                 "        {\n"
                 "            ajaxGet('/stat',\n"
                 "                function(request)\n"
                 "                {\n"
                 "                    var r = eval('(' + request.responseText + ')');\n"
                 "                    document.getElementById('cpu').innerHTML = r.cpu;\n"
                 "                    document.getElementById('user').innerHTML = r.user;\n"
                 "                    document.getElementById('nice').innerHTML = r.nice;\n"
                 "                    document.getElementById('system').innerHTML = r.system;\n"
                 "                    document.getElementById('idle').innerHTML = r.idle;\n"
                 "                    document.getElementById('iowait').innerHTML = r.iowait;\n"
                 "                    document.getElementById('irq').innerHTML = r.irq;\n"
                 "                    document.getElementById('softirq').innerHTML = r.softirq;\n"
                 "                    document.getElementById('user').innerHTML = r.user;\n"
                 "                });\n"
                 "        }\n"
                 "\n"
                 "        window.setInterval('updateStat()', 1000);\n"
                 "     </script>\n"
                 " </head>\n"
                 "\n"
                 " <body bgcolor='#FFFFFF'>\n"
                 "\n"
                 "    <table>\n"
                 "     <tr><th>cpu</th><td id='cpu'></td></tr>\n"
                 "     <tr><th>user</th><td id='user'></td></tr>\n"
                 "     <tr><th>nice</th><td id='nice'></td></tr>\n"
                 "     <tr><th>system</th><td id='system'></td></tr>\n"
                 "     <tr><th>idle</th><td id='idle'></td></tr>\n"
                 "     <tr><th>iowait</th><td id='iowait'></td></tr>\n"
                 "     <tr><th>irq</th><td id='irq'></td></tr>\n"
                 "     <tr><th>softirq</th><td id='softirq'></td></tr>\n"
                 "    </table>\n"
                 "\n"
                 " </body>\n"
                 "</html>\n";
}

class StatResponder : public cxxtools::http::Responder
{
    public:
        explicit StatResponder(cxxtools::http::Service& service)
            : cxxtools::http::Responder(service)
            { }

        virtual void reply(std::ostream&, cxxtools::http::Request& request, cxxtools::http::Reply& reply);
};

void StatResponder::reply(std::ostream& out, cxxtools::http::Request& request, cxxtools::http::Reply& reply)
{
    std::lock_guard<std::mutex> lock(statMutex);
    reply.addHeader("Content-Type", "application/json");
    out << cxxtools::Json(currentStat);
}

typedef cxxtools::http::CachedService<StatResponder> StatService;
typedef cxxtools::http::CachedService<MainResponder> MainService;

int main(int argc, char* argv[])
{
    try
    {
        log_init();

        cxxtools::Arg<std::string> listenIp(argc, argv, 'l');
        cxxtools::Arg<unsigned short int> listenPort(argc, argv, 'p', 8001);

        cxxtools::EventLoop loop;
        cxxtools::http::Server server(loop, listenIp, listenPort);

        MainService mainService;
        StatService statService;
        server.addService("/", mainService);
        server.addService("/stat", statService);

        std::cout << "http server running on port " << listenPort.getValue() << std::endl;

        std::thread thread(statThread);
        loop.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

