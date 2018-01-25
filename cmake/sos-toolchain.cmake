
if( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows" )
    set(CMAKE_MAKE_PROGRAM "C:/StratifyLabs-SDK/Tools/gcc/bin/mingw32-make.exe")
    set(CMAKE_SET_GENERATOR "MinGW Makefiles" CACHE INTERNAL "cmake generator mingw makefiles")
endif()

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

if(TOOLCHAIN_DIR)
  message(STATUS "User provided toolchain directory: " ${TOOLCHAIN_DIR})
else()
  if( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin" )
    set(TOOLCHAIN_DIR "/Applications/StratifyLabs-SDK/Tools/gcc")
    message(STATUS "Darwin provided toolchain directory" ${TOOLCHAIN_DIR})
    set(TOOLCHAIN_EXEC_SUFFIX "")
  endif()
  if( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows" )
    set(TOOLCHAIN_DIR "C:/StratifyLabs-SDK/Tools/gcc")
    message(STATUS "Windows provided toolchain directory" ${TOOLCHAIN_DIR})
    set(TOOLCHAIN_EXEC_SUFFIX ".exe")
  endif()
endif()

if(TOOLCHAIN_HOST)               # <--- Use 'BOOST_DIR', not 'DEFINED ${BOOST_DIR}'
  MESSAGE(STATUS "User provided toolchain directory" ${TOOLCHAIN_HOST})
else()
  set(TOOLCHAIN_HOST "arm-none-eabi")
  MESSAGE(STATUS "Set toolchain host to: " ${TOOLCHAIN_HOST})
endif()

include(${CMAKE_CURRENT_LIST_DIR}/sos-gcc-toolchain.cmake)

set(TOOLCHAIN_C_FLAGS "-Wall -mthumb -Os -D__StratifyOS__ -ffunction-sections -fdata-sections -fomit-frame-pointer" CACHE INTERNAL "common c compiler flags")
set(TOOLCHAIN_CXX_FLAGS "-fno-rtti -fno-exceptions -fno-threadsafe-statics -fno-unwind-tables -fno-use-cxa-atexit" CACHE INTERNAL "common c compiler flags")
set(TOOLCHAIN_ASM_FLAGS "-mthumb" CACHE INTERNAL "common asm compiler flags")
set(TOOLCHAIN_FLOAT_OPTIONS_ARMV7EM_FPU -mfloat-abi=hard -mfpu=fpv4-sp-d16 -U__SOFTFP__ -D__FPU_USED CACHE INTERNAL "armv7em_fpu build float options")
set(TOOLCHAIN_FLOAT_DIR_ARMV7M "." CACHE INTERNAL "armv7m build float dir")
set(TOOLCHAIN_FLOAT_DIR_ARMV7EM_FPU "fpu/fpv5-sp-d16" CACHE INTERNAL "armv7m build float dir")

set(CMAKE_VERBOSE_MAKEFILE TRUE CACHE INTERNAL "verbose make")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
