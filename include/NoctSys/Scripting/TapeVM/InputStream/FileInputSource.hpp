/* FileInputSource.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once 

#include <NoctSys/Configuration.hxx>
#include <NoctSys/Scripting/TapeVM/InputStream/InputSource.hpp>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>

namespace noct {
  class FileInputSource 
    : public InputSource
  {
    std::ifstream m_stream;

  public:
    explicit FileInputSource(const std::filesystem::path& path);
    FileInputSource(std::ifstream& stream);

    bool good() const;
    bool eof()  const override;
    int  get()        override;
    void unget()      override;
  };
}