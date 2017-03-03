/*
 * Copyright (C) 2006-2008 Marc Boris Duerner
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
#ifndef CXXTOOLS_APPLICATION_H
#define CXXTOOLS_APPLICATION_H

#include <cxxtools/eventloop.h>
#include <cxxtools/application.h>
#include <cxxtools/connectable.h>
#include <cxxtools/event.h>
#include <cxxtools/signal.h>

namespace cxxtools {

    class ApplicationImpl;

    /**
     * \brief The Application class provides an event loop for console applications
     * without a GUI.
     *
     * This class is used by non-GUI applications to provide the applications's event
     * loop. There should be only exactly one instance of Application (or one of its
     * subclasses) per application. This is not ensured, though.
     *
     * Application contains the main event loop, where event sources can be registered
     * and events from those sources are dispatched to listeners, that were registered
     * to the event loop. Events may for example be operating system events (timer, file
     * system changes).
     *
     * The application and therefore the event loop is started with a call to run() and
     * can be exited with a call to exit(). After calling exit() the application should
     * terminate.
     *
     * The event loop can be access by calling eventLoop(). Events can be committed by
     * calling EventLoop::commitEvent(). Long running operations can call
     * EventLoop::processEvents() to keep the application responsive.
     *
     * There are convenience methods available for easier access to functionality of
     * the underlying event loop. commitEvent() delegates to EventLoop::commitEvent(),
     * queueEvent() delegates to EventLoop::queueEvent() and processEvents() delegates
     * to EventLoop::processEvents() without making it necessary to first obtain the
     * event loop manually.
     */
    class Application : public Connectable
    {
            void construct();

        public:
            Application();

            Application(int argc, char** argv);

            Application(EventLoopBase* loop);

            Application(EventLoopBase* loop, int argc, char** argv);

            ~Application();

            static Application& instance();

            EventLoopBase& loop()
            { return *_loop; }

            void run()
            { _loop->run(); }

            void exit()
            { _loop->exit(); }

            bool catchSystemSignal(int sig);

            bool raiseSystemSignal(int sig);

            Signal<int> systemSignal;

            int argc() const
            { return _argc; }

            int& argc()
            { return _argc; }

            const char* const* argv() const
            { return _argv; }

            char** argv()
            { return _argv; }

            ApplicationImpl& impl()
            { return *_impl; }

            /// Puts a event in the queue but does not wake the loop.
            void queueEvent(const Event& event)
            { loop().queueEvent(event); }

            /// Puts a event in the queue and wakes the loop so that the event is processed.
            void commitEvent(const Event& event)
            { loop().commitEvent(event); }

            /// Puts a priority event in the queue but does not wake the loop.
            /// Priority events are processed before any non priority event.
            void queuePriorityEvent(const Event& event)
            { loop().queuePriorityEvent(event); }

            /// Puts a priority event in the queue and wakes the loop so that the event is processed.
            /// Priority events are processed before any non priority event.
            void commitPriorityEvent(const Event& event)
            { loop().commitPriorityEvent(event); }

        protected:
            void init(EventLoopBase& loop);

        private:
            ApplicationImpl* _impl;
            int     _argc;
            char**  _argv;
            EventLoopBase* _loop;
            EventLoop* _owner;
    };

} // namespace cxxtools

#endif
