#ifndef cxxtools_Connection_h
#define cxxtools_Connection_h

#include <cxxtools/slot.h>
#include <cxxtools/refcounted.h>

namespace cxxtools {

    class Connectable;


    class ConnectionData : public RefCounted {
        public:
            ConnectionData()
            : _refs(1)
            , _valid(false)
            , _slot(0)
            , _sender(0)
            { }

            ConnectionData(Connectable& sender, Slot* slot)
            : _refs(1)
            , _valid(true)
            , _slot(slot)
            , _sender(&sender)
            { }

            ~ConnectionData()
            { delete _slot; }

            unsigned ref()
            { return ++_refs; }

            unsigned unref()
            { return --_refs; }

            unsigned refs() const
            { return _refs; }

            bool valid() const
            { return _valid; }

            void setValid(bool valid)
            { _valid = valid; }

            Connectable& sender()
            { return *_sender; }

            const Connectable& sender() const
            { return *_sender; }

            Slot& slot()
            { return *_slot; }

            const Slot& slot() const
            { return *_slot; }

        private:
            unsigned _refs;
            bool _valid;
            Slot* _slot;
            Connectable* _sender;
    };


    class Connection
    {
        public:
            Connection();

            Connection(Connectable& sender, Slot* slot);

            Connection(const Connection& connection);

            ~Connection();

            bool valid() const
            { return _data->valid(); }

            const Connectable& sender() const
            { return _data->sender(); }

            const Slot& slot() const
            { return _data->slot(); }

            bool operator!() const
            { return this->valid() == false; }

            void close();

            Connection& operator=(const Connection& connection);

            bool operator==(const Connection& connection) const;

        private:
            ConnectionData* _data;
    };

} // namespace cxxtools

#endif
