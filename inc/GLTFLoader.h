#ifndef GLTF_LOADER_H
#define GLTF_LOADER_H

#include <vector>

#include "cgltf/cgltf.h"

#include "StaticMesh.h"
#include "SimpleMesh.h"

cgltf_data*               LoadGLTFFile(const char* path);
void                      FreeGLTFFile(cgltf_data* handle);

std::vector<StaticMesh>   LoadStaticMeshes(cgltf_data* data);
std::vector<SimpleMesh>   LoadSimpleMeshes(cgltf_data* data);

#endif
