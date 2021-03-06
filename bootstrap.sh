#!/bin/bash

# TODO: install these:
# libasound2-dev libdbus-1-dev libdrm-dev libegl1-mesa-dev libgl1-mesa-dev libgles2-mesa-dev libglib2.0-dev libglu1-mesa-dev libice-dev libmirclient-dev libmircommon-dev libmircookie-dev libmircookie2 libpcre3-dev libpcre32-3 libpcrecpp0v5 libprotobuf-dev libpulse-dev libsm-dev libsndio-dev libudev-dev libwayland-dev libx11-xcb-dev libxcb-dri2-0-dev libxcb-glx0-dev libxcb-present-dev libxcb-randr0-dev libxcb-render0-dev libxcb-shape0-dev libxcb-sync-dev libxcb-xfixes0-dev libxcursor-dev libxdamage-dev libxext-dev libxfixes-dev libxi-dev libxinerama-dev libxkbcommon-dev libxrandr-dev libxrender-dev libxshmfence-dev libxss-dev libxt-dev libxv-dev libxxf86vm-dev mesa-common-dev x11proto-damage-dev x11proto-dri2-dev x11proto-fixes-dev x11proto-gl-dev x11proto-randr-dev x11proto-render-dev x11proto-scrnsaver-dev x11proto-video-dev x11proto-xext-dev x11proto-xf86vidmode-dev x11proto-xinerama-dev

# TODO: Turn this into a python script

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

FBX_INSTALL="fbx${FBX_VERSION}_fbxsdk_linux"
FBX_TAR="${FBX_INSTALL}.tar.gz"
FBX_DOWNLOAD_FOLDER='http://images.autodesk.com/adsk/files'

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

# Install clang-3.6, CMake, Boost, SDL2, Freetype
$PACKAGE_MANAGER update
$PACKAGE_MANAGER install $CLANG $CMAKE $BOOST $SDL2 $FREETYPE
if [ $? -ne $SUCCESS ] ; then
	exit $PACKAGE_MANAGER_FAILED
fi

# If the the folder $FBX_INSTALL_PREFIX/fbxsdk exists,
# then we assume that the FBX SDK is already installed.
# In that case, we skip the FBX SDK installation.
if ! [ -d "$FBX_INSTALL_PREFIX/include/fbxsdk" ]; then

	# Create a temporary directory, the "correct" way :-)
	CURRENTDIR=`pwd`
	TEMPDIR=`mktemp -d 2>/dev/null || mktemp -d -t 'mytmpdir'` && cd $TEMPDIR

	# Fetch installation files from the Autodesk website.
	wget -O $FBX_TAR $FBX_DOWNLOAD_FOLDER/$FBX_TAR

	# Untar.
	tar -xf $FBX_TAR

	# Install the FBX SDK.
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
else
	echo 'FBX SDK seems to be already installed. Skipping FBX SDK installation.'
fi

# Configure the project with CMake.
mkdir $BUILD_DIRECTORY
cd $BUILD_DIRECTORY
cmake ..
if [ $? -ne $SUCCESS ]; then
	echo $'\n\n\tFAILED to configure the project with CMake!\n\n'
	exit $CMAKE_FAILED_TO_CONFIGURE
fi

# Build the project.
make -j$NUM_THREADS
if [ $? -ne $SUCCESS ]; then
	echo $'\n\n\tFAILED to build the project!\n\n'
	exit $MAKE_FAILED_TO_BUILD
fi

# Run the unit tests.
ctest . -j$NUM_THREADS
if [ $? -ne $SUCCESS ]; then
	echo $'\n\n\tThe project has been built succesfully!'
	printf '\t'
	echo $'However, some unit tests failed.\n\n'
else
	echo $'\n\n\tThe project has been built succesfully!'
	printf '\t'
	echo "Go to the $BUILD_DIRECTORY/Examples directory and"
	echo $'\trun some of the examples. Have fun.\n\n'
fi

# If we arrive here then everything went OK.
exit $SUCCESS
