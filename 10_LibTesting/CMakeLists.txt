cmake_minimum_required(VERSION 3.10)
project(GrowableBuffers C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

add_compile_options(-Wall -Wextra)

if(ENABLE_COVERAGE)
  add_compile_options(-fprofile-arcs -ftest-coverage)
  add_link_options(-fprofile-arcs -ftest-coverage)
endif()

add_library(buf buf.c)
target_include_directories(buf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(buf_test tests.c)
target_link_libraries(buf_test buf)
set_target_properties(buf_test PROPERTIES OUTPUT_NAME tests)

enable_testing()
add_test(NAME buf_test COMMAND tests)

if(ENABLE_COVERAGE)
  add_custom_target(coverage
    COMMAND lcov --directory . --capture --output-file coverage.info
    COMMAND lcov --remove coverage.info '/usr/*' --output-file coverage.info
    COMMAND lcov --list coverage.info
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating coverage report"
  )
endif()
