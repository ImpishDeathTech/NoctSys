/* NativeScript.cpp
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#include <NoctSys/Resource/NativeScript.hpp>
#include <NoctSys/Exception/NativeScriptError.hpp>

#include <algorithm>

namespace noct {
  NativeScript::NativeScript() 
    : m_handle(nullptr), m_error(), m_path()
  {}

  NativeScript::NativeScript(const std::filesystem::path& path) 
    : m_handle(nullptr), m_error(), m_path(path)
  {
    if (!open(path))
#if defined(__NoctSys_UNIX__)
      throw NativeScriptError(getError());

#elif defined(__NoctSys_Windows__)
      throw NativeScriptError(getFilePath(), getError());
#endif
  }

  NativeScript::~NativeScript() {
    close();
  }


  bool NativeScript::open(const std::filesystem::path& path) {
    bool ret = false;

#if defined(__NoctSys_UNIX__)
    if (path.extension().string() == "noct") {
      if (std::filesystem::exists(path)) {
        m_handle = dlopen((CStr)path.c_str(), RTLD_NOW);
        m_error  = dlerror();

        if (m_handle)
          ret = true;
      }
    } else m_error = "invalid file type: '" + path.extension().string() "'";

#elif defined(__NoctSys_Windows__)
    if (path.extension().string() == "noct") {
      if (std::filesystem::exists(path)) {
        m_handle = LoadLibraryA((CStr)path.c_str());
        m_error  = GetLastError();

        if (m_error == ERROR_SUCCESS);
          ret = true;
          
      } else m_error = ERROR_FILE_NOT_FOUND;
    } else m_error = ERROR_MOD_NOT_FOUND;
#endif

    return ret;
  }


  void NativeScript::close() {
    if (m_handle) {
#if defined(__NoctSys_Unix__)
      dlclose(m_handle);

#elif defined(__NoctSys_Windows__)
      FreeLibrary(m_handle);
#endif
    }
  }


  bool NativeScript::isOpen() {
    return m_handle ? true : false;
  }

  
  const CStr NativeScript::getError() {
#if defined(__NoctSys_UNIX__)
    return (CStr)m_error.c_str();

#elif defined(__NoctSys_Windows__)
    std::stringstream error;

    switch (m_error) {
      case ERROR_SUCCESS: 
        error << "code " << m_error << ": OK: last operation completed successfully"; 
        break;
      
      case ERROR_FILE_NOT_FOUND:
        error << "code " << m_error << ": File Not Found: the system connot find the file specified";
        break;

      case ERROR_PATH_NOT_FOUND:
        error << "code " << m_error << ": Path Not Found: the system cannot find the path specified";
        break;

      case ERROR_TOO_MANY_OPEN_FILES:
        error << "code " << m_error << ": File Overflow: the system cannot open the file";
        break;

      case ERROR_ACCESS_DENIED:
        error << "code " << m_error << ": access is denied: ";
        break;

      case ERROR_INVALID_HANDLE:
        error << "code " << m_error << ": handle is invalid";
        break;

      case ERROR_NOT_ENOUGH_MEMORY:
        error << "code " << m_error << ": Out of Memory";
        break;

      case ERROR_INVALID_PARAMETER:
        error << "code " << m_error << ": Invalid Parameter: ";
        break; 

      case ERROR_PROC_NOT_FOUND:
        error << "code " << m_error << ": export name or ordinal does not exist in module: '" << m_lastSymbol;
        break;

      case ERROR_MOD_NOT_FOUND:
        error << "code " << m_error << ": the specified module could not be found";
        break;

      case ERROR_BAD_EXE_FORMAT:
        error << "code " << m_error << ": not a valid Win32 application";
        break;

      case ERROR_DLL_INIT_FAILED:
        error << "code " << m_error << ": DLL Init Failed";
        break;

      default: 
        error << "code " << m_error;

    }

    return (CStr)error.str().c_str();
#endif
  }


  const std::filesystem::path& NativeScript::getFilePath() {
    return m_path;
  }


  LoaderHandle NativeScript::find(const std::string_view& symbol) {
    std::string sym = symbol.data();
    std::replace(sym.begin(), sym.end(), ' ', '_');

#if defined(__NoctSys_UNIX__)
    LoaderHandle proc = dlsym(m_handle, sym.c_str());
    m_error = dlerror();

#elif defined(__NoctSys_Windows__)
    LoaderHandle proc = GetProcAddress(m_handle, (CStr)sym.c_str());
    m_error           = GetLastError();
    m_lastSymbol      = symbol.data();

    if (m_error != ERROR_SUCCESS)
      return nullptr;
#endif
    return proc;
  }
}