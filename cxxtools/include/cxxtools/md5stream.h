////////////////////////////////////////////////////////////////////////
// md5stream
//

#ifndef MD5STREAM_H
#define MD5STREAM_H

#include <cxxtools/md5.h>
#include <iostream>

class md5streambuf : public std::streambuf
{
  public:
    md5streambuf()
    { }

    void getDigest(unsigned char digest[16]);

  private:
    static const unsigned int bufsize = 64;
    char buffer[bufsize];
    cxxtools_MD5_CTX context;

    std::streambuf::int_type overflow(std::streambuf::int_type ch);
    std::streambuf::int_type underflow();
    std::streambuf::int_type sync();
};

class md5stream : public std::ostream
{
    md5streambuf streambuf;
    char hexdigest[33];

  public:
    md5stream()
      : std::ostream(&streambuf)
    { }

    void getDigest(unsigned char digest[16])
    { streambuf.getDigest(digest); }
    const char* getHexDigest();
};

#endif
