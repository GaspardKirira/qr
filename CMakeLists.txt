cmake_minimum_required(VERSION 3.20)
project(qr LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(qr INTERFACE)
add_library(qr::qr ALIAS qr)

target_include_directories(qr INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

if (MSVC)
  target_compile_options(qr INTERFACE /W4 /permissive-)
else()
  target_compile_options(qr INTERFACE -Wall -Wextra -Wpedantic)
endif()

include(CTest)
enable_testing()

add_executable(qr_basic_test tests/test_basic.cpp)
target_link_libraries(qr_basic_test PRIVATE qr::qr)
add_test(NAME qr.basic COMMAND qr_basic_test)
