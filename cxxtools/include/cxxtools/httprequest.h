////////////////////////////////////////////////////////////////////////
// 
//

#ifndef CXXTOOLS_HTTPREQUEST_H
#define CXXTOOLS_HTTPREQUEST_H

#include <cxxtools/tcpstream.h>
#include <cxxtools/query_params.h>

namespace cxxtools
{
  class httprequest : public std::iostream
  {
    public:
      enum request_type {
        GET, POST
      } type;

    private:
      request_type method;
      std::string host;
      std::string url;
      unsigned short port;

      query_params params;

      tcp::iostream connection;

    public:
      httprequest(const std::string& host_, const std::string& url_, request_type method_ = GET)
        : std::iostream(0),
          method(method_),
          host(host_),
          url(url_)
      {
        init(connection.rdbuf());
      }

      httprequest(const std::string& host_, unsigned short port_, const std::string& url_,
          request_type method_ = GET)
        : std::iostream(0),
          method(method_),
          host(host_),
          url(url_),
          port(port_)
      {
        init(connection.rdbuf());
      }

      query_params& getQueryParams()                            { return params; }
      void set(const std::string name, const std::string value) { params.add(name, value); }
      void set(const std::string value)                         { params.add(value); }

      void execute();
  };

}

#endif // CXXTOOLS_HTTPREQUEST_H

