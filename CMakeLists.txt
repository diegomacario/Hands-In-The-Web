cmake_minimum_required(VERSION 3.11.1 FATAL_ERROR)

project(Hands-In-The-Web)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../game/client/dist)

include_directories("${CMAKE_SOURCE_DIR}/inc"
                    "${CMAKE_SOURCE_DIR}/dependencies/win/inc/desktop_and_web"
                    "${CMAKE_SOURCE_DIR}/dependencies/draco")

set(project_headers
    inc/AABB.h
    inc/AnimatedMesh.h
    #inc/AudioEngine.h
    #inc/AudioSource.h
    inc/Blending.h
    #inc/camera.h
    inc/Camera3.h
    inc/Clip.h
    inc/CrossFadeControllerMultiple.h
    inc/CrossFadeTarget.h
    inc/finite_state_machine.h
    #inc/FlyCam.h
    inc/Frame.h
    inc/game.h
    inc/GLTFLoader.h
    inc/Interpolation.h
    inc/LazyTexture.h
    inc/MenuState.h
    inc/Octree.h
    inc/OctreeNode.h
    inc/Painting.h
    inc/PaintingManager.h
    inc/PaintingMetadataDAO.h
    inc/Plane.h
    inc/Player.h
    inc/PlayState.h
    inc/Pose.h
    inc/quat.h
    inc/Ray.h
    inc/RearrangeBones.h
    inc/resource_manager.h
    inc/shader.h
    inc/shader_loader.h
    inc/SimpleMesh.h
    inc/Skeleton.h
    inc/Sky.h
    inc/state.h
    inc/StaticMesh.h
    inc/texture.h
    inc/texture_loader.h
    inc/Track.h
    inc/TrainManager.h
    inc/Transform.h
    inc/TransformTrack.h
    inc/Triangle.h
    inc/TriangleAABB.h
    inc/Utility.h
    inc/Water.h
    inc/window.h
    inc/World.h)

