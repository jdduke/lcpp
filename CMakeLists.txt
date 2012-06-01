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

if(UNIX OR MINGW)
  set(STOP_ON_FIRST_ERROR TRUE CACHE BOOL "Stop compilation on first error")
  if(TREAT_WARNINGS_AS_ERRORS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
  endif()
  if (STOP_ON_FIRST_ERROR)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wfatal-errors")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -std=c++11 -Wall -Wno-missing-braces")
  if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -U__STRICT_ANSI__")
  endif()
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wno-deprecated")
    add_definitions( "-DDEBUG" )
  elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -fno-omit-frame-pointer")
  elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithProfile")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -g -O3-fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg -fPIC")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -pg")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wno-unused -Wno-deprecated -fno-omit-frame-pointer")
  endif()
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
