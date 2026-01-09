/* StderrSource.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once 

#include <NoctSys/Configuration.hxx>
#include <NoctSys/Scripting/TapeVM/OutputStream/OutputSource.hpp>
#include <cstdio>

namespace noct {
  class NoctSysAPI StderrSource 
    : public OutputSource<char>
  {
  public:
    void write(const char* data, std::size_t size) override;
    void put(char ch)                              override;
    void flush()                                   override;
  };
}