/* StringOutputSource.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once 

#include <NoctSys/Configuration.hxx>
#include <NoctSys/Scripting/TapeVM/OutputStream/OutputSource.hpp>
#include <cstdio>
#include <string>

namespace noct {
  class NoctSysAPI StringOutputSource
    : public OutputSource<char>
  {
    std::string m_buffer;

  public:
    explicit StringOutputSource(std::size_t reserve = 0ul) {
      if (reserve)
        m_buffer.reserve(reserve);
    } 

    void write(const char* data, std::size_t size) override;
    void put(char ch)                              override;
    void clear();

    std::string      str()  const;
    std::string_view view() const;
    const char*      cstr() const;

  };
}