cmake_minimum_required(VERSION 3.20)


project(html)

file(GLOB PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB html_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)


add_compile_options(-g -Wall)

add_executable(html_e ${html_sources})


list(FILTER html_ources EXCLUDE REGEX "main.c")

add_library(html ${html_sources})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(
  html
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_include_directories(
  html_e
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)


include(FetchContent)

FetchContent_Declare(
  easystr
  URL /home/ianertson/workspace/easystr
  #  GIT_REPOSITORY https://github.com/sebbekarlsson/easystr.git
)

FetchContent_MakeAvailable(easystr)


target_link_libraries(html PRIVATE easystr)
target_link_libraries(html_e PRIVATE easystr)