set(project_sources
    src/AABB.cpp
    src/AnimatedMesh.cpp
    #src/AudioEngine.cpp
    #src/AudioSource.cpp
    src/Blending.cpp
    #src/camera.cpp
    src/Camera3.cpp
    src/Clip.cpp
    src/CrossFadeControllerMultiple.cpp
    src/CrossFadeTarget.cpp
    src/finite_state_machine.cpp
    src/game.cpp
    src/GLTFLoader.cpp
    src/LazyTexture.cpp
    src/main.cpp
    src/MenuState.cpp
    src/Octree.cpp
    src/OctreeNode.cpp
    src/Painting.cpp
    src/PaintingManager.cpp
    src/PaintingMetadataDAO.cpp
    src/Plane.cpp
    src/Player.cpp
    src/PlayState.cpp
    src/Pose.cpp
    src/quat.cpp
    src/Ray.cpp
    src/RearrangeBones.cpp
    src/shader.cpp
    src/SimpleMesh.cpp
    src/shader_loader.cpp
    src/Skeleton.cpp
    src/Sky.cpp
    src/StaticMesh.cpp
    src/texture.cpp
    src/texture_loader.cpp
    src/Track.cpp
    src/TrainManager.cpp
    src/Transform.cpp
    src/TransformTrack.cpp
    src/Triangle.cpp
    src/TriangleAABB.cpp
    src/Utility.cpp
    src/Water.cpp
    src/window.cpp
    src/World.cpp
    dependencies/draco/draco/attributes/attribute_octahedron_transform.cc
    dependencies/draco/draco/attributes/attribute_quantization_transform.cc
    dependencies/draco/draco/attributes/attribute_transform.cc
    dependencies/draco/draco/attributes/geometry_attribute.cc
    dependencies/draco/draco/attributes/point_attribute.cc
    dependencies/draco/draco/compression/attributes/attributes_decoder.cc
    dependencies/draco/draco/compression/attributes/kd_tree_attributes_decoder.cc
    dependencies/draco/draco/compression/attributes/sequential_attribute_decoder.cc
    dependencies/draco/draco/compression/attributes/sequential_attribute_decoders_controller.cc
    dependencies/draco/draco/compression/attributes/sequential_integer_attribute_decoder.cc
    dependencies/draco/draco/compression/attributes/sequential_normal_attribute_decoder.cc
    dependencies/draco/draco/compression/attributes/sequential_quantization_attribute_decoder.cc
    dependencies/draco/draco/compression/bit_coders/adaptive_rans_bit_decoder.cc
    dependencies/draco/draco/compression/bit_coders/direct_bit_decoder.cc
    dependencies/draco/draco/compression/bit_coders/rans_bit_decoder.cc
    dependencies/draco/draco/compression/bit_coders/symbol_bit_decoder.cc
    dependencies/draco/draco/compression/decode.cc
    dependencies/draco/draco/compression/entropy/shannon_entropy.cc
    dependencies/draco/draco/compression/entropy/symbol_decoding.cc
    dependencies/draco/draco/compression/mesh/mesh_decoder.cc
    dependencies/draco/draco/compression/mesh/mesh_edgebreaker_decoder.cc
    dependencies/draco/draco/compression/mesh/mesh_edgebreaker_decoder_impl.cc
    dependencies/draco/draco/compression/mesh/mesh_sequential_decoder.cc
    dependencies/draco/draco/compression/point_cloud/algorithms/dynamic_integer_points_kd_tree_decoder.cc
    dependencies/draco/draco/compression/point_cloud/algorithms/float_points_tree_decoder.cc
    dependencies/draco/draco/compression/point_cloud/algorithms/integer_points_kd_tree_decoder.cc
    dependencies/draco/draco/compression/point_cloud/point_cloud_decoder.cc
    dependencies/draco/draco/compression/point_cloud/point_cloud_kd_tree_decoder.cc
    dependencies/draco/draco/compression/point_cloud/point_cloud_sequential_decoder.cc
    dependencies/draco/draco/core/bit_utils.cc
    dependencies/draco/draco/core/bounding_box.cc
    dependencies/draco/draco/core/cycle_timer.cc
    dependencies/draco/draco/core/data_buffer.cc
    dependencies/draco/draco/core/decoder_buffer.cc
    dependencies/draco/draco/core/divide.cc
    dependencies/draco/draco/core/draco_types.cc
    dependencies/draco/draco/core/hash_utils.cc
    dependencies/draco/draco/core/options.cc
    dependencies/draco/draco/core/quantization_utils.cc
    dependencies/draco/draco/mesh/corner_table.cc
    dependencies/draco/draco/mesh/mesh.cc
    dependencies/draco/draco/mesh/mesh_are_equivalent.cc
    dependencies/draco/draco/mesh/mesh_attribute_corner_table.cc
    dependencies/draco/draco/mesh/mesh_cleanup.cc
    dependencies/draco/draco/mesh/mesh_misc_functions.cc
    dependencies/draco/draco/mesh/mesh_splitter.cc
    dependencies/draco/draco/mesh/mesh_stripifier.cc
    dependencies/draco/draco/mesh/mesh_utils.cc
    dependencies/draco/draco/mesh/triangle_soup_mesh_builder.cc
    dependencies/draco/draco/metadata/geometry_metadata.cc
    dependencies/draco/draco/metadata/metadata.cc
    dependencies/draco/draco/metadata/metadata_decoder.cc
    dependencies/draco/draco/point_cloud/point_cloud.cc
    dependencies/draco/draco/point_cloud/point_cloud_builder.cc
    dependencies/win/src/cgltf/cgltf.c
    dependencies/win/src/dr_libs/dr_wav.c
    #dependencies/win/src/imgui/imgui.cpp
    #dependencies/win/src/imgui/imgui_demo.cpp
    #dependencies/win/src/imgui/imgui_draw.cpp
    #dependencies/win/src/imgui/imgui_impl_glfw.cpp
    #dependencies/win/src/imgui/imgui_impl_opengl3.cpp
    #dependencies/win/src/imgui/imgui_tables.cpp
    #dependencies/win/src/imgui/imgui_widgets.cpp
    dependencies/win/src/stb_image/stb_image.cpp)

# This path must be relative to the location of the build folder
set(project_resources "../engine/resources@resources")

set(CMAKE_EXECUTABLE_SUFFIX ".html")

# For debugging
#set(CMAKE_CXX_FLAGS "-O0 -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_GLFW=3 -s WASM=1 -s ASSERTIONS=1 -s ALLOW_MEMORY_GROWTH=1 -o index.html --preload-file ${project_resources} --use-preload-plugins -pthread")
# For releasing
# Setting PTHREAD_POOL_SIZE_STRICT=0 means threads will be created on demand. This stopped the error
# message that says "thread pool exhausted" from showing up in the console.
# TODO: Remember to remove ASSERTIONS before shipping the final build
set(CMAKE_CXX_FLAGS "-O3 -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_GLFW=3 -s WASM=1 -s ASSERTIONS=1 -s ALLOW_MEMORY_GROWTH=1 -o index.html --preload-file ${project_resources} --use-preload-plugins")

# Uncomment this line and the imgui .cpp files above to enable IMGUI
#add_definitions(-DENABLE_IMGUI)

# Uncomment this line and the AudioEngine.cpp, AudioSource.cpp, AudioEngine.h and AudioSource.h files above to enable IMGUI
#add_definitions(-DENABLE_AUDIO)

# Uncomment this line and the FlyCam.h file above to enable the fly cam, which is great for recording videos for marketing purposes
#add_definitions(-DENABLE_FLY_CAM)

add_executable(${PROJECT_NAME} ${project_headers} ${project_sources})
