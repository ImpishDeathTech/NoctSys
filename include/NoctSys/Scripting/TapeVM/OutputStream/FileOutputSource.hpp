/* FileOutputSource.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once 

#include <NoctSys/Configuration.hxx>
#include <NoctSys/Scripting/TapeVM/OutputStream/OutputSource.hpp>
#include <cstdio>

namespace noct {
  class NoctSysAPI FileOutputSource 
    : public OutputSource<char>
  {
    std::FILE* m_fd;
    
  public:

    explicit FileOutputSource(std::FILE* fd)
      : m_fd(fd)
    {}

    ~FileOutputSource() {
      if (m_fd)
        std::fclose(m_fd);
    }
    
    void write(const char* data, std::size_t size) override;
    void put(char ch)                              override;
    void flush()                                   override;
  };
}