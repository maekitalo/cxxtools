////////////////////////////////////////////////////////////////////////
// multifstream.cpp
//

#include "cxxtools/multifstream.h"

multifstreambuf::multifstreambuf(const char* pattern, int flags)
  : current(0)
{
  glob(pattern, flags, 0, &mglob);

  if (mglob.gl_pathv && mglob.gl_pathv[current])
    file.open(mglob.gl_pathv[current], std::ios::in);
}

multifstreambuf::~multifstreambuf()
{
  globfree(&mglob);
}

std::streambuf::int_type multifstreambuf::overflow(std::streambuf::int_type c)
{
  return traits_type::eof();
}

std::streambuf::int_type multifstreambuf::underflow()
{
  if (mglob.gl_pathv == 0 || mglob.gl_pathv[current] == 0)
    return traits_type::eof();

  int_type r;
  do
  {
    r = file.sbumpc();
  } while (r == traits_type::eof() && open_next());

  if (r != traits_type::eof())
  {
    ch = static_cast<char>(r);
    setg(&ch, &ch, &ch + 1);
  }

  return r;
}

int multifstreambuf::sync()
{
  return 0;
}

bool multifstreambuf::open_next()
{
  file.close();
  if (mglob.gl_pathv[current] && mglob.gl_pathv[current + 1])
  {
    ++current;
    file.open(mglob.gl_pathv[current], std::ios::in);
    return true;
  }
  else
    return false;
}

