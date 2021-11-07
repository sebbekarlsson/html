cmake_minimum_required(VERSION 3.20)

project(html_test)


file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


add_executable(test src/main.c)


target_link_libraries(test PRIVATE html_static)
