/* InputStream.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once

#include <NoctSys/Configuration.hxx>
#include <NoctSys/Scripting/TapeVM/InputStream/InputSource.hpp>
#include <NoctSys/Scripting/TapeVM/InputStream/StringInputSource.hpp>
#include <NoctSys/Scripting/TapeVM/InputStream/FileInputSource.hpp>

#include <vector>
#include <memory>

namespace noct {
  class NoctSysAPI InputStream 
  {
    std::vector<std::unique_ptr<InputSource>> m_stack;

  public:
    
    void push(InputSource* ptr);
    void pop();
    bool eof() const;
    int  get();
    void unget();
  };
}