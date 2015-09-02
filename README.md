Special Considerations for Windows and Git
==========================================

Windows has no git in its terminal. Download [Git for Windows]
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

Add the contents of this key to your SSH keys in your account on BitBucket.

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