#include <cxxtools/signal.h>
#include <iostream>

int function()
{
  std::cout << "function called." << std::endl;
  return 0;
}

struct Object : public cxxtools::Connectable
{
  bool method()
  {
    std::cout << "Object::method called." << std::endl;
    return true;
  }

  char constMethod() const
  {
    std::cout << "Object::constMethod called." << std::endl;
    return 'r';
  }

  static std::string staticMethod()
  {
    std::cout << "Object::staticMethod called." << std::endl;
    return "test";
  }
};


int main(int argc, char* argv[])
{
  try
  {
    Object obj;
    cxxtools::Signal<> signal;
    signal.connect( cxxtools::slot(function) );
    signal.connect( cxxtools::slot(obj, &Object::method) );
    signal.connect( cxxtools::slot(obj, &Object::constMethod) );
    signal.connect( cxxtools::slot(&Object::staticMethod) );

    signal.send();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
