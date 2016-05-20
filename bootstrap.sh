#!/bin/bash

SUCCESS=0
PACKAGE_MANAGER_FAILED=1
FBX_INSTALLATION_FAILED=2
CMAKE_FAILED_TO_CONFIGURE=3
MAKE_FAILED_TO_BUILD=4
OS_DETECTION_FAILED=5
USER_ABORT=6

if [[ "$OSTYPE" == "linux-gnu" ]]; then
	echo "We're on Linux/GNU."
	PACKAGE_MANAGER='sudo apt-get'
	CLANG='clang-3.6'
	BOOST='libboost-all-dev'
	SDL2='libsdl2-dev'
	CMAKE='cmake'
	FREETYPE='libfreetype6-dev'
	# Get the number of CPU threads available for building.
	NUM_THREADS=`grep -c ^processor /proc/cpuinfo`
elif [[ "$OSTYPE" == "darwin"* ]]; then
	echo "We're on a Mac."
	PACKAGE_MANAGER='brew'
	which $PACKAGE_MANAGER
	if [ $? -ne $SUCCESS ]; then
		# Install homebrew
		echo "Installing homebrew..."
		curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install
	fi
	CLANG=''
	BOOST='boost'
	SDL2='sdl2'
	CMAKE='cmake'
	FREETYPE='freetype'
	# Get the number of CPU threads available for building.
	NUM_THREADS=`sysctl -n hw.ncpu`
elif [[ "$OSTYPE" == "cygwin" ]]; then
	echo "We're on Cygwin."
	exit $OS_DETECTION_FAILED
elif [[ "$OSTYPE" == "msys" ]]; then
	echo "We're on MSYS."
	exit $OS_DETECTION_FAILED
elif [[ "$OSTYPE" == "win32" ]]; then
	echo "We're on windows... ?"
	exit $OS_DETECTION_FAILED
elif [[ "$OSTYPE" == "freebsd"* ]]; then
	echo "We're on FreeBSD."
	exit $OS_DETECTION_FAILED
else
	echo "Unknown OS type: $OSTYPE"
	exit $OS_DETECTION_FAILED
fi

# Change the version number to download other versions
: ${FBX_VERSION:='20151'}

# Change the install prefix to install the FBX SDK somewhere else.
: ${FBX_INSTALL_PREFIX:='/usr/local'}

# Change the build directory to build the project somewhere else.
: ${BUILD_DIRECTORY:='build'}

# Tell the user what the script is going to do.
echo $'\nThis script will install:'
echo ' * clang-3.6 (compiler)'
echo ' * CMake'
echo ' * Boost libraries and headers'
echo ' * SDL2 library and headers'
echo ' * Freetype library and headers'
echo ' * FBX SDK library and headers'
echo $'If any of these are already installed, then the installation will be skipped.'
echo $"The FBX SDK will be downloaded from Autodesk's website as a tar file."
echo "The FBX SDK version that will be fetched is: $FBX_VERSION"
echo "It will then be installed in $FBX_INSTALL_PREFIX."
echo $'All other libraries and tools will be installed via apt-get.'
echo $'After the dependencies have been installed, the project will be built'
echo "with CMake in the directory $BUILD_DIRECTORY"
echo $'Be ready to have about 2GB of free memory available.'
echo $'Also be ready to answer some yes/no questions at some points'
echo $'And some parts may need sudo access, so be ready for that too.'

printf '\n'
echo " * FBX_VERSION:        $FBX_VERSION"
echo " * FBX_INSTALL_PREFIX: $FBX_INSTALL_PREFIX"
echo " * BUILD_DIRECTORY:    $BUILD_DIRECTORY"
echo " * NUM_THREADS:        $NUM_THREADS"
printf '\n'

# Ask the user to continue at this point.
read -p $'Continue? (y/n) ' -n 1 -r
echo $'\n'
if ! [[ $REPLY =~ ^[Yy]$ ]] ; then exit $USER_ABORT ; fi

# Create a temporary directory
# The "correct" way :-)
CURRENTDIR=`pwd`
TEMPDIR=`mktemp -d 2>/dev/null || mktemp -d -t 'mytmpdir'` && cd $TEMPDIR

# Install clang-3.6, CMake, Boost, SDL2, Freetype
$PACKAGE_MANAGER update
$PACKAGE_MANAGER install $CLANG $CMAKE $BOOST $SDL2 $FREETYPE
if [ $? -ne $SUCCESS ] ; then
	exit $PACKAGE_MANAGER_FAILED
fi

# Install FBX SDK
FBX_INSTALL="fbx${FBX_VERSION}_fbxsdk_linux"
FBX_TAR="${FBX_INSTALL}.tar.gz"
FBX_DOWNLOAD_FOLDER='http://images.autodesk.com/adsk/files'
wget -O $FBX_TAR $FBX_DOWNLOAD_FOLDER/$FBX_TAR
tar -xf $FBX_TAR
sudo ./$FBX_INSTALL $FBX_INSTALL_PREFIX
if [ $? -eq $SUCCESS ]; then
	continue
else
	exit $FBX_INSTALLATION_FAILED
fi

# Go back to our source directory.
cd $CURRENTDIR

# Clean up the temporary directory.
rm -rf $TEMPDIR

# Start building the project.
mkdir $BUILD_DIRECTORY
cd $BUILD_DIRECTORY
cmake ..
if [ $? -eq $SUCCESS ]; then
	make -j$NUM_THREADS
	if [ $? -eq $SUCCESS ]; then
		echo $'\n\n\tThe project has been built succesfully!'
		printf '\t'
		echo "Go to the $BUILD_DIRECTORY/Examples directory and"
		echo $'\trun some of the examples. Have fun.\n\n'
	else
		echo $'\n\n\tFAILED to build the project!\n\n'
		exit $MAKE_FAILED_TO_BUILD
	fi
else
	echo $'\n\n\tFAILED to configure the project with CMake!\n\n'
	exit $CMAKE_FAILED_TO_CONFIGURE
fi

exit $SUCCESS