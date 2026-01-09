/* TapeError.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once

#include <NoctSys/Configuration.hxx>
#include <NoctSys/Exception/Error.hpp>

namespace noct {
  class NoctSysAPI TapeError 
   : public Error 
  {
  public:
    TapeError(const std::string_view type, const std::string_view word) 
      : Error(type, ">>> " + std::string(word) + " <<<")
    {}
  };
}