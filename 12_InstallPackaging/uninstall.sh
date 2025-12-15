#!/bin/bash
set -e

PREFIX=${PREFIX:-/usr/local}

# Check if running as root
if [[ $EUID -eq 0 ]]; then
    echo "Uninstalling system-wide from $PREFIX (requires root privileges)"
else
    echo "Uninstalling from user directory $PREFIX"
fi

REMOVE_FILES=(
    "$PREFIX/bin/guess"
    "$PREFIX/share/man/man1/guess.1"
    "$PREFIX/share/locale/ru/LC_MESSAGES/guess.mo"
    "$PREFIX/include/roman.h"
    "$PREFIX/lib/libroman.so"
    "$PREFIX/lib/libroman.a"
    "$PREFIX/lib/pkgconfig/roman.pc"
)

REMOVE_DIRS=(
    "$PREFIX/share/doc/number-game/"
)

echo "Removing installed files..."
for file in "${REMOVE_FILES[@]}"; do
    if [ -f "$file" ]; then
        if [[ $EUID -eq 0 ]]; then
            rm -f "$file"
        else
            rm -f "$file" 2>/dev/null || echo "Could not remove $file (need sudo)"
        fi
        echo "Removed: $file"
    fi
    
    dir=$(dirname "$file")
    if [ -d "$dir" ] && [ -z "$(ls -A "$dir" 2>/dev/null)" ]; then
        if [[ $EUID -eq 0 ]]; then
            rmdir "$dir" 2>/dev/null || true
        else
            rmdir "$dir" 2>/dev/null || true
        fi
    fi
done

echo "Removing documentation directories..."
for dir in "${REMOVE_DIRS[@]}"; do
    if [ -d "$dir" ]; then
        if [[ $EUID -eq 0 ]]; then
            rm -rf "$dir"
        else
            rm -rf "$dir" 2>/dev/null || echo "Could not remove $dir (need sudo)"
        fi
        echo "Removed: $dir"
    fi
done

if command -v mandb &> /dev/null && [[ $EUID -eq 0 ]]; then
    echo "Updating manpage database..."
    mandb
fi

echo "Uninstallation complete!"
