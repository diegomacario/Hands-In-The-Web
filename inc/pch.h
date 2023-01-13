#ifndef PCH_H
#define PCH_H

#include <fstream>

#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcGeom/All.h>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/bind.h>
    #define wabcAPI EMSCRIPTEN_KEEPALIVE extern "C" __attribute__((visibility("default")))
#endif

#endif
