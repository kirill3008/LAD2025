cmake_minimum_required(VERSION 3.10)
project(guess_number C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find gettext package
find_package(Gettext REQUIRED)
find_package(Intl REQUIRED)

# Define LOCALEDIR to be used in the C code
add_definitions(-DLOCALEDIR="${CMAKE_BINARY_DIR}/locale")

# Add executable
add_executable(guess guess.c)
target_link_libraries(guess ${Intl_LIBRARIES})
target_include_directories(guess PRIVATE ${Intl_INCLUDE_DIRS})

# Simplified internationalization setup
set(PO_DIR "${CMAKE_SOURCE_DIR}/po")
set(MO_DIR "${CMAKE_BINARY_DIR}/locale")

# Create a target to generate the POT file
add_custom_target(pot
    COMMAND xgettext --keyword=_ --language=C -o ${PO_DIR}/guess.pot ${CMAKE_SOURCE_DIR}/guess.c
    COMMENT "Extracting messages to pot file")

# Target to update PO files
add_custom_target(update_po
    COMMAND msgmerge --update ${PO_DIR}/ru.po ${PO_DIR}/guess.pot
    DEPENDS pot
    COMMENT "Updating Russian translations")

# Build Russian MO file
add_custom_command(
    OUTPUT ${MO_DIR}/ru/LC_MESSAGES/guess.mo
    COMMAND mkdir -p ${MO_DIR}/ru/LC_MESSAGES
    COMMAND msgfmt -o ${MO_DIR}/ru/LC_MESSAGES/guess.mo ${PO_DIR}/ru.po
    DEPENDS ${PO_DIR}/ru.po
    COMMENT "Building Russian translation file")

# Make sure MO files are built
add_custom_target(translations ALL
    DEPENDS ${MO_DIR}/ru/LC_MESSAGES/guess.mo)
