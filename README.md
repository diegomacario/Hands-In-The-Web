# Hands-In-The-Web

An example of how to bring mocap data from handy to the web.

## Credit

- The geisha mask was created by [naxete](https://sketchfab.com/naxete). It can be found [here](https://sketchfab.com/3d-models/ghost-in-the-shell-geisha-mask-79df6677512249e9b98b898676182e1b).
- The samurai mask was created by [flioink](https://sketchfab.com/flioink). It can be found [here](https://sketchfab.com/3d-models/samurai-mask-model-2-70a43b389819494f9fa23065ede03afc).

## Building

To build this project for the web I did the following:

- In the root directory of my emscripten clone I ran this command:

```
source ./emsdk_env.sh
```

- Then I navigated to the `build` folder that's in the root of this repository and ran these commands:

```
emcmake cmake ..
make
```

- After that I ran a local webserver with this command:

```
python -m http.server
```

- That allowed me to see the project running here: http://localhost:8000/

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
