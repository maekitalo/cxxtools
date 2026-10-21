Time zone how-to
==============

cxxtools provides classes for handling time zones. Their main purpose is to
convert date and time values from UTC to a local time zone and vice versa.

cxxtools provides the class `cxxtools::DateTime`, which represents a date and
time. Two classes, `LocalDateTime` and `UtcDateTime`, are derived from it.
They do not add any functionality, but provide additional type safety.
`TzDateTime`, which is derived from `LocalDateTime`, additionally stores time
zone information.

The `cxxtools::Tz` class uses the system time zone database to convert date and
time values between UTC and local time zones.

The default constructor of `Tz` initializes the object with the system time
zone. Alternatively, a different time zone can be passed to the constructor.

The most important methods of `Tz` are `toLocal()` and `toUtc()`.

Here is a simple example:

```cpp
cxxtools::Tz myTimezone;
cxxtools::Tz nauruTimezone("Pacific/Nauru");

auto localtime = cxxtools::DateTime::localtime();
std::cout << "current local time " << localtime.toString() << std::endl;

auto utctime = myTimezone.toUtc(localtime);
std::cout << "current UTC time " << utctime.toString() << std::endl;

auto naurutime = nauruTimezone.toLocal(utctime);
std::cout << "time in Nauru: " << naurutime.toString() << std::endl;
```









Cxxtools offers a class for handling time zones. The main task
is to convert a datetime from UTC to a time zone or back.

Cxxtools has already a class `cxxtools:DateTime`, which represents obviously a
date and a time. From that 2 classes `LocalDateTime` and `UtcDateTime` are
derived, which do not add any functionality but offsers a little safety. In
addition a `class TzDateTime`, which is derived from `LocalDateTime` adds the
time zone information to the class.

The class `cxxtools cxxtools::Tz` uses the system time zone database
to convert the datetimes between UTC and time zone.

The default constructor of Tz initializes it with the system
time zone. Optionally a different time zone can be passed as
a parameter.

The most important methods in Tz are toLocal and toUtc.

A very simple example is here:

```cpp
    cxxtools::Tz myTimezone;
    cxxtools::Tz nauruTimezone("Pacific/Nauru");

    auto localtime = cxxtools::DateTime::localtime();
    std::cout << "current local time " << localtime.toString() << std::endl;
    auto utctime = myTimezone.toUtc(localtime);
    std::cout << "current utc time " << utctime.toString() << std::endl;

    auto naurutime = nauruTimezone.toLocal(utctime);
    std::cuot << "time at nauru: " << naurutime.toString() << std::endl;
```
