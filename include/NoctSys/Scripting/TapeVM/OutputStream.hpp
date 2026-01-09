/* OutputStream.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once 

// Noctsys Headers
#include <NoctSys/Configuration.hxx>
#include <NoctSys/Scripting/TapeVM/OutputStream/OutputSource.hpp>
#include <NoctSys/Scripting/TapeVM/OutputStream/StdoutSource.hpp>
#include <NoctSys/Scripting/TapeVM/OutputStream/FileOutputSource.hpp>
#include <NoctSys/Scripting/TapeVM/OutputStream/StringOutputSource.hpp>
#include <NoctSys/Scripting/TapeVM/OutputStream/StderrSource.hpp>

// Standard Library Headers
#include <memory>
#include <string>

namespace noct {
  class NoctSysAPI OutputStream 
  {
    StdoutSource                         m_stdout;
    OutputSource<char>*                  m_source;

  public:
    explicit OutputStream(OutputSource<char>& src) 
      : m_source(&src)
    {}

    OutputStream() 
      : m_source(nullptr)
    {}

    void reset();
    void reset(OutputSource<char>& src);
    void write(std::string_view s);
    void put(char ch);
    void newline();
    void flush();
    bool isString() const;

    StringOutputSource* getCurrentStringSource();

    OutputStream& operator<<(std::string_view s) {
      write(s);
      return *this;
    }

    OutputStream& operator<<(char ch) {
      put(ch);
      return *this;
    }

    OutputStream& operator<<(int i) {
      write(std::to_string(i));
      return *this;
    }

    OutputStream& operator<<(unsigned i) {
      write(std::to_string(i));
      return *this;
    }

    OutputStream& operator<<(std::size_t i) {
      write(std::to_string(i));
      return *this;
    }

    OutputStream& operator<<(float f) {
      write(std::to_string(f));
      return *this;
    }
  };
}
