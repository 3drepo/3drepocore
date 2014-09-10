3D Repo Core
========

C++ library providing 3D Repo Scene Graph definition and repository management logic.

## Dependencies

- [Boost 1.56.0](https://github.com/boostorg/boost/tree/boost-1.56.0)
- [MongoDB 2.6 Compatible C++ Driver 2.6.4](https://github.com/mongodb/mongo-cxx-driver/tree/legacy-0.0-26compat-2.6.4)
- [Assimp v3.1.1](https://github.com/assimp/assimp/tree/v3.1.1)

Both MongoDB and Assimp make use of Boost, so make sure this is installed first. 

## Compiling on Windows 

### Boost

For Windows, download pre-built binaries [http://sourceforge.net/projects/boost/files/boost-binaries/](http://sourceforge.net/projects/boost/files/boost-binaries/)

There is also a GitHub transition taking place:
[https://svn.boost.org/trac/boost/wiki/ModularBoost](https://svn.boost.org/trac/boost/wiki/ModularBoost)
If using Modular Boost, make sure to recursively update submodules since each boost library within the super projec is in itself a submodule.

### MongoDB

Assuming you've installed the 64-bit pre-built boost libraries for VC120 (MSVS2013)

- Install [Python2x (32-bit)](https://www.python.org/download) and add Python to your PATH variable, e.g. C:\Python27;
- Install [Scons](http://www.scons.org/download.php) and add Scons to your PATH variable, e.g. C:\Python27\Scripts

Then compile MongoDB running the following:

```
scons --64 --mute --use-system-boost --extrapath=c:\local\boost_1_56_0 --cpppath=c:\local\boost_1_56_0 --libpath=c:\local\boost_1_56_0\lib64-msvc-12.0
```

### Assimp

When building using CMake GUI, make sure to disable ASSIMP_ENABLE_BOOST_WORKAROUND variable and instead use +Add Entry button on the right-hand side to include a path to your boost installation as BOOST_ROOT variable. 
See also [http://assimp.sourceforge.net/lib_html/cmake_build.html](http://assimp.sourceforge.net/lib_html/cmake_build.html)

To be able to compile Assimp Tools, you also need to install [DirectX SDK](http://www.microsoft.com/en-gb/download/details.aspx?id=6812). If you have Microsoft Visual C++ 2010 Redistributable, make sure to uninstall it from Programs and Features first.


## Compiling on Linux
