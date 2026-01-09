/* TapeVM/OutputStream/StringOutputSource.cpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#include <NoctSys/Scripting/TapeVM/OutputStream/StringOutputSource.hpp>

namespace noct {
  void StringOutputSource::write(const char* data, std::size_t size) {
    m_buffer.append(data, size);
  }

  void StringOutputSource::put(char ch) {
    m_buffer += ch;
  }

  void StringOutputSource::clear() {
    m_buffer.clear();
  }

  std::string StringOutputSource::str() const {
    return m_buffer;
  } 

  std::string_view StringOutputSource::view() const {
    return m_buffer;
  }

  const char* StringOutputSource::cstr() const {
    return m_buffer.c_str();
  }

}