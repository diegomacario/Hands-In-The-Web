cmake_minimum_required(VERSION 3.11.1 FATAL_ERROR)

project(Hands-In-The-Web)

include_directories("${CMAKE_SOURCE_DIR}/inc"
                    "${CMAKE_SOURCE_DIR}/dependencies/alembic"
                    "${CMAKE_SOURCE_DIR}/dependencies/cgltf"
					"${CMAKE_SOURCE_DIR}/dependencies/dr_libs"
                    "${CMAKE_SOURCE_DIR}/dependencies/glad"
                    "${CMAKE_SOURCE_DIR}/dependencies/GLFW"
                    "${CMAKE_SOURCE_DIR}/dependencies/glm"
                    "${CMAKE_SOURCE_DIR}/dependencies/imgui"
                    "${CMAKE_SOURCE_DIR}/dependencies/KHR"
					"${CMAKE_SOURCE_DIR}/dependencies/OpenEXR"
                    "${CMAKE_SOURCE_DIR}/dependencies/stb_image")

file(GLOB emscripten_libs ${CMAKE_SOURCE_DIR}/dependencies/emscripten_lib/*.a)

set(project_headers
    inc/AlembicMesh.h
    #inc/AudioEngine.h
    #inc/AudioSource.h
    inc/Camera3.h
    inc/FiniteStateMachine.h
    inc/Frame.h
    inc/Game.h
    inc/GLTFLoader.h
    inc/Interpolation.h
	inc/pch.h
    inc/Plane.h
    inc/PlayState.h
    inc/Quat.h
    inc/Ray.h
    inc/ResourceManager.h
	inc/SceneGraph.h
	inc/sfbxMeta.h
	inc/sfbxRawVector.h
	inc/sfbxTypes.h
    inc/Shader.h
    inc/ShaderLoader.h
    inc/SimpleMesh.h
    inc/Sky.h
    inc/State.h
    inc/StaticMesh.h
    inc/Texture.h
    inc/textureLoader.h
    inc/Track.h
    inc/Transform.h
    inc/TransformTrack.h
    inc/Triangle.h
    inc/Utility.h
	inc/VectorMath.h
    inc/WebAlembicViewer.h
    inc/Window.h)

set(project_sources
    src/AlembicMesh.cpp
    #src/AudioEngine.cpp
    #src/AudioSource.cpp
    src/Camera3.cpp
    src/FiniteStateMachine.cpp
    src/Game.cpp
    src/GLTFLoader.cpp
    src/main.cpp
    src/pch.cpp
    src/Plane.cpp
    src/PlayState.cpp
    src/Quat.cpp
    src/Ray.cpp
	src/SceneABC.cpp
    src/SceneGraph.cpp
    src/Shader.cpp
    src/ShaderLoader.cpp
    src/SimpleMesh.cpp
    src/Sky.cpp
    src/StaticMesh.cpp
    src/Texture.cpp
    src/TextureLoader.cpp
    src/Track.cpp
    src/Transform.cpp
    src/TransformTrack.cpp
    src/Triangle.cpp
    src/Utility.cpp
    src/Window.cpp
    dependencies/cgltf/cgltf/cgltf.c
    dependencies/dr_libs/dr_libs/dr_wav.c
    #dependencies/imgui/imgui/imgui.cpp
    #dependencies/imgui/imgui/imgui_demo.cpp
    #dependencies/imgui/imgui/imgui_draw.cpp
    #dependencies/imgui/imgui/imgui_impl_glfw.cpp
    #dependencies/imgui/imgui/imgui_impl_opengl3.cpp
    #dependencies/imgui/imgui/imgui_tables.cpp
    #dependencies/imgui/imgui/imgui_widgets.cpp
    dependencies/stb_image/stb_image/stb_image.cpp)

# This path must be relative to the location of the build folder
set(project_resources "../resources@resources")

set(CMAKE_EXECUTABLE_SUFFIX ".html")

# For debugging
#set(CMAKE_CXX_FLAGS "-fvisibility=hidden -std=c++17 -O3 -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_GLFW=3 -s WASM=1 -s ASSERTIONS=1 -s ALLOW_MEMORY_GROWTH=1 -o index.html --preload-file ${project_resources} --use-preload-plugins")
# For releasing
set(CMAKE_CXX_FLAGS "-fvisibility=hidden -std=c++17 -O3 -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_GLFW=3 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -o index.html --preload-file ${project_resources} --use-preload-plugins")

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --bind")

# Uncomment this line and the imgui .cpp files above to enable IMGUI
#add_definitions(-DENABLE_IMGUI)

# Uncomment this line and the AudioEngine.cpp, AudioSource.cpp, AudioEngine.h and AudioSource.h files above to enable audio
#add_definitions(-DENABLE_AUDIO)

add_executable(${PROJECT_NAME} ${project_headers} ${project_sources})

target_link_libraries(${PROJECT_NAME} PRIVATE ${emscripten_libs})
