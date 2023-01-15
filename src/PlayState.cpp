#include "glm/gtx/compatibility.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#ifdef ENABLE_IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#endif

#include "PlayState.h"
#include "ResourceManager.h"
#include "ShaderLoader.h"
#include "GLTFLoader.h"
#include "TextureLoader.h"
#include "Transform.h"

#ifdef ENABLE_AUDIO
PlayState::PlayState(const std::shared_ptr<FiniteStateMachine>& finiteStateMachine,
                     const std::shared_ptr<Window>&             window,
                     const std::shared_ptr<AudioEngine>&        audioEngine)
#else
PlayState::PlayState(const std::shared_ptr<FiniteStateMachine>& finiteStateMachine,
                     const std::shared_ptr<Window>&             window)
#endif
   : mFSM(finiteStateMachine)
   , mWindow(window)
#ifdef ENABLE_AUDIO
   , mAudioEngine(audioEngine)
#endif
   , mCamera3(1.0f, 15.0f, glm::vec3(0.0f, 1.25f, 0.0), Q::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::vec3(0.0f, 0.0f, 0.0f), 0.5f, 10.0f, -90.0f, 90.0f, 45.0f, 1280.0f / 720.0f, 0.1f, 130.0f, 0.25f)
{
   // Initialize the static mesh without normals shader
   mStaticMeshWithoutNormalsShader = ResourceManager<Shader>().loadUnmanagedResource<ShaderLoader>("resources/shaders/static_mesh_without_normals.vert",
                                                                                                   "resources/shaders/baked_illumination.frag");

   // Initialize the static mesh with normals shader
   mStaticMeshWithNormalsShader = ResourceManager<Shader>().loadUnmanagedResource<ShaderLoader>("resources/shaders/static_mesh_with_normals.vert",
                                                                                                "resources/shaders/diffuse_illumination.frag");
   configureLights(mStaticMeshWithNormalsShader);

   // Initialize the hands shader
   //mHandsShader = ResourceManager<Shader>().loadUnmanagedResource<ShaderLoader>("resources/shaders/gourad.vert",
   //                                                                             "resources/shaders/gourad.frag");
   mBlinnPhongShader = ResourceManager<Shader>().loadUnmanagedResource<ShaderLoader>("resources/shaders/blinn_phong.vert",
                                                                                     "resources/shaders/blinn_phong.frag");
   configureLights(mBlinnPhongShader);

   // Set the listener data
#ifdef ENABLE_AUDIO
   const Q::quat& cameraOrientation = mCamera3.getOrientation();
   glm::vec3      viewVector        = cameraOrientation * glm::vec3(0.0f, 0.0f, -1.0f);
   glm::vec3      cameraUp          = cameraOrientation * glm::vec3(0.0f, 1.0f, 0.0f);
   mAudioEngine->setListenerData(mCamera3.getPosition(), glm::vec3(0.0f, 0.0f, 0.0f), viewVector, cameraUp);

   // Load sounds
   //mAudioEngine->loadSound("resources/sounds/....wav");
#endif

   loadHands();
   loadGeisha();
   loadSamurai();
}

void PlayState::enter()
{

}

void PlayState::processInput()
{
#ifndef __EMSCRIPTEN__
   // Close the game
   if (mWindow->keyIsPressed(GLFW_KEY_ESCAPE))
   {
      mWindow->setShouldClose(true);
   }

   // Switch between the full screen mode and the windowed mode
   if (mWindow->keyIsPressed(GLFW_KEY_F) && !mWindow->keyHasBeenProcessed(GLFW_KEY_F))
   {
      mWindow->setKeyAsProcessed(GLFW_KEY_F);
      mWindow->setFullScreen(!mWindow->isFullScreen());
      // TODO: Not sure if this is necessary or not
      // Going from windowed to fullscreen changes the position of the cursor, so we reset the first move flag to avoid a jump
      mWindow->resetFirstMove();
   }
#endif

   // Reset the camera
   if (mWindow->keyIsPressed(GLFW_KEY_R)) { resetCamera(); }

   // Orient the camera
   if (mWindow->mouseMoved() && mWindow->isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
   {
      mCamera3.processMouseMovement(mWindow->getCursorXOffset(), mWindow->getCursorYOffset());
      mWindow->resetMouseMoved();
   }

   // Adjust the distance between the target and the camera
   if (mWindow->scrollWheelMoved())
   {
      mCamera3.processScrollWheelMovement(mWindow->getScrollYOffset());
      mWindow->resetScrollWheelMoved();
   }
}

void PlayState::update(float deltaTime)
{
   // Update the audio engine
   const Q::quat& cameraOrientation = mCamera3.getOrientation();
   glm::vec3      viewVector        = cameraOrientation * glm::vec3(0.0f, 0.0f, -1.0f);
#ifdef ENABLE_AUDIO
   glm::vec3      cameraUp          = cameraOrientation * glm::vec3(0.0f, 1.0f, 0.0f);
   mAudioEngine->setListenerData(mCamera3.getPosition(), glm::vec3(0.0f, 0.0f, 0.0f), viewVector, cameraUp);
   mAudioEngine->update();
#endif

   // Update the hands
   mAlembicAnimationPlaybackTime += deltaTime * mPlaybackSpeed;
   if (mAlembicAnimationPlaybackTime > mAlembicAnimationDuration)
   {
      mAlembicAnimationPlaybackTime -= mAlembicAnimationDuration;
   }
}

void PlayState::render()
{
#ifdef ENABLE_IMGUI
   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplGlfw_NewFrame();
   ImGui::NewFrame();

   userInterface();
#endif

#ifndef __EMSCRIPTEN__
   mWindow->bindMultisampleFramebuffer();
#endif

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Enable depth testing for 3D objects
   glEnable(GL_DEPTH_TEST);

   renderHands();
   renderGeisha();
   //renderSamurai();

#ifdef ENABLE_IMGUI
   ImGui::Render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

#ifndef __EMSCRIPTEN__
   mWindow->generateAntiAliasedImage();
#endif

   mWindow->swapBuffers();
   mWindow->pollEvents();
}

void PlayState::exit()
{

}

#ifdef __EMSCRIPTEN__
void PlayState::setupCameraForTouchControl(bool touchControlsEnabled)
{
   if (touchControlsEnabled)
   {
      mWindow->setTouchControlsEnabled(true);
      mWindow->enableCursor(true);
   }
}
#endif

void PlayState::configureLights(const std::shared_ptr<Shader>& shader)
{
   shader->use(true);

   shader->setUniformVec3( "pointLights[0].worldPos",  glm::vec3(0.0f, 1.25f, 1.0f));
   shader->setUniformVec3( "pointLights[0].color",     glm::vec3(1.0f));
   shader->setUniformFloat("pointLights[0].linearAtt", 0.00115f);

   shader->setUniformInt("numPointLightsInScene", 1);
   shader->use(false);
}

void PlayState::loadHands()
{
   mScene = wabc::LoadScene("resources/animations/handy.abc");

   mScene->seek(0.0);
   wabc::IMesh* mesh = mScene->getMesh();
   mAlembicMesh.InitializeBuffers(mesh);

   int positionsAttribLoc = mBlinnPhongShader->getAttributeLocation("position");
   int normalsAttribLoc   = mBlinnPhongShader->getAttributeLocation("normal");
   mAlembicMesh.ConfigureVAO(positionsAttribLoc, normalsAttribLoc);

   std::tuple<double, double> timeRange = mScene->getTimeRange();
   mAlembicAnimationDuration = static_cast<float>(std::get<1>(timeRange));
}

void PlayState::loadGeisha()
{
   cgltf_data* data = LoadGLTFFile("resources/models/geisha/geisha.glb");
   mGeishaMeshes = LoadStaticMeshes(data);
   FreeGLTFFile(data);

   int positionsAttribLoc = mStaticMeshWithNormalsShader->getAttributeLocation("position");
   int normalsAttribLoc   = mStaticMeshWithNormalsShader->getAttributeLocation("normal");
   int texCoordsAttribLoc = mStaticMeshWithNormalsShader->getAttributeLocation("texCoord");

   for (unsigned int i = 0,
        size = static_cast<unsigned int>(mGeishaMeshes.size());
        i < size;
        ++i)
   {
      mGeishaMeshes[i].ConfigureVAO(positionsAttribLoc,
                                    normalsAttribLoc,
                                    texCoordsAttribLoc);
   }

   mGeishaFaceTexture = ResourceManager<Texture>().loadUnmanagedResource<TextureLoader>("resources/models/geisha/face.jpeg", nullptr, nullptr, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true, true);
   mGeishaEyesTexture = ResourceManager<Texture>().loadUnmanagedResource<TextureLoader>("resources/models/geisha/eyes.png", nullptr, nullptr, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true, true);
}

void PlayState::loadSamurai()
{
   cgltf_data* data = LoadGLTFFile("resources/models/samurai/samurai.glb");
   mSamuraiMeshes = LoadStaticMeshes(data);
   FreeGLTFFile(data);

   int positionsAttribLoc = mBlinnPhongShader->getAttributeLocation("position");
   int normalsAttribLoc   = mBlinnPhongShader->getAttributeLocation("normal");

   for (unsigned int i = 0,
        size = static_cast<unsigned int>(mSamuraiMeshes.size());
        i < size;
        ++i)
   {
      mSamuraiMeshes[i].ConfigureVAO(positionsAttribLoc,
                                     normalsAttribLoc,
                                     -1);
   }
}

#ifdef ENABLE_IMGUI
void PlayState::userInterface()
{
   ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Appearing);

   char title[64];
   snprintf(title, 32, "Hands In The Web (%.1f FPS)###HandsInTheWeb", ImGui::GetIO().Framerate);
   ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

   ImGui::SliderFloat("Playback speed", &mPlaybackSpeed, 0.0f, 1.0f, "%.3f");

   ImGui::End();
}
#endif

