Introduction
============

Gintonic is the name of an engine framework written in C++ using OpenGL.

Dependencies of Gintonic
========================

Gintonic depends on the following software:

* CMake -- For building
* Boost libraries -- Cross-platform filesystem, serialization, iostreams, etc.
* Freetype -- For rendering fonts
* libjpeg -- If on OSX/Linux, for texture loading
* libpng -- If on OSX/Linux, for texture loading
* SDL2 -- For a cross-platform windowing system
* FBX SDK -- For importing meshes, materials, light, etc.

Installing Windows Dependencies
===============================

You do not have to install libjpeg and libpng. Loading images into memory can 
be done with the WIC API of Windows. There is no universal way to install
dependencies on Windows, so it can be a bit of a pain to do this. CMake has an
installer, so no problem there. I recommend downloading the precompiled 
binaries of SDL2. The website of the Freetype project has some links to
websites which provide precompiled binaries. I suggest making a separate
folder for each dependency in your top-level directory. For instance,
C:\Freetype, C:\SDL2, and so forth. The FBX SDK has an installer, so that's
nice too. The FBX SDK will be installed in %programfiles%\Autodesk\FBX. Once
the dependencies are installed, you need to setup some environment variables.
The top-level CMakeLists.txt file expects the following environment variables
to be defined.

* BOOST_ROOT: Location of Boost (e.g. C:\Boost)
* SDL2: Location of the SDL2 directory (e.g. C:\SDL2)
* FREETYPE_DIR: Location of the Freetype library (e.g. C:\Freetype)

In addition, if for some reason CMake cannot find the FBX SDK, you can set an
environment variable called FBX_ROOT to point to the root FBX folder.

Installing OSX Dependencies
===========================

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

Installing Linux Dependencies
=============================

In the top-level directory there's a file called `bootstrap-linux.sh`.
Run that file to install all dependencies.

Git Instructions
================

The Gintonic git repo is hosted at a site called "The Storehouse". It's a 
pretty obscure site, I think it's a site managed by just one guy. It's also a 
new site, so some things are still a bit buggy. In particular, setting up SSH 
keys seems to fail everytime. If you are a new user setting up an SSH key, 
let me (Raoul) know so that I can email the guy behind Storehouse.

For every machine that you want to use the Gintonic git repo with, you NEED to
set up an SSH key for that machine.

Once you have an SSH key set up (see section below on how to do that), you can
do

	$ git clone <your-username>@sthse.co:/code/gt.git

to clone the repo.

Special Considerations for Windows and Git
==========================================

Windows has no git in its terminal. Download "Git for Windows"
(https://git-scm.com/download/win) to get a Unix environment in Windows. Open 
a new terminal with Git Bash, and don't ever use the native Windows terminal 
anymore. From this point on I will assume that Windows users use the Git Bash 
terminal instead of the native terminal.

Setting up an SSH key
=====================

Execute the following command.

	$ cd ~/.ssh

You are now inside a hidden folder called .ssh that is located in your home 
folder. Check if you already have a public key:

    $ ls | grep id_rsa.pub

If such a file is present, you can skip the steps on how to generate a public 
key. If there is no such file, read on. To generate a public key, call

	$ ssh-keygen

A bunch of questions and options are given to you, but just press enter a few 
times because the default values are fine. After this, a file called 
id_rsa.pub should now be present in ~/.ssh. Now do

	$ cat id_rsa.pub

to get the content of the file. Copy the content of the file to the clipboard.

Next, go to

	http://sthse.co

and log in with your user name and password. Click in the upper right corner
on "Account" and then click on "SSH Keys". Click on "Add Key". Give a name for
your public key. For example, the name of your machine or your current 
location. Paste the content of the clipboard as the key. If done correctly, 
a "key fingerprint" will be shown with a bunch of symbols. If not, it will say
"invalid key".

Unfortunately (as of 07/08/2015) there seems to be an issue with SSH keys not 
getting updated so if you added a new key let me know and I'll contact the guy
from Storehouse so that he'll update the keys.

Build Instructions
==================

We use CMake for building. CMake is not a compiler, but a cross-platform
build generator. What this is means in practise is that it generates Visual
C++ project files on Windows, and it generates Makefiles on Unix. The files
that are generated can then be build using a native compiler.

cd into the project root folder, and create a new directory where the build
will go. I usually use the name "build". If you go with another directory
name, then be sure to add that directory name to the .gitignore file. We don't
want to have build files checked in. When you have made your build folder, cd
into it and call CMake. CMake requires that you specify the directory of the
root CMakeLists.txt file. For example, if the engine source code is located
in ~/dev/gt, then you can create a new folder called build at ~/dev/gt/build.
Then cd into ~/dev/gt/build. Then call CMake with

	$ cmake ..

to generate Makefiles. After a succesful configuration stage of CMake, you can
then call

    $ make

to build the engine library, the example executables and the unit tests on
Unix/OSX. On Windows, you can call

    $ cmake --build .

By default, on Windows a multithreaded build is started. On Unix/OSX this is 
not the case. To use multithreaded building on those platforms you can do

    $ make -jN

where N is the number of threads that you want to devote to building. A good 
choice is the number of (logical) processors on the machine. For instance, if 
you have an Intel CPU with 2 physical cores, then make -j4 is a good choice.

To run all unit tests on Unix/OSX, simply call

	$ make test

I still have no idea how to run all unit tests on Windows. You can manually 
call them by their executable though.

The Project File Structure
==========================

The root folder has five directories:

* lib -- This is where the source code resides for the engine.
* s -- This is where all the OpenGL shaders reside.
* test -- This is where the unit tests are.
* examples -- Some example applications are here.
* resources -- The place for supporting files such as textures etc.

In addition, there's the top-level CMakeLists.txt file, and two support files 
FindFBX.cmake and FindSDL.cmake so that CMake can find those libs.

The Structure of the Engine
===========================

My suggestion is to explore the code in the examples directory to get a feel
for how the various classes interact with eachother. Basically, there's a huge
singleton class in renderer.hpp that takes care of rendering. There are
vectors, matrices and quaternion classes in math.hpp.

Rendering geometry is done with the mesh class in mesh.hpp. However if you 
need simple geometric shapes you could look into basic_shapes.hpp. When 
rendering a mesh, you need to bind a material so that the correct shader gets
activated. This interplay is in the files materials.hpp and shaders.hpp. Every
class in shaders.hpp derives from the gintonic::opengl::shader class and adds
some extra methods that correspond to uniform variables in the corresponding
shader with the same name (as the class). Each material in materials.hpp
(again with the same naming scheme) then references its corresponding shader
class and sets up the uniform variables in the shader given its own data
members. Each new shader class needs to be known to the giant singleton class
in renderer.hpp (called gintonic::renderer). Currently, I just give it a new
static method that fetches the new shader class (and add a new static pointer
to the new class). This works for now, but could probably benefit from a more
generic approach.

I wrote the classes in math.hpp a very long time ago. I believe they could use
a rewrite using SSE intrinsics. The vector and matrix class being templated
is pointless.

The classes in lights.hpp represent a light in space. Currently, I have
implemented a directional light and a point light. Just as with materials,
each light has a corresponding shader class and a corresponding shader.