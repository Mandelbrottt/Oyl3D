# https://llvm.org/docs/CMake.html#embedding-llvm-in-your-project

cmake_minimum_required(VERSION 3.20.0)
project(Spyll)

set(LLVM_DIR "../../Packages/Llvm" CACHE FILEPATH "")

set(LLVM_SEARCH_PATHS
  ${LLVM_DIR}
  ${LLVM_DIR}/lib/cmake
  ${LLVM_DIR}/lib/cmake/llvm
  ${LLVM_DIR}/lib/cmake/clang
  ${LLVM_DIR}/share/clang/cmake
  ${LLVM_DIR}/share/llvm/cmake
)

find_package(LLVM
  REQUIRED
  CONFIG
  PATHS ${LLVM_SEARCH_PATHS}
  NO_DEFAULT_PATH
)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

add_llvm_library(PrintFunctionNames MODULE PrintFunctionNames.cpp PLUGIN_TOOL clang)

find_package(Clang
  REQUIRED
  CONFIG
  PATHS ${LLVM_SEARCH_PATHS}
  NO_DEFAULT_PATH
)

message(STATUS "Found Clang ${CLANG_PACKAGE_VERSION}")
message(STATUS "Using ClangConfig.cmake in: ${CLANG_DIR}")

list(APPEND CMAKE_MODULE_PATH "${CLANG_CMAKE_DIR}")
include(AddClang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(PrintFunctionNames PRIVATE
    clangAST
    clangBasic
    clangFrontend
  )
endif()