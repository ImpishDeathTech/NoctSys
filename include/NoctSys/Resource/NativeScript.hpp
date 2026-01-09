/* NativeScript.hpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once

#include <NoctSys/Configuration.hxx>
#include <filesystem>

#define NATIVESCRIPT_VERSION_MAJOR 1
#define NATIVESCRIPT_VERSION_MINOR 0 
#define NATIVESCRIPT_VERSION_PATCH 1

namespace noct {
#if defined(__NoctSys_UNIX__)
  #include <dlfcn.h>

  typedef void* ModuleHandle;
  typedef void* LoaderHandle;
  typedef char* CStr;

#elif defined(__NoctSys_Windows__)
  #include <libloaderapi.h>
  #include <errhandlingapi.h>
  #include <winerror.h>
  #include <sstream>

  typedef HMODULE ModuleHandle;
  typedef FARPROC LoaderHandle;
  typedef LPCSTR  CStr;

#endif

  class NoctSysAPI NativeScript 
  {
    const std::filesystem::path m_path;
    ModuleHandle                m_handle;

#if defined(__NoctSys_UNIX__)
    std::string                 m_error;

#elif defined(__NoctSys_Windows__)
    DWORD                       m_error;
    std::string                 m_lastSymbol;

#endif

  public:
    NativeScript();
    NativeScript(const std::filesystem::path& path);
    ~NativeScript();

    bool                         open(const std::filesystem::path& path);
    void                         close();
    bool                         isOpen();
    const CStr                   getError();
    const std::filesystem::path& getFilePath();
    LoaderHandle                 find(const std::string_view& symbol);

    template<typename LoaderT>
    LoaderT findAs(const std::string_view& symbol) {
      LoaderHandle loader = find(symbol);

      if (m_error == 0)
        return reinterpret_cast<LoaderT>(loader);
      
      return {};
    }
  };
}