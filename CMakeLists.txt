cmake_minimum_required(VERSION 3.20)


project(html)

file(GLOB PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB html_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

add_compile_options(-g -Wall)

add_executable(html_e ${html_sources})


list(FILTER html_sources EXCLUDE REGEX "main.c")

add_library(html SHARED ${html_sources})
add_library(html_static STATIC ${html_sources})

target_compile_options(html PUBLIC -fPIC)
target_compile_options(html_static PUBLIC -fPIC)

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
  html_static
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


include(libraries)

init_libraries(html_e)
init_libraries(html_static)
init_libraries(html)


add_custom_command(
    OUTPUT always_rebuild
    COMMAND cmake -E echo
    )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Copying assets"
    DEPENDS always_rebuild
)

add_custom_target(project_assets ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMAND echo "yo"
    VERBATIM
)
