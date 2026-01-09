/* ResourceError.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once

#include <NoctSys/Configuration.hxx>
#include <NoctSys/Exception/Error.hpp>

#include <filesystem>

namespace noct {
  class ResourceError
    : public Error  
  {
  public:
    ResourceError(const std::string_view& message) 
      : Error("Resource Error", message.data())
    {}

    ResourceError(const std::filesystem::path& path, const std::string_view& message)
      : Error("Resource Error", "'" + path.string() + "': " + message.data())
    {}
  };
}