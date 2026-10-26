Event loop
==========

Many applications need an event loop, which handles events such as
network or timer events. It can also receive events from other
threads in a thread-safe manner.

cxxtools provides an event loop implemented by the class
`cxxtools::EventLoop`. Before describing it, we have to understand
the signal/slot mechanism.

Signal/Slot
-----------

When an event occurs, cxxtools emits a signal. A signal can be connected to one
or more slots in the application. A signal is represented by
`cxxtools::Signal`. Like a function, a signal can have parameters.

A slot is the target for a signal. It has to match the signature of the signal
it is connected to. A slot can be a free function, a member function, a
std::function or a lambda.

A signal is connected to a slot using the function `cxxtools::connect`. The
function comes in several variants and takes either two or three parameters.
The first parameter is a signal. The two-parameter variant of connect expects
a function pointer, a std::function or a lambda. When connecting to a member
function, the second parameter is a reference to an object and the third a
pointer to a member function. The object has to be derived from
`cxxtools::Connectable`, which manages the lifetime of the connection. So if
the object is deleted, the connection is also removed.

The return type of the slot must be `void`.

### Example - free function

```cpp

#include <cxxtools/signal.h>
#include <iostream>

cxxtools::Signal<const std::string&> helloSignal;

void helloSlot(const std::string& who)
{
    std::cout << "Hello " << who << std::endl;
}

int main(int argc, char* argv[])
{
    cxxtools::connect(helloSignal, helloSlot);
    helloSignal("World");
}

```

Calling a signal invokes all connected slots in the order in which they
were connected.

Signals and slots are the foundation of the cxxtools event model. The EventLoop
emits signals when I/O or timer events occur, and application code reacts by
connecting appropriate slots. To process events from other threads, a callback
must be registered using the `subscribe` method.

### Example - lambda

```cpp

#include <cxxtools/signal.h>
#include <iostream>

cxxtools::Signal<const std::string&> helloSignal;

int main(int argc, char* argv[])
{
    cxxtools::connect(helloSignal, [](const std::string& who) {
    std::cout << "Hello " << who << std::endl;
    });
    helloSignal("World");
}

```

The same signal can also be connected to a lambda expression or to a
member function of a class derived from cxxtools::Connectable.

### Example - member function

```cpp

#include <cxxtools/signal.h>
#include <iostream>

cxxtools::Signal<const std::string&> helloSignal;

class Hello : public cxxtools::Connectable
{
public:
    void onHello(const std::string& who);
};

void Hello::onHello(const std::string& who)
{
    std::cout << "Hello " << who << std::endl;
}

int main(int argc, char* argv[])
{
    Hello hello;
    cxxtools::connect(helloSignal, hello, &Hello::onHello);
    helloSignal("World");
}

```

Event loop
----------

The class `cxxtools::EventLoop` is instantiated without parameters. To start
the event loop the application has to call the method `run()`. It dispatches
I/O, timer, and custom events to the registered handlers.

Timer
-----

Applications typically need timers, which emit events at specific
times in the future. cxxtools provides a class `cxxtools::Timer` in the header
`<cxxtools/timer.h>` which implements that.

The timer object is initialized with the event loop in which it should run.

The timer can be run in fixed intervals or as a one-time timer.
When the timer is set, the previous mode is discarded.

When running in fixed intervals, the application can pass a start date and time
at which the interval should begin. If the date and time is in the past, the
next cycle is calculated. That way the timer can run at fixed times, e.g. every
full minute instead of every minute.

The timer emits the signals `timeout` and `timeoutts`. `timeoutts` passes the
scheduled timestamp of the timer event as a parameter.

### Examples

Starting a timer every 10 seconds:

```cpp
int main(int argc, char* argv[])
{
    cxxtools::EventLoop loop;
    cxxtools::Timer timer(&loop);
    cxxtools::connect(timer.timeout, [](){
        std::cout << "timer elapsed" << std::endl;
    });
    timer.start(cxxtools::Seconds(10));
    loop.run();
}
```

In the following examples we omit the main function and show
just, how to start the timer.

Issuing the timer every full minute:

```cpp
    timer.start(cxxtools::DateTime(0, 1, 1, 0, 0, 0), cxxtools::Minutes(1));
```

Issuing the timer once at a specific point in time:

```cpp
    timer.at(cxxtools::DateTime(2026, 10, 26, 15, 30, 0));
```

Issuing the timer once after a delay:

```cpp
    timer.after(cxxtools::Milliseconds(500));
```

The timer can be reinitialized whenever needed in which case the previous mode
is replaced.

Thread-Events / Cross-thread Events
-----------------------------------

In multithreaded applications we typically have a main event loop and worker
threads. The worker threads sometimes need to send data to the main event loop.
This can be achieved by using the method `commitEvent(const Event&)` of the
main event loop. That method is safe to be called from a different thread and
wakes up the event loop.

An event object must be derived from `cxxtools::BasicEvent` with the class as a
template parameter, e.g. `class MyEvent : public cxxtools::BasicEvent<MyEvent>`
and must be copyable.

To receive the event in the event loop, the template method `subscribe` must be
used to register a callback. The method expects a reference to a class and a
member function to that ojbect's class with a const reference to the event type
as parameter. The event loop will call that method when the event of that type
is committed.

### Example:

```cpp
class MyEvent : cxxtools::BasicEvent<MyEvent>
{
    int _value;
public:
    explicit MyEvent(int value)
        : _value(value)
        { }
    MyEvent(const MyEvent&) = default;
    int value() const   { return _value; }

};
```

In the main application:

```cpp

cxxtools::EventLoop loop;

{
    class MyApplication
    {
    public:
        void onMyEvent(const MyEvent& ev)
        {
            std::cout << "my event with value " << ev.value() << " occurred" << std::endl;
        }
    };

    MyApplication app;
    loop.subscribe(app, &MyApplication::onMyEvent);
    loop.run();
}
```

In a worker thread:
```cpp

void foo()
{
    loop.commitEvent(MyEvent(42));
}

```
