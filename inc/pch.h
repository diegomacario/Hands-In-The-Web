#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <functional>
#include <memory>
#include <fstream>
#include <chrono>
#ifdef __cpp_lib_span
    #include <span>
#endif

#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
//#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcMaterial/All.h>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/bind.h>
    #define wabcAPI EMSCRIPTEN_KEEPALIVE extern "C" __attribute__((visibility("default")))
#else
    #ifdef _WIN32
        #define wabcAPI extern "C" __declspec(dllexport)
    #else
        #define wabcAPI extern "C" __attribute__((visibility("default")))
    #endif
#endif // __EMSCRIPTEN__
