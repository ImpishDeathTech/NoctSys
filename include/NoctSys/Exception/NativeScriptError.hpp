/* NativeScriptError.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once

#include <NoctSys/Configuration.hxx>
#include <NoctSys/Exception/ResourceError.hpp>

namespace noct {
  class NoctSysAPI NativeScriptError
    : public ResourceError 
  {
  public:
    NativeScriptError(const std::string_view& message) 
      : ResourceError(std::string("NativeScript Error: ") + message.data())
    {}

    NativeScriptError(const std::filesystem::path& path, const std::string_view& message) 
      : ResourceError("NativeScript Error: '" + path.string() + "': " + message.data())
    {}
  };
}