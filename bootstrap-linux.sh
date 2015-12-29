#!/bin/bash

# Check if we are root.
# We need root access for apt-get and to write to /usr/local
if ! [[ "$UID" -eq 0 ]] ; then
	echo $'Please run this script as root. Run:\n'
	echo $'\tsudo '$0$'\n'
	exit 1
fi

# Change the version number to download other versions
FBX_VERSION='20151'

# Tell the user what the script is going to do.
echo $'\nThis script will install:\n'
echo $' * clang-3.6 (compiler)'
echo $' * CMake'
echo $' * Boost libraries and headers'
echo $' * jpeg library and headers'
echo $' * png library and headers'
echo $' * SDL2 library and headers'
echo $' * Freetype library and headers'
echo $' * FBX SDK library and headers\n'
echo $'If any of these are already installed, then the installation will be skipped.'
echo $"The FBX SDK will be downloaded from Autodesk's website as a tar file."
echo "The FBX SDK version that will be fetched is: $FBX_VERSION"
echo $'It will then be installed in /usr/local. All other libraries and tools will be installed via apt-get.'
echo $'Be ready to have about 2GB of free memory available.'

# Ask the user to continue at this point.
read -p $'Continue? (y/n) ' -n 1 -r
echo $'\n'
if ! [[ $REPLY =~ ^[Yy]$ ]] ; then exit 1 ; fi

FBX_INSTALL="fbx${FBX_VERSION}_fbxsdk_linux"
FBX_TAR="${FBX_INSTALL}.tar.gz"
FBX_DOWNLOAD_FOLDER='http://images.autodesk.com/adsk/files'

# Create a temporary directory
# The "correct" way :-)
CURRENTDIR=`pwd`
TEMPDIR=`mktemp -d` && cd $TEMPDIR

# Install CMake, Boost, libjpeg, libpng, SDL2, Freetype
# apt-get update
# apt-get install cmake libboost-all-dev libjpeg-dev libpng-dev libsdl2-dev libfreetype6-dev

# Install FBX SDK
# wget -O $FBX_TAR $FBX_DOWNLOAD_FOLDER/$FBX_TAR
# tar -xf $FBX_TAR
# ./$FBX_INSTALL /usr/local

# Clean up
rm -rf $TEMPDIR

# Go back to our source directory.
cd $CURRENTDIR

# Get the number of CPU threads available for building.
NUM_THREADS=`grep -c ^processor /proc/cpuinfo`

# Start building the project.
mkdir build
cd build
cmake ..
make -j$NUM_THREADS