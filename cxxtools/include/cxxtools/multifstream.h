////////////////////////////////////////////////////////////////////////
// cxxtools/multifstream.h
//

#ifndef CXXTOOLS_MULTIFSTREAM_H
#define CXXTOOLS_MULTIFSTREAM_H

#include <fstream>
#include <glob.h>

class multifstreambuf : public std::streambuf
{
    glob_t mglob;
    unsigned current;
    std::filebuf file;
    char ch;

  public:
    explicit multifstreambuf(const char* pattern, int flags = 0);
    ~multifstreambuf();

    bool open_next();

    int_type overflow(int_type c);
    int_type underflow();
    int sync();

    const char* current_filename() const
      { return mglob.gl_pathv[current]; }
};

class multi_ifstream : public std::istream
{
    multifstreambuf buffer;

  public:
    multi_ifstream(const char* pattern, int flags = 0)
      : std::istream(&buffer),
        buffer(pattern, flags)
    { }

    const char* current_filename() const
      { return buffer.current_filename(); }
};

#endif // CXXTOOLS_MULTIFSTREAM_H

