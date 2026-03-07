# https://llvm.org/docs/CMake.html#embedding-llvm-in-your-project

set(LIBRARY_NAME "Oyl.Spyll")

cmake_minimum_required(VERSION 3.20.0)
project(${LIBRARY_NAME})

set(LLVM_LINK_LLVM_DYLIB ON)
set(CLANG_LINK_CLANG_DYLIB ON)

# https://stackoverflow.com/questions/53652077/how-to-build-and-debug-a-msys2-mingw64-cmake-project-in-visual-studio-2017
include(GNUInstallDirs)

set(LLVM_DIR ../../Packages/Llvm)

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

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

file(GLOB_RECURSE Spyll_SRC CONFIGURE_DEPENDS "Spyll/*.h" "Spyll/*.cpp")

add_llvm_library(${LIBRARY_NAME} SHARED ${Spyll_SRC})

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
  clang_target_link_libraries(${LIBRARY_NAME} PRIVATE)
endif()