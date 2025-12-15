cmake_minimum_required(VERSION 3.10)
project(rhasher VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wformat-security -Wformat-nonliteral")

find_library(RHASH_LIBRARY NAMES rhash)
find_path(RHASH_INCLUDE_DIR rhash.h)

if(NOT RHASH_LIBRARY OR NOT RHASH_INCLUDE_DIR)
    message(FATAL_ERROR "librhash library not found. Please install development package for librhash.")
endif()

option(USE_READLINE "Build with readline support" ON)

if(USE_READLINE)
    find_library(READLINE_LIBRARY NAMES readline)
    find_path(READLINE_INCLUDE_DIR readline/readline.h)
    
    if(READLINE_LIBRARY AND READLINE_INCLUDE_DIR)
        set(READLINE_FOUND TRUE)
        add_definitions(-DUSE_READLINE)
        message(STATUS "Readline support enabled")
    else()
        set(READLINE_FOUND FALSE)
        message(STATUS "Readline library not found, building without readline support")
    endif()
else()
    set(READLINE_FOUND FALSE)
    message(STATUS "Readline support disabled by user")
endif()

add_executable(rhasher src/rhasher.c)

target_include_directories(rhasher PRIVATE ${RHASH_INCLUDE_DIR})
target_link_libraries(rhasher ${RHASH_LIBRARY})

if(READLINE_FOUND)
    target_include_directories(rhasher PRIVATE ${READLINE_INCLUDE_DIR})
    target_link_libraries(rhasher ${READLINE_LIBRARY})
endif()

install(TARGETS rhasher DESTINATION bin)

enable_testing()

file(WRITE ${CMAKE_BINARY_DIR}/test1.txt "Hello, World!")
file(WRITE ${CMAKE_BINARY_DIR}/test2.txt "Test file for hashing")

add_test(NAME test_md5_string
    COMMAND ${CMAKE_BINARY_DIR}/rhasher MD5 Hello, World!
)

add_test(NAME test_sha1_string
    COMMAND ${CMAKE_BINARY_DIR}/rhasher SHA1 Hello, World!
)

add_test(NAME test_md5_file
    COMMAND ${CMAKE_BINARY_DIR}/rhasher MD5 ${CMAKE_BINARY_DIR}/test1.txt
)

add_test(NAME test_sha1_file
    COMMAND ${CMAKE_BINARY_DIR}/rhasher SHA1 ${CMAKE_BINARY_DIR}/test1.txt
)

find_program(SHA1SUM_TOOL sha1sum)
if(SHA1SUM_TOOL)
    add_test(NAME test_sha1sum_comparison
        COMMAND bash -c "${CMAKE_BINARY_DIR}/rhasher SHA1 ${CMAKE_BINARY_DIR}/test1.txt | cut -d' ' -f1 > hash1.txt && ${SHA1SUM_TOOL} ${CMAKE_BINARY_DIR}/test1.txt | cut -d' ' -f1 > hash2.txt && diff hash1.txt hash2.txt"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

find_program(MD5SUM_TOOL md5sum)
if(MD5SUM_TOOL)
    add_test(NAME test_md5sum_comparison
        COMMAND bash -c "${CMAKE_BINARY_DIR}/rhasher MD5 ${CMAKE_BINARY_DIR}/test1.txt | cut -d' ' -f1 > hash1.txt && ${MD5SUM_TOOL} ${CMAKE_BINARY_DIR}/test1.txt | cut -d' ' -f1 > hash2.txt && diff hash1.txt hash2.txt"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()
