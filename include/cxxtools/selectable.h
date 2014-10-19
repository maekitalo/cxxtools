/*
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
#ifndef CXXTOOLS_SELECTABLE_H
#define CXXTOOLS_SELECTABLE_H

#include <cxxtools/signal.h>
#include <cxxtools/selector.h>
#include <cxxtools/timespan.h>

namespace cxxtools
{

class SelectableImpl;

class Selectable
{
        Selectable(const Selectable&) { }
        Selectable& operator=(const Selectable&) { return *this; }

    public:
        static const std::size_t WaitInfinite = Selector::WaitInfinite;

        enum State
        {
            Disabled = 0,
            Idle = 1,
            Busy = 2,
            Avail = 3
        };

    public:
        //! @brief Destructor
        virtual ~Selectable();

        void setSelector(SelectorBase* parent);

        SelectorBase* selector();

        const SelectorBase* selector() const;

        //! @brief Closes the I/O device
        /*!
           Frees any resources associated with this object, like I/O handles.
        */
        void close();

        bool wait(Milliseconds msecs = WaitInfinite);

        //! @brief Test if the I/O device object is enabled
        /*!
            Test if the I/O device object is enabled i.e. open and ready
            to perform I/O operations

            \return true if the I/O device is usable, false otherwise.
        */
        bool enabled() const;

        bool idle() const;

        bool busy() const;

        bool avail() const;

        virtual SelectableImpl& simpl() = 0;

    protected:
        //! @brief Default Constructor
        Selectable();

        //! @brief Sets or unsets the device enabled
        void setEnabled(bool isEnabled);

        //TODO: tell Selector more specifically what changed
        //      add onReinit() to Selector:
        //      pass old state as second arg with onChanged()
        void setState(State state);

        //! @brief Closes the Selector
        virtual void onClose() = 0;

        virtual bool onWait(Timespan timeout) = 0;

        virtual void onAttach(SelectorBase&) = 0;

        virtual void onDetach(SelectorBase&) = 0;

    private:
        SelectorBase* _parent;
        State _state;
};

} // namespace cxxtools

#endif
