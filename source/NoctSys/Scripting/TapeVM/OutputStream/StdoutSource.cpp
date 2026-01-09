/* TapeVM/OutputStream/StdoutSource.cpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#include <NoctSys/Scripting/TapeVM/OutputStream/StdoutSource.hpp>

namespace noct {
  void StdoutSource::write(const char* data, std::size_t size) {
    std::fwrite(data, 1, size, stdout);
  }


  void StdoutSource::put(char ch) {
    std::fputc(ch, stdout);
  }

  
  void StdoutSource::flush() {
    std::fflush(stdout);
  }
}