/* TapeVM/OutputStream.cpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#include <NoctSys/Scripting/TapeVM/OutputStream.hpp>

namespace noct {
  void OutputStream::reset() {
    m_source = nullptr;
  }


  void OutputStream::reset(OutputSource<char>& src) {
    m_source = &src;
  }


  void OutputStream::write(std::string_view s){
    if (m_source)
      m_source->write(s.data(), s.size());
    
    else m_stdout.write(s.data(), s.size());
  }


  void OutputStream::put(char ch) {
    if (m_source)
      m_source->put(ch);

    else m_stdout.put(ch);
  }


  void OutputStream::newline()  {
    if (m_source)
      m_source->put('\n');
    
    else m_stdout.put('\n');
  }


  void OutputStream::flush() {
    if (m_source)
      m_source->flush();
    
    else m_stdout.flush();
  }


  bool OutputStream::isString() const {
    return reinterpret_cast<StringOutputSource*>(m_source) != nullptr;
  }

  
  StringOutputSource* OutputStream::getCurrentStringSource() {
    return reinterpret_cast<StringOutputSource*>(m_source);
  }

}