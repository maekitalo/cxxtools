#include <md5stream.h>

int main()
{
  md5stream s;
  char digest[33];
  s << "Hallo";
  std::cout << s.getHexDigest() << '\t' << "Hallo" << std::endl;
}
