#!/bin/bash
# install.sh - Auto-install script for clical (CLI Calendar)

set -e

echo "🗓️  Installing clical (CLI Calendar)..."

# Detect Linux distribution
if [ -f /etc/fedora-release ]; then
    echo "📦 Fedora detected. Installing dependencies..."
    sudo dnf install -y readline-devel nlohmann-json-devel gcc-c++ cmake rpm-build
    echo "🔨 Building and installing clical..."
    mkdir -p build && cd build
    cmake ..
    make
    sudo make install
    echo "✅ Installation complete! Run 'clical' to start."

elif [ -f /etc/debian_version ]; then
    echo "📦 Debian/Ubuntu detected. Installing dependencies..."
    sudo apt update
    sudo apt install -y libreadline-dev nlohmann-json3-dev g++ cmake dpkg
    echo "🔨 Building and installing clical..."
    mkdir -p build && cd build
    cmake ..
    make
    sudo make install
    echo "✅ Installation complete! Run 'clical' to start."

elif [ -f /etc/arch-release ]; then
    echo "📦 Arch Linux detected. Installing dependencies..."
    sudo pacman -S --needed readline nlohmann-json gcc cmake
    echo "🔨 Building and installing clical..."
    mkdir -p build && cd build
    cmake ..
    make
    sudo make install
    echo "✅ Installation complete! Run 'clical' to start."

else
    echo "❌ Unsupported distribution. Please install dependencies manually:"
    echo "  - readline (development headers)"
    echo "  - nlohmann-json (development headers)"
    echo "  - g++ and cmake"
    echo "Then run: mkdir build && cd build && cmake .. && make && sudo make install"
    exit 1
fi