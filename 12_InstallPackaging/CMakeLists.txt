cmake_minimum_required(VERSION 3.10)
project(guess_number C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(Gettext REQUIRED)
find_package(Intl REQUIRED)

add_definitions(-DLOCALEDIR=\"${CMAKE_INSTALL_PREFIX}/share/locale\")

add_subdirectory(lib)

add_executable(guess guess.c)
target_link_libraries(guess roman ${Intl_LIBRARIES})
target_include_directories(guess PRIVATE ${Intl_INCLUDE_DIRS})
target_include_directories(guess PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_directories(guess PRIVATE ${CMAKE_BINARY_DIR}/lib)

install(TARGETS guess
    RUNTIME DESTINATION bin
)

install(FILES ${CMAKE_SOURCE_DIR}/guess.1
    DESTINATION share/man/man1
)

if(EXISTS ${MO_DIR}/ru/LC_MESSAGES/guess.mo)
    install(FILES ${MO_DIR}/ru/LC_MESSAGES/guess.mo
        DESTINATION share/locale/ru/LC_MESSAGES
    )
endif()

set(PO_DIR "${CMAKE_SOURCE_DIR}/po")
set(MO_DIR "${CMAKE_BINARY_DIR}/locale")

add_custom_target(pot
    COMMAND xgettext --keyword=_ --language=C -o ${PO_DIR}/guess.pot ${CMAKE_SOURCE_DIR}/guess.c
    COMMENT "Extracting messages to pot file")

add_custom_target(update_po
    COMMAND msgmerge --update ${PO_DIR}/ru.po ${PO_DIR}/guess.pot
    DEPENDS pot
    COMMENT "Updating Russian translations")

add_custom_command(
    OUTPUT ${MO_DIR}/ru/LC_MESSAGES/guess.mo
    COMMAND mkdir -p ${MO_DIR}/ru/LC_MESSAGES
    COMMAND msgfmt -o ${MO_DIR}/ru/LC_MESSAGES/guess.mo ${PO_DIR}/ru.po
    DEPENDS ${PO_DIR}/ru.po
    COMMENT "Building Russian translation file")

add_custom_target(translations ALL
    DEPENDS ${MO_DIR}/ru/LC_MESSAGES/guess.mo)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/README.md
    COMMAND ${CMAKE_BINARY_DIR}/guess --help-md > ${CMAKE_BINARY_DIR}/README.md
    DEPENDS guess
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating README.md from --help-md output"
)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(
        ${CMAKE_SOURCE_DIR}/Doxyfile.in
        ${CMAKE_BINARY_DIR}/Doxyfile
        @ONLY
    )
    
    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/doc/html/index.html
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
        DEPENDS ${CMAKE_BINARY_DIR}/README.md ${CMAKE_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating Doxygen documentation with README.md from build directory"
    )
    
    add_custom_target(docs
        DEPENDS ${CMAKE_BINARY_DIR}/doc/html/index.html
    )
else()
    message(WARNING "Doxygen not found: 'make docs' target will not be available")
endif()

enable_testing()

add_executable(test_roman tests/test_roman.c)
target_link_libraries(test_roman roman_static)
target_include_directories(test_roman PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_executable(test_game tests/test_game.c)

add_test(NAME test_roman COMMAND test_roman)
add_test(NAME test_game COMMAND test_game)

add_custom_target(check
    COMMAND ctest --output-on-failure
    DEPENDS test_roman test_game
    COMMENT "Running all tests"
)

add_custom_target(all_except_docs
    DEPENDS guess ${CMAKE_BINARY_DIR}/README.md test_roman test_game
)
