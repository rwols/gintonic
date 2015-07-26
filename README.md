Gintonic depends on the following software:

* CMake -- For building
* Boost libraries -- Cross-platform filesystem, serialization, iostreams, etc.
* Freetype -- For rendering fonts
* libjpeg -- If on OSX/Linux, for texture loading
* libpng -- If on OSX/Linux, for texture loading
* SDL2 -- For a cross-platform windowing system
* FBX SDK -- For importing meshes, materials, light, etc.

Windows
=======

You do not have to install libjpeg and libpng. Loading images into memory can 
be done with the WIC API of Windows. I probably want to do the same with OSX, 
using the Cocoa API, but that's another story. There is no universal way to 
install dependencies on Windows, so it can be a bit of a pain to do this. CMake
has an installer, so no problem there. I recommend downloading the precompiled 
binaries of SDL2. The website of the Freetype project has some links to websites
which provide precompiled binaries. I suggest making a separate folder for each
dependency in your top-level directory. For instance, C:\Freetype, C:\SDL2, and
so forth. Once the dependencies are installed, you need to setup some
environment variables. The top-level CMakeLists.txt file expects the following
environment variables to be defined.

* BOOST_ROOT: Location of Boost (e.g. C:\Boost)
* FBX_DIR: Location of the FBX SDK directory 
	(e.g. %PROGRAMFILES%\Autodesk\FBX SDK\)
* SDL2: Location of the SDL2 directory (e.g. C:\SDL2)
* FREETYPE_DIR: Location of the Freetype library (e.g. C:\Freetype)

OSX
===

The best way to install the dependencies is via Homebrew. Go to http://brew.sh
and read the instructions on how to install Homebrew. Once installed, you can
type

	brew install cmake boost freetype libjpeg libpng sdl2

In a terminal to install all the dependencies except for the FBX SDK. The FBX
SDK needs to be installed manually (with an install wizard). As of this
writing, the most recent FBX SDK is version 2016.1. This can be downloaded
from

http://download.autodesk.com/us/fbx/20161/fbx20161_fbxsdk_clang_mac.pkg.tgz

You may need to edit some variables in the top-level CMakeLists.txt file
of the Gintonic project to get everything to work.

Linux
=====

Use apt-get to install the dependencies. The following command should work:

	sudo apt-get install cmake boost freetype libjpeg libpng sdl2
	
Download the FBX SDK and follow the instructions for Linux.
