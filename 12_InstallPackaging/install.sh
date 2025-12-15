#!/bin/bash

set -e  # Exit on any error

PREFIX=${PREFIX:-/usr/local}

if [[ $EUID -eq 0 ]]; then
    echo "Installing system-wide to $PREFIX (requires root privileges)"
else
    echo "Installing to user directory $PREFIX (use sudo for system-wide install)"
fi

if [ ! -d "build" ]; then
    mkdir build
fi

cd build

cmake .. -DCMAKE_INSTALL_PREFIX=$PREFIX -DCMAKE_INSTALL_RPATH=\$ORIGIN/../lib
make -j$(nproc)

if [[ $EUID -eq 0 ]]; then
    make install
else
    sudo make install
fi

if command -v mandb &> /dev/null && [[ $EUID -eq 0 ]]; then
    echo "Updating manpage database..."
    mandb
fi

if [ -d "locale/ru/LC_MESSAGES" ]; then
    if [[ $EUID -eq 0 ]]; then
        mkdir -p $PREFIX/share/locale/ru/LC_MESSAGES/
        cp locale/ru/LC_MESSAGES/guess.mo $PREFIX/share/locale/ru/LC_MESSAGES/
    else
        echo "Translation files installed to build directory, manual installation required for system-wide"
    fi
fi

echo "Installation complete!"
echo "Try running: guess"
echo "Or with Roman numerals: guess --roman"
