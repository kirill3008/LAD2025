cmake_minimum_required(VERSION 3.10)
project(guess_number C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(Gettext REQUIRED)
find_package(Intl REQUIRED)

add_definitions(-DLOCALEDIR="${CMAKE_BINARY_DIR}/locale")

add_executable(guess guess.c)
target_link_libraries(guess ${Intl_LIBRARIES})
target_include_directories(guess PRIVATE ${Intl_INCLUDE_DIRS})

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
    COMMAND ./guess --help-md > ${CMAKE_BINARY_DIR}/README.md
    DEPENDS guess
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating README.md from --help-md output"
)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/doc/html/index.html
        COMMAND cd ${CMAKE_SOURCE_DIR} && ${DOXYGEN_EXECUTABLE} Doxyfile
        DEPENDS ${CMAKE_BINARY_DIR}/README.md
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating Doxygen documentation"
    )
    
    add_custom_target(docs
        DEPENDS ${CMAKE_BINARY_DIR}/doc/html/index.html
    )
else()
    message(WARNING "Doxygen not found: 'make docs' target will not be available")
endif()

add_custom_target(all_except_docs
    DEPENDS guess ${CMAKE_BINARY_DIR}/README.md
)
