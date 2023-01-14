# Hands-In-The-Web

An example of how to bring mocap data from handy to the web.

## Libraries

To get this to work I had to build the [Imath](https://github.com/AcademySoftwareFoundation/Imath) and [Alembic](https://github.com/alembic/alembic) libraries using `emcmake`.

### Imath

To build `Imath` I did the following:

- Downloaded release 3.1.6
- Placed it in a folder called `Imath-3.1.6`
- Created a folder next to that one called `Imath_build`
- Created a folder next to the previous two called `Imath_install`
- In the root directory of my emscripten clone I ran this command:

```
source ./emsdk_env.sh
```

- Then I navigated to the `Imath_build` folder and ran these commands:

```
emcmake cmake -DIMATH_CXX_STANDARD=17 -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=/Users/maca/Desktop/Imath_install /Users/maca/Desktop/Imath-3.1.6
emmake make install
```

- The output of the build could then be found in the `Imath_install` folder.

### Alembic

To build `Alembic` I did the following:

- Cloned the Alembic repo into a folder called `alembic`
- Created a folder next to that one called `alembic_build`
- Opened this file: `/Users/maca/Desktop/alembic/cmake/AlembicIlmBase.cmake`
- Added this line at the very top:

```
SET(Imath_DIR /Users/maca/Desktop/Imath_install/lib/cmake/Imath)
```

- In the root directory of my emscripten clone I ran this command:

```
source ./emsdk_env.sh
```

- Then I navigated to the `alembic_build` folder and ran these commands:

```
emcmake cmake -DALEMBIC_SHARED_LIBS=OFF -DUSE_HDF5=OFF -DUSE_MAYA=OFF -DUSE_PYALEMBIC=OFF -DCMAKE_CXX_STANDARD=17 /Users/maca/Desktop/alembic
emmake make
emmake make install
```

- The output of the build could then be found in the following folder inside of my emscripten clone: `/upstream/emscripten/cache/sysroot`
