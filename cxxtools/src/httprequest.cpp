
#include <cxxtools/httprequest.h>

namespace cxxtools
{
  void httprequest::execute()
  {
    connection.Connect(host, port);
    switch (method)
    {
      case GET:
        connection << "GET ";
        if (url.size() == 0 || url.at(0) != '/')
          connection << '/';
        connection << url;

        if (!params.empty())
          connection << '?' << params.getUrl();

        connection << " HTTP/1.0\r\n\r\n" << std::flush;
        break;

      case POST:
        {
          std::string p = params.getUrl();

          connection << "POST ";
          if (url.size() == 0 || url.at(0) != '/')
            connection << '/';

          connection << url << " HTTP/1.0\r\n"
                        "Content-Length: " << p.size() << "\r\n"
                        "\r\n"
                     << p << std::flush;
        }
        break;
    }
  }
}
