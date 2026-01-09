/* InputSource.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once 

#include <NoctSys/Configuration.hxx>

namespace noct {
  class NoctSysAPI InputSource 
  {
  public:
    virtual ~InputSource()   = default;
    virtual bool eof() const = 0;
    virtual int  get()       = 0;
    virtual void unget()     = 0;
  };
}