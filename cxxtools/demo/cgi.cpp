#include <exception>
#include <iostream>
#include <cxxtools/cgi.h>
#include <cxxtools/arg.h>

using cxxtools::cgi;
using cxxtools::xmltag;

int cgidemo()
{
  cgi q;  // this parses all input-parameters

  // print standard-header
  std::cout << q.header();

  xmltag html("html");  // start html-block
  xmltag body("body");  // start body-block

  xmltag form("form");  // start form
  std::cout << "<input type=\"text\" name=\"v\">"
               "<br>"
               "<input type=\"submit\">";
  form.close(); // alternative 1: explicitly trigger closing form-tag

  std::cout << "<hr>\n"
               "you entered: ";

  {
    xmltag bold("b");  // bold here in an embedded scope
    std::cout << q["v"];
  } // alternatvie 2: implicitly closing bold-tag by leaving scope

  // automatically close body and html by leaving scope

  return 0;
}

int tagstest()
{
  xmltag("html");  // prints <html></html>
  xmltag("test param=1");  // prints <test param=1></test>
  xmltag("<noparam>");     // prints <noparam></noparam>
  xmltag("<param p=1>");   // prints <param p=1></param>
  xmltag("<param", "p=1>");   // prints <param p=1></param>
  xmltag("<br>").clear();  // prints <br>

  std::cout << std::endl;
  return 0;
}

int main(int argc, char* argv[])
{
  cxxtools::arg<bool> do_tagstest(argc, argv, 't');
  return  do_tagstest ? tagstest()
                      : cgidemo();
}
