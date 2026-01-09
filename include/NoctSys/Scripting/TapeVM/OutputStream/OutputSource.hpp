#pragma once 

#include <NoctSys/Configuration.hxx>
#include <cstdio>

namespace noct {
  template<typename CharT>
  class NoctSysAPI OutputSource 
  {
  public:
    virtual ~OutputSource()                                = default;
    
    virtual void write(const CharT* ch, std::size_t size) = 0;
    virtual void put(CharT ch)                            = 0;

    virtual void flush() {}
  };


}