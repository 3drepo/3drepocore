3D Repo Core
========

C++ library providing 3D Repo Scene Graph definition and repository management logic.

### Dependencies

- [Assimp v3.1.1](https://github.com/assimp/assimp/tree/v3.1.1)
- [MongoDB 2.6 Compatible C++ Driver 2.6.4](https://github.com/mongodb/mongo-cxx-driver/tree/legacy-0.0-26compat-2.6.4)
- [Modular Boost 1.56.0](https://github.com/boostorg/boost/tree/boost-1.56.0)

#### Boost

For Windows, download pre-built binaries [http://sourceforge.net/projects/boost/files/boost-binaries/](http://sourceforge.net/projects/boost/files/boost-binaries/)

There is also a GitHub transition taking place:
[https://svn.boost.org/trac/boost/wiki/ModularBoost](https://svn.boost.org/trac/boost/wiki/ModularBoost)

If using Modular Boost, make sure to recursively update submodules since each boost library within the super projec is in itself a submodule.
