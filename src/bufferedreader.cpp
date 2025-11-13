#include <cxxtools/bufferedreader.h>
#include <cxxtools/iodevice.h>

namespace cxxtools
{
void BufferedReader::uflow()
{
    _inputBuffer.resize(_bufsize);
    auto n = _device.read(_inputBuffer.data(), _bufsize);
    _inputBuffer.resize(n);
    _getp = 0;
}

unsigned BufferedReader::read(char* data, unsigned count)
{
    auto remaining = count;
    while (remaining > 0)
    {
        if (!in_avail())
            uflow();
        if (_device.eof())
        {
            if (remaining < count)
                _device.setEof(false);
            return count - remaining;
        }

        auto toCopy = std::min(remaining, in_avail());
        std::copy(gptr(), gptr() + toCopy, data);
        _getp += toCopy;
        data += toCopy;
        remaining -= toCopy;
    }

    return count;
}

char BufferedReader::peek()
{
    if (!in_avail())
        uflow();
    if (_device.eof())
        return '\0';
    return *gptr();
}

char BufferedReader::get()
{
    char ch = peek();
    if (!_device.eof())
        ++_getp;
    return ch;
}

}
