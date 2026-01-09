/* Configuration.hxx
 * Copyright (c) 2020-2025, Christopher Stephen Rafuse
 * BSD-2-Clause
 */
#pragma once

#define NOCTSYS_VERSION_MAJOR 2
#define NOCTSYS_VERSION_MINOR 0
#define NOCTSYS_VERSION_PATCH 0

#if defined(_WIN32)
  #define __NoctSys_Windows__

#elif defined(__unix__)
  #define __NoctSys_UNIX__

  #if defined(__linux__)
    #define __NoctSys_Linux__
    
  #elif defined(__FreeBSD__) || defined(__FreeBSD_Kernel__)
    #define __NoctSys_FreeBSD__

  #else 
    #error NoctSys does not support this UNIX system
  #endif

#else 
  #error NoctSys does not support this operating system
#endif



#if defined(__NoctSys_Windows__)
  #define __NoctSys_Export__ __declspec(dllexport)
  #define __NoctSys_Import__ __declspec(dllimport)

#else
  #define __NoctSys_Export__ __attribute__((__visibility__("default")))
  #define __NoctSys_Import__ __attribute__((__visibility__("default")))
#endif


#if defined(NOCT_EXPORTS)
  #define NoctSysAPI __NoctSys_Export__ 
#else 
  #define NoctSysAPI __NoctSys_Import__ 
#endif