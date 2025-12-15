cmake_minimum_required(VERSION 3.10)

add_library(roman SHARED roman.c)

add_library(roman_static STATIC roman.c)
set_target_properties(roman_static PROPERTIES OUTPUT_NAME roman)

set_target_properties(roman PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
)

install(TARGETS roman roman_static
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES roman.h
    DESTINATION include
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/roman.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/roman.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/roman.pc
    DESTINATION lib/pkgconfig
)