void PlayState::renderHands()
{
   mScene->seek(mAlembicAnimationPlaybackTime);
   wabc::IMesh* mesh = mScene->getMesh();
   mAlembicMesh.UpdateBuffers(mesh);

   mBlinnPhongShader->use(true);
   mBlinnPhongShader->setUniformMat4("model", glm::mat4(1.0f));
   mBlinnPhongShader->setUniformMat4("view", mCamera3.getViewMatrix());
   mBlinnPhongShader->setUniformMat4("projection", mCamera3.getPerspectiveProjectionMatrix());
   mBlinnPhongShader->setUniformVec3("cameraPos",    mCamera3.getPosition());
   // Grey
   //mBlinnPhongShader->setUniformVec3("diffuseColor", Utility::hexToColor(0xd9d2d7));
   // White
   //mBlinnPhongShader->setUniformVec3("diffuseColor", Utility::hexToColor(0xf2eeef));
   // Pink
   //mBlinnPhongShader->setUniformVec3("diffuseColor", Utility::hexToColor(0xd99aa3));
   // Red
   mBlinnPhongShader->setUniformVec3("diffuseColor", Utility::hexToColor(0xaf3d4d));

   glFrontFace(GL_CW);
   mAlembicMesh.Render();
   glFrontFace(GL_CCW);

   mBlinnPhongShader->use(false);
}

void PlayState::renderGeisha()
{
   wabc::span<wabc::ICamera*> cameras = mScene->getCameras();
   wabc::float3 cameraPosition        = cameras[0]->getPosition();
   wabc::float3 cameraDirection       = cameras[0]->getDirection();
   wabc::float3 cameraUp              = cameras[0]->getUp();

   Transform modelTransform(glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z),
                            Q::lookRotation(glm::vec3(cameraDirection.x, cameraDirection.y, cameraDirection.z),
                            glm::vec3(cameraUp.x, cameraUp.y, cameraUp.z)), glm::vec3(0.1f));

   mStaticMeshWithNormalsShader->use(true);
   mStaticMeshWithNormalsShader->setUniformMat4("model", transformToMat4(modelTransform));
   mStaticMeshWithNormalsShader->setUniformMat4("view", mCamera3.getViewMatrix());
   mStaticMeshWithNormalsShader->setUniformMat4("projection", mCamera3.getPerspectiveProjectionMatrix());
   //mStaticMeshWithNormalsShader->setUniformVec3("cameraPos", mCamera3.getPosition());

   mGeishaFaceTexture->bind(0, mStaticMeshWithNormalsShader->getUniformLocation("diffuseTex"));
   mGeishaMeshes[1].Render();
   mGeishaFaceTexture->unbind(0);

   mGeishaEyesTexture->bind(0, mStaticMeshWithNormalsShader->getUniformLocation("diffuseTex"));
   mGeishaMeshes[0].Render();
   mGeishaMeshes[2].Render();
   mGeishaEyesTexture->unbind(0);

   mStaticMeshWithNormalsShader->use(false);
}

void PlayState::renderSamurai()
{
   wabc::span<wabc::ICamera*> cameras = mScene->getCameras();
   wabc::float3 cameraPosition        = cameras[0]->getPosition();
   wabc::float3 cameraDirection       = cameras[0]->getDirection();
   wabc::float3 cameraUp              = cameras[0]->getUp();

   Transform modelTransform(glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z),
                            Q::lookRotation(glm::vec3(cameraDirection.x, cameraDirection.y, cameraDirection.z),
                            glm::vec3(cameraUp.x, cameraUp.y, cameraUp.z)), glm::vec3(0.001f));

   mBlinnPhongShader->use(true);
   mBlinnPhongShader->setUniformMat4("model", transformToMat4(modelTransform));
   mBlinnPhongShader->setUniformMat4("view", mCamera3.getViewMatrix());
   mBlinnPhongShader->setUniformMat4("projection", mCamera3.getPerspectiveProjectionMatrix());
   mBlinnPhongShader->setUniformVec3("cameraPos", mCamera3.getPosition());
   // Red
   mBlinnPhongShader->setUniformVec3("diffuseColor", Utility::hexToColor(0xaf3d4d));

   for (unsigned int i = 0,
        size = static_cast<unsigned int>(mSamuraiMeshes.size());
        i < size;
        ++i)
   {
      mSamuraiMeshes[i].Render();
   }

   mBlinnPhongShader->use(false);
}

void PlayState::resetCamera()
{
   mCamera3.reposition(1.0f, 15.0f, glm::vec3(0.0f, 1.25f, 0.0f), Q::quat(), glm::vec3(0.0f, 0.0f, 0.0f), 0.5f, 10.0f, -90.0f, 90.0f);
   mCamera3.processMouseMovement(180.0f / 0.25f, 0.0f);
}
