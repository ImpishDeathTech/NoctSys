/* StackUnderflowError.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once
#include <NoctSys/Configuration.hxx>
#include <NoctSys/Exception/Error.hpp>

namespace noct {
  class NoctSysAPI StackUnderflowError
    : public Error
  {
  public:
    StackUnderflowError()
      : Error("Stack Underflow", "the state stack is empty")
    {}


    StackUnderflowError(const std::string_view& message)
      : Error("Stack Underflow", message)
    {}
  };

}