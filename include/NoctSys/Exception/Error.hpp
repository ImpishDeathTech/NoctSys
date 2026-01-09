/* Error.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once

#include <NoctSys/Configuration.hxx>
#include <exception>
#include <string>

namespace noct {
  class NoctSysAPI Error 
    : public std::exception
  {
    std::string m_message;

  public:
    Error(const std::string_view& message) 
      : m_message("NoctSys Error: ")
    {
      m_message += message.data();
    }

    Error(const std::string_view& type, const std::string_view& message) 
      : m_message("NoctSys Error: ")
    {
      m_message = m_message + type.data() + ": " + message.data();
    }

    const char* what() noexcept {
      return m_message.c_str();
    }
  };
}