cmake_minimum_required(VERSION 3.20)

project(test)

file(GLOB PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file (GLOB html_src_files ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.c)
list(FILTER html_src_files EXCLUDE REGEX "main.c")
file(GLOB html_test_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c )

add_compile_options(-g -Wall)

add_executable(test ${html_test_sources} ${html_src_files})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(
  test
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)



set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules")
include(libraries)
init_libraries(test)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
# target_link_libraries(test PUBLIC html_static)

target_link_directories(test PUBLIC ${CMAKE_SOURCE_DIR}/../build)
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../src DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/html/)
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../include DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/html/)
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../CMakeLists.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/html/)
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../cmake DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/html/)

# add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/html)

# target_link_libraries(test PUBLIC html_static)

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

add_custom_target(test_assets ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMAND echo "yo"
    VERBATIM
)
