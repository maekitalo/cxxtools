#include <cgi.h>
#include <iostream>

int main(int argc, char* argv)
{
  std::string query_string = "uhu&blah&ttt=t1&qqq=q&ttt=t2";
  CGI q(query_string);

  using std::cout;
  using std::endl;

  cout << "query_string=" << query_string << endl;
  cout << "q[\"uhu\"]=" << q["uhu"] << endl;
  cout << "q[\"ttt\"]=" << q["ttt"] << endl;
  cout << "q.param(\"ttt\", 0)=" << q.param("ttt", 0) << endl;
  cout << "q.param(\"ttt\", 1)=" << q.param("ttt", 1) << endl;
}
