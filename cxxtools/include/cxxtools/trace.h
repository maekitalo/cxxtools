////////////////////////////////////////////////////////////////////////
// trace.h
//

#include <iostream>

class CTrace
{
    const char* m_msg;
    bool TraceFlag;

  public
    CTrace(const char* msg)
      : m_msg(msg)
    {
      if (TraceFlag)
        std::cerr << "start: " << msg << std::endl;
    }
    ~CTrace()
    {
      if (TraceFlag)
        std::cerr << "end  : " << msg << std::endl;
    }

    static setTrace(bool flag)
    { TraceFlag = flag; }
};
