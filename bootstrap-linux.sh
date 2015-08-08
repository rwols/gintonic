#!/bin/sh

# Change the version number to download other versions
FBX_VERSION='20151'
FBX_INSTALL="fbx${FBX_VERSION}_fbxsdk_linux"
FBX_TAR="${FBX_INSTALL}.tar.gz"
FBX_DOWNLOAD_FOLDER='http://images.autodesk.com/adsk/files'

# Create a temporary directory
# The -p is there to suppress warnings if
# such a directory already exists
mkdir -p tmp
cd tmp

# Install CMake, Boost, libjpeg, libpng, SDL2, Freetype
sudo apt-get update
sudo apt-get install cmake libboost-dev libjpeg-dev libpng-dev libsdl2-dev libfreetype6-dev

# Install FBX SDK
wget -O $FBX_TAR $FBX_DOWNLOAD_FOLDER/$FBX_TAR
tar -xf $FBX_TAR
sudo ./$FBX_INSTALL /usr/local

# Clean up
cd ..
rm -rf tmp