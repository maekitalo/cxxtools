////////////////////////////////////////////////////////////////////////
// sudo.cpp
//

#include "cxxtools/sudo.h"

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>

#include <stdexcept>
#include <sstream>

namespace sudo
{
  void set_user(uid_t uid)
  {
    int ret = setuid(uid);
    if (ret != 0)
    {
      std::ostringstream msg;
      msg << "cannot change user to " << uid << ": "
          << strerror(errno);
      throw std::runtime_error(msg.str());
    }
  }

  void set_user(const char* user)
  {
    // wir sind root und können unseren Benutzer wechseln
    struct passwd * pw = getpwnam(user);
    if (pw == 0)
      throw std::runtime_error(std::string("unknown user \"") + user + '"');

    return set_user(pw->pw_uid);
  }

  std::pair<bool, int> fork_wait()
  {
    int pid = ::fork();
    if (pid < 0)
      throw std::runtime_error(
        std::string("error in fork(): ") + strerror(errno));

    if (pid > 0)
    {
      // parent
      int status;
      int ret = waitpid(pid, &status, 0);
      if (ret < 0)
        throw std::runtime_error(
          std::string("error in waitpid(): ") + strerror(errno));

      return std::pair<bool, int>(true, status);
    }
    else
    {
      return std::pair<bool, int>(false, 0);
    }
  }
}
