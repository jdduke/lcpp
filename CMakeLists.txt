cmake_minimum_required (VERSION 2.6)
project(lcpp)

set(CMAKE_DEBUG_POSTFIX "d")

# Treat warnings as errors cmake option
set(TREAT_WARNINGS_AS_ERRORS TRUE  CACHE BOOL "Treat warnings as errors")
set(BUILD_SAMPLES            TRUE  CACHE BOOL "Build samples")
set(BUILD_TESTS              FALSE CACHE BOOL "Build tests")

if(MSVC)
  set(USE_INTEL              FALSE CACHE BOOL "Use Intel C++ compiler")
  if (NOT USE_INTEL)
    MESSAGE(FATAL_ERROR "ERROR: MSVC does not support variadic templates.  Try the latest release of the most excellent MinGW.")
  endif()
endif()

if(CMAKE_SIZEOF_VOID_P MATCHES 4)
  set(LCPP_SIZE_TYPE x86)
else()
  set(LCPP_SIZE_TYPE x64)
endif()

set(LCPP_BINARY_PATH ${CMAKE_HOME_DIRECTORY}/bin/${LCPP_SIZE_TYPE}${LCPP_SUFFIX})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${LCPP_BINARY_PATH}
    CACHE PATH
    "Single Directory for all Executables.")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${LCPP_BINARY_PATH}
    CACHE PATH
    "Single Directory for all Libraries")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${LCPP_LIBRARY_PATH}
    CACHE PATH
    "Single Directory for all static libraries.")


if (CMAKE_CXX_COMPILER MATCHES ".*clang")
  set(CMAKE_COMPILER_IS_CLANGXX 1)
endif ()

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
  set(BUILD_64_BIT FALSE CACHE BOOL "Enable 64-bit build")
  if (BUILD_64_BIT)
    set(COMMON_FLAGS "-m64")
    set(${NAME}_SIZE_TYPE x64)
  else()
    set(COMMON_FLAGS "-m32")
    set(${NAME}_SIZE_TYPE x86)
  endif()

  if(TREAT_WARNINGS_AS_ERRORS)
    set(COMMON_FLAGS "${COMMON_FLAGS} -Werror")
  endif()

  set(STOP_ON_FIRST_ERROR TRUE CACHE BOOL "Stop compilation on first error")
  if (STOP_ON_FIRST_ERROR)
    set(COMMON_FLAGS "${COMMON_FLAGS} -Wfatal-errors")
  endif()

  if (MINGW)
    set(COMMON_FLAGS "${COMMON_FLAGS} -static-libstdc++ -static-libgcc -static")
  endif()

  if (CMAKE_COMPILER_IS_CLANGXX)
    set(COMMON_FLAGS "${COMMON_FLAGS} -stdlib=libc++")
    add_definitions("-std=c++0x")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  endif()

  set(CMAKE_C_FLAGS             "${CMAKE_C_FLAGS}             ${COMMON_FLAGS}")
  set(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS}           ${COMMON_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    ${COMMON_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${COMMON_FLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${COMMON_FLAGS}")
endif()

include_directories(include)

if(USE_INTEL)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Qstd=c++0x")
  MACRO(lcpp_add_executable target ARGS)
    add_executable(${target} ${ARGS})
    set_target_properties(${target} PROPERTIES PLATFORM_TOOLSET "Intel C++ Compiler XE 12.1")
  ENDMACRO()
else()
  MACRO(lcpp_add_executable target ARGS)
    add_executable(${target} ${ARGS})
  ENDMACRO()
endif()

if(BUILD_SAMPLES)
  add_subdirectory(samples)
endif()

if(BUILD_TESTS)
  include_directories(${LCPP_SOURCE_DIR}/tests)
  add_subdirectory(externals/googletest)
  add_subdirectory(tests)
endif()
