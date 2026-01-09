/* TapeVM/InputStream/FileInputSource.cpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#include <NoctSys/Scripting/TapeVM/InputStream/FileInputSource.hpp>

namespace noct {
  FileInputSource::FileInputSource(const std::filesystem::path& path)
    : m_stream(path)
  {}

  FileInputSource::FileInputSource(std::ifstream& stream)
    : m_stream(std::move(stream))
  {}


  bool FileInputSource::good() const {
    return m_stream.good();
  }

  bool FileInputSource::eof() const {
    return m_stream.eof();
  }

  int FileInputSource::get() {
    return m_stream.get();
  }

  void FileInputSource::unget() {
    m_stream.unget();
  }
}