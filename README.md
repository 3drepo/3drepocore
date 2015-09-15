3D Repo Core (DEPRECATED)
========

This library is deprecated and has been replaced by [3drepobouncer](https://github.com/3drepo/3drepobouncer).


>! C++ library providing 3D Repo Scene Graph definition and repository management logic.

>! ## License
>! This project is copyright of [3D Repo Ltd](http://3drepo.org) and is released under the open source [GNU Affero General Public License v3](http://www.gnu.org/licenses/agpl-3.0.en.html). Should you require a commercial license, please contact [support@3drepo.org](mailto:support@3drepo.org)

>! ## Dependencies

>! - [Boost 1.56.0](https://github.com/boostorg/boost/tree/boost-1.56.0)
>! - [MongoDB 2.6 Compatible C++ Driver 2.6.4](https://github.com/mongodb/mongo-cxx-driver/tree/legacy-0.0-26compat-2.6.4)
>! - [Assimp v3.1.1](https://github.com/assimp/assimp/tree/v3.1.1)

>! Both MongoDB and Assimp make use of Boost, so make sure this is installed first. 

>! ## Compiling on Windows 

>! ### Boost

>! For Windows, download pre-built binaries [http://sourceforge.net/projects/boost/files/boost-binaries/](http://sourceforge.net/projects/boost/files/boost-binaries/)

>! There is also a GitHub transition taking place:
>! [https://svn.boost.org/trac/boost/wiki/ModularBoost](https://svn.boost.org/trac/boost/wiki/ModularBoost)
>! If using Modular Boost, make sure to recursively update submodules since each boost library within the super projec is in itself a submodule.

>! ### MongoDB

>! Assuming you've installed the 64-bit pre-built boost libraries for VC120 (MSVS2013)

>! - Install [Python2x (32-bit)](https://www.python.org/download) and add Python to your PATH variable, e.g. C:\Python27;
>! - Install [Scons](http://www.scons.org/download.php) and add Scons to your PATH variable, e.g. C:\Python27\Scripts
>! - Optionally, you can also install [Pywin32](http://sourceforge.net/projects/pywin32/files/pywin32/) to enable parallel compile (the -j2 flag below).

>! Then compile MongoDB running the following from within the mongo-cxx-driver folder (modify the boost path to match your setup):

```
>! scons -j2 --full --64 --mute --sharedclient --dynamic-windows --use-system-boost --extrapath=c:\local\boost_1_56_0 --cpppath=c:\local\boost_1_56_0 --libpath=c:\local\boost_1_56_0\lib64-msvc-12.0 install-mongoclient
```
>! Add --dbg=on --opt=on flags to build a debug version of the library.

>! Alternatively, you can include [SCons in your IDE](http://www.scons.org/wiki/IDEIntegration).

>! Include the WinSock library in your application: Linker > Input > Additional Dependencies. Add ws2_32.lib.

>! If you want to build a Qt project, first you need to generate MongoDB error codes using Python:

```
>! cd submodules/mongo-cxx-driver/src/mongo/base
>! python generate_error_codes.py error_codes.err error_codes.h error_codes.cpp
```

>! ### Assimp

>! When building using CMake GUI, make sure to disable ASSIMP_ENABLE_BOOST_WORKAROUND variable and instead use +Add Entry button on the right-hand side to include a path to your boost installation as BOOST_ROOT variable. 
>! See also [http://assimp.sourceforge.net/lib_html/cmake_build.html](http://assimp.sourceforge.net/lib_html/cmake_build.html)

>! To be able to compile Assimp Tools, you also need to install [DirectX SDK](http://www.microsoft.com/en-gb/download/details.aspx?id=6812). If you have Microsoft Visual C++ 2010 Redistributable, make sure to uninstall it from Programs and Features first.

>! ### 3drepocore

>! Finally to compile the library itself, open the build.pro project from qtcreator and build.

>! ## Compiling on Mac

>! The following instructions explain how to compile the 3drepocore library on Yosemite.

>! You will need boost, scons and cmake installed on your machine. You can get them from homebrew:

```
>! brew install boost
>! brew install scons
>! brew install cmake
```

>! ### MongoDB

>! To compile MongoDB, run the following command:

```
>! scons -j2 --full --64 --mute --sharedclient --osx-version-min=10.9 --use-system-boost install-mongoclient
```

>! ### Assimp

>! Assimp uses cmake. Make a folder named **build** at the root of the assimp submodule directly. Then, run the following commands:

```
>! cd build
>! ccmake ..
```

>! Choose the following options:

```
>! ASSIMP_ENABLE_BOOST_WORKAROUND -> off
>! ASSIMP_BUILD_TESTS -> off
```

>! Generate the project, and finally run:

```
>! make -j8
```

>! ### 3drepocore

>! Finally to compile the library itself, open the build.pro project from qtcreator and build.

>! ## Compiling on Linux

>! ## Contributing
>! We encourage continuous development of 3D Repo by the community and welcome any fixes and improvements. By sending a pull request, you agree that the modifications are solely your work and assign the copyright to 3D Repo Ltd
