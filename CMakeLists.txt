cmake_minimum_required(VERSION 3.20)


project(html)

file(GLOB PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB html_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)


# add_compile_options(-g -Wall)

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


include(FetchContent)

FetchContent_Declare(
  easystr
#  URL /home/ianertson/workspace/easystr
  GIT_REPOSITORY ssh://git@github.com:/sebbekarlsson/easystr.git
)

# add_subdirectory(${easystr_SOURCE_DIR})
FetchContent_MakeAvailable(easystr)


target_link_libraries(html PUBLIC easystr)
target_link_libraries(html_e PUBLIC easystr)
target_link_libraries(html_static PUBLIC easystr)



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
