cmake_minimum_required(VERSION 2.8.7 FATAL_ERROR)
project(libk2tree)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(libcds2 REQUIRED)
include(ExternalProject)
include(find_or_download_package)
find_or_download_package(GTest GTEST gtest)
#find_or_download_package(Boost Boost boost COMPONENTS system filesystem regex)
find_package(Boost COMPONENTS system filesystem regex)


include_directories($GTEST_INCLUDE_DIRS)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/dacs/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${Boost_INCLUDE_DIRS})

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

set(LIBK2TREE_NAME "k2tree")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # require at least gcc 4.8
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.7)
    message(FATAL_ERROR "GCC version must be at least 4.7!")
  endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # require at least clang 3.4
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.4)
    message(FATAL_ERROR "Clang version must be at least 3.4!")
  endif()
else()
  message(WARNING "You are using an unsupported compiler! Compilation has only been tested with Clang and GCC.")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wconversion -Wsign-conversion")

add_subdirectory(src)
add_subdirectory(dacs)
add_subdirectory(tests)

set(CMAKE_BUILD_TYPE Release)
