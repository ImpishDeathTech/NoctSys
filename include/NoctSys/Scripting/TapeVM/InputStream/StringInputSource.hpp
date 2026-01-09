/* StringInputSource.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once 
#include <NoctSys/Configuration.hxx>
#include <NoctSys/Scripting/TapeVM/InputStream/InputSource.hpp>
#include <sstream>

namespace noct {
  class NoctSysAPI StringInputSource
    : public InputSource
  {
    std::istringstream m_stream;

  public:
    explicit StringInputSource(std::string line)
      : m_stream(std::move(line))
    {}

    bool eof() const override;
    int  get()       override;
    void unget()     override;
  };
}