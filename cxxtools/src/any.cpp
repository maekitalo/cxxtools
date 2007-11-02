#include "cxxtools/any.h"
#include <algorithm>
#include <utility>

namespace cxxtools {

Any::Any()
: _value(0)
{ }


Any& Any::assign(Value* value)
{
    if(_value)
        delete _value;

    _value = value;
    return *this;
}


Any::Any(const Any& val)
: _value(0)
{
    _value = val._value ? val._value->clone() : 0;
}


Any::~Any()
{
    if(_value)
        delete _value;
}


void Any::clear()
{
    if(_value) {
        delete _value;
        _value = 0;
    }
}


Any& Any::swap(Any& rhs)
{
    std::swap(_value, rhs._value);
    return *this;
}


Any& Any::operator=(const Any& rhs)
{
    Any(rhs).swap(*this);
    return *this;
}


bool Any::operator==(const Any& a) const
{
    if(_value && a._value)
    {
        return _value->equal( *(a._value) );
    }

    // if one or both of the Anys is not initialised
    // they are considered equal if both have NULL values.
    return _value == a._value;
}


bool Any::operator!=(const Any& a) const
{
    return !( this->operator==(a) );
}


bool Any::operator<(const Any& a) const
{
    if(_value && a._value)
    {
        return _value->lt( *(a._value) );
    }

    // if one of the Anys is not initialised the
    //one having a NULL valueis considered less.
    return _value < a._value;
}

} // namespace cxxtools
