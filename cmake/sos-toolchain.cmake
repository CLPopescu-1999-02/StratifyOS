
if( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows" )
		set(CMAKE_MAKE_PROGRAM "C:/StratifyLabs-SDK/Tools/gcc/bin/make.exe")
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
	set(TOOLCHAIN_HOST "arm-none-eabi" CACHE INTERNAL "TOOLCHAIN HOST ARM-NONE-EABI")
  MESSAGE(STATUS "Set toolchain host to: " ${TOOLCHAIN_HOST})
endif()

include(${CMAKE_CURRENT_LIST_DIR}/sos-gcc-toolchain.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/sos-build-flags.cmake)

set(TOOLCHAIN_C_FLAGS ${SOS_BUILD_C_FLAGS})
set(TOOLCHAIN_CXX_FLAGS ${SOS_BUILD_CXX_FLAGS})
set(TOOLCHAIN_ASM_FLAGS ${SOS_BUILD_ASM_FLAGS})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
