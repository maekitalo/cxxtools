////////////////////////////////////////////////////////////////////////
// pollclass.h
//
#ifndef POLLCLASS_H
#define POLLCLASS_H

#include <map>
#include <poll.h>

////////////////////////////////////////////////////////////////////////
// Wrapper für poll-Systemfunktion
//
class Poller
{
  public:
    ////////////////////////////////////////////////////////////////////
    // PollClient-Interface
    //
    class ClientIf
    {
      public:
        virtual short GetEventMask() const = 0;
        virtual void  PollEvent(short events) = 0;
    };

    Poller()
      : m_fdsptr(0),
        m_fdsptr_size(0)
      { }

    ~Poller()
    {
      delete[] m_fdsptr;
    }

    int Poll(int timeout = -1);

    void add(ClientIf* pc, int fd);
    void forget(ClientIf* pc, int fd);

  private:
    // noncopyable:
    Poller(const Poller&);
    Poller& operator= (const Poller&);

    typedef std::map<int, ClientIf*> map_type;
    map_type m_clientmap;

    struct pollfd*      m_fdsptr;
    map_type::size_type m_fdsptr_size;
};

////////////////////////////////////////////////////////////////////////
// PollClient-Implementation
//
class PollClient : public Poller::ClientIf
{
  public:
    PollClient(Poller& p, int fd, short events);
    virtual ~PollClient();

    short GetEventMask() const;

    void setEventMask(short events)
    { m_events = events; }

  private:
    Poller& m_poller;
    int     m_fd;
    short   m_events;
};

#endif // POLLCLASS_H
