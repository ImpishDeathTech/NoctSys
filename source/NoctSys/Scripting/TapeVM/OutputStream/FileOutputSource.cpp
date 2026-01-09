/* TapeVM/OutputStream/FileOutputSource.cpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#include <NoctSys/Scripting/TapeVM/OutputStream/FileOutputSource.hpp>

namespace noct {

  FileOutputSource::~FileOutputSource() {
    if (m_fd) 
      std::fclose(m_fd);
  }

  void FileOutputSource::write(const char* data, std::size_t size) {
    std::fwrite(data, 1, size, m_fd);
  }

  void FileOutputSource::put(char ch) {
    std::fputc(ch, m_fd);
  }

  void FileOutputSource::flush() {
    std::fflush(m_fd);
  }

}