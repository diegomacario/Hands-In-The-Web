# Hands-In-The-Web

<p align="center">
 <img alt="Header" src="https://github.com/diegomacario/Hands-In-The-Web/blob/main/readme_images/social_preview.png">
</p>

An example of how to bring mocap data captured with Shopify's [handy](https://github.com/Shopify/handy) tool to the web.

## Technical details

This project was written using C++ and OpenGL. It was then compiled to WebAssembly using [Emscripten](https://emscripten.org). The open source libraries it uses and their purposes are the following:

- [GLFW](https://www.glfw.org/) is used to receive inputs.
- [Dear ImGui](https://github.com/ocornut/imgui) is used for the user interface.
- [GLAD](https://glad.dav1d.de/) is used to load pointers to OpenGL functions.
- [GLM](https://glm.g-truc.net/0.9.9/index.html) is used to do linear algebra.
- [stb_image](https://github.com/nothings/stb) is used to load textures.
- [cgltf](https://github.com/jkuhlmann/cgltf) is used to parse glTF files.
- [Imath](https://github.com/AcademySoftwareFoundation/Imath) and [Alembic](https://github.com/alembic/alembic) are used to load motion capture data.

The code that reads and plays Alembic files was written by [i-saint](https://github.com/i-saint). It can be found [here](https://github.com/i-saint/WebAlembicViewer).

The geisha mask was created by [naxete](https://sketchfab.com/naxete). It can be found [here](https://sketchfab.com/3d-models/ghost-in-the-shell-geisha-mask-79df6677512249e9b98b898676182e1b).

The samurai mask was created by [flioink](https://sketchfab.com/flioink). It can be found [here](https://sketchfab.com/3d-models/samurai-mask-model-2-70a43b389819494f9fa23065ede03afc).

## Building

This project can be built as a Windows desktop application, a macOS desktop application, and a website.

To build it as a Windows desktop application use the Visual Studio 2022 solution that's inside the `VS2022_solution` folder. Note that the solution can only be built in `Release` mode right now.

To build it as a macOS desktop application use the Xcode project that's inside the `Xcode_Project` folder.

To build it as a website you will first need to clone [Emscripten](https://emscripten.org), and then you will need to follow the steps below. Note that I have only tried this on macOS.

- In the root directory of your emscripten clone run this command:

```
source ./emsdk_env.sh
```

- Then navigate to the `build` folder that's in the root of this repository and run these commands:

```
emcmake cmake ..
make
```

- After that you can run a local webserver with this command:

```
python -m http.server
```

- That will allow you to see the project running here: http://localhost:8000/

## Building Alembic and Imath

If you observe linker errors when you try to build the project as a website, then you probably need to build the [Imath](https://github.com/AcademySoftwareFoundation/Imath) and [Alembic](https://github.com/alembic/alembic) libraries using `emcmake`. Below are the steps I followed to do that. Note that I have only tried this on macOS.

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
emcmake cmake -DIMATH_CXX_STANDARD=17 -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=/Users/JaneDoe/Desktop/Imath_install /Users/JaneDoe/Desktop/Imath-3.1.6
emmake make install
```

- The output of the build could then be found in the `Imath_install` folder
- Finally, I updated the contents of these folders with the output:

```
<root_of_this_repository>/dependencies/Imath/inc/mac
<root_of_this_repository>/dependencies/Imath/lib/mac
```

### Alembic

To build `Alembic` I did the following:

- Cloned the Alembic repo into a folder called `alembic`
- Created a folder next to that one called `alembic_build`
- Created a folder next to the previous two called `alembic_install`
- Opened this file: `/Users/JaneDoe/Desktop/alembic/cmake/AlembicIlmBase.cmake`
- Added this line at the very top:

```
SET(Imath_DIR /Users/JaneDoe/Desktop/Imath_install/lib/cmake/Imath)
```

- In the root directory of my emscripten clone I ran this command:

```
source ./emsdk_env.sh
```

- Then I navigated to the `alembic_build` folder and ran these commands:

```
emcmake cmake -DALEMBIC_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=/Users/JaneDoe/Desktop/alembic_install -DUSE_HDF5=OFF -DUSE_MAYA=OFF -DUSE_PYALEMBIC=OFF -DCMAKE_CXX_STANDARD=17 /Users/JaneDoe/Desktop/alembic
emmake make
emmake make install
```

- The output of the build could then be found in the `alembic_install` folder
- Finally, I updated the contents of these folders with the output:

```
<root_of_this_repository>/dependencies/Alembic/inc/mac/Alembic
<root_of_this_repository>/dependencies/Alembic/lib/mac
```
