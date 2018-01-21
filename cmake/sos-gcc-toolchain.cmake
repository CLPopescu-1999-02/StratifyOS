message(STATUS "GCC Toolchain:" ${TOOLCHAIN_DIR}/${TOOLCHAIN_HOST})


set(TOOLCHAIN_BIN_DIR "${TOOLCHAIN_DIR}/bin" CACHE INTERNAL "toolchain bin dir")
set(TOOLCHAIN_INC_DIR "${TOOLCHAIN_DIR}/${TOOLCHAIN_HOST}/include" CACHE INTERNAL "toolchain inc dir")
set(TOOLCHAIN_LIB_DIR "${TOOLCHAIN_DIR}/${TOOLCHAIN_HOST}/lib" CACHE INTERNAL "toolchain lib dir")


set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_C_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN_HOST}-gcc${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "c compiler")
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN_HOST}-g++${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "cxx compiler")
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN_HOST}-as${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "asm compiler")
set(CMAKE_OBJCOPY ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN_HOST}-objcopy${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "object copy tool")
set(CMAKE_OBJDUMP ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN_HOST}-objdump${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "object dump tool")
set(CMAKE_SIZE ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN_HOST}-size${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "size tool")
set(CMAKE_INSTALL_PREFIX ${TOOLCHAIN_DIR}/${TOOLCHAIN_HOST} CACHE INTERNAL "install prefix")
