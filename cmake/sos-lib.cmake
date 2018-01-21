
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-build-flags.cmake)
set(CMAKE_VERBOSE_MAKEFILE TRUE CACHE INTERNAL "verbose make")

if( ${SOS_LIB_ARCH} STREQUAL armv7-m )
	set(BUILD_ARM_ARCH "armv7m" CACHE INTERNAL "build device")
	set(INSTALL_ARM_ARCH ${SOS_LIB_ARCH} CACHE INTERNAL "build device")
	set(BUILD_FLOAT ${SOS_BUILD_FLOAT_DIR_ARMV7M} CACHE INTERNAL "build float")
	set(BUILD_FLOAT_OPTIONS "" CACHE INTERNAL "build float options")
	set(BUILD_LIB_ARCH -march=${SOS_LIB_ARCH})
elseif( ${SOS_LIB_ARCH} STREQUAL armv7e-m )
	set(BUILD_FLOAT "${SOS_BUILD_FLOAT_DIR_ARMV7EM_FPU}" CACHE INTERNAL "build float dir")
	set(BUILD_ARM_ARCH "armv7em" CACHE INTERNAL "build device")
	set(INSTALL_ARM_ARCH ${SOS_LIB_ARCH} CACHE INTERNAL "build device")
	set(BUILD_FLOAT_OPTIONS ${SOS_BUILD_FLOAT_OPTIONS_ARMV7EM_FPU} CACHE INTERNAL "build float options")
	set(BUILD_LIB_ARCH -march=${SOS_LIB_ARCH})
elseif( ${SOS_LIB_ARCH} STREQUAL link )
	set(BUILD_ARM_ARCH "link" CACHE INTERNAL "build device")
	set(INSTALL_ARM_ARCH "." CACHE INTERNAL "build device")
	set(BUILD_FLOAT_OPTIONS "" CACHE INTERNAL "build float options")
	set(BUILD_FLOAT "." CACHE INTERNAL "build float")
	set(BUILD_LIB_ARCH "")
endif()

if( ${SOS_LIB_CONFIG} STREQUAL release )
	set(SOS_LIB_SUFFIX "")
else()
	set(SOS_LIB_SUFFIX _${SOS_LIB_CONFIG})
endif()

set(BUILD_FLAGS ${SOS_LIB_BUILD_FLAGS} -D__${BUILD_ARM_ARCH} ${BUILD_LIB_ARCH} -D__${SOS_LIB_CONFIG} ${BUILD_FLOAT_OPTIONS} CACHE INTERNAL "sos app build options")

set(BUILD_LIBRARY_INSTALL_NAME ${SOS_LIB_NAME}${SOS_LIB_SUFFIX})
set(BUILD_LIBRARY_TARGET ${BUILD_LIBRARY_INSTALL_NAME}_${BUILD_ARM_ARCH})
include_directories(${SOS_LIB_INCLUDE_DIRECTORIES})
add_library(${BUILD_LIBRARY_TARGET} STATIC ${SOS_LIB_SOURCELIST})

target_compile_options(${BUILD_LIBRARY_TARGET} PUBLIC ${BUILD_FLAGS})
install(FILES ${PROJECT_BINARY_DIR}/lib${BUILD_LIBRARY_TARGET}.a DESTINATION lib/${INSTALL_ARM_ARCH}/${BUILD_FLOAT} RENAME lib${BUILD_LIBRARY_INSTALL_NAME}.a)
