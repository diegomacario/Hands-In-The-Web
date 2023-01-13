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
#include "TextureLoader.h"
#include "GLTFLoader.h"
#include "RearrangeBones.h"

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
   , mCamera3(4.5f, 45.0f, glm::vec3(0.0f), Q::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 10.0f, -90.0f, 90.0f, 45.0f, 1280.0f / 720.0f, 0.1f, 130.0f, 0.25f)
{
   // Initialize the static mesh without normals shader
   mStaticMeshWithoutNormalsShader = ResourceManager<Shader>().loadUnmanagedResource<ShaderLoader>("resources/shaders/static_mesh_without_normals.vert",
                                                                                                   "resources/shaders/baked_illumination.frag");

   // Initialize the static mesh with normals shader
   mStaticMeshWithNormalsShader = ResourceManager<Shader>().loadUnmanagedResource<ShaderLoader>("resources/shaders/static_mesh_with_normals.vert",
                                                                                                "resources/shaders/diffuse_illumination.frag");
   configureLights(mStaticMeshWithNormalsShader);

   // Initialize the hands shader
   mHandsShader = ResourceManager<Shader>().loadUnmanagedResource<ShaderLoader>("resources/shaders/hands.vert",
                                                                                "resources/shaders/hands.frag");

   // Set the listener data
#ifdef ENABLE_AUDIO
   const Q::quat& cameraOrientation = mCamera3.getOrientation();
   glm::vec3      viewVector        = cameraOrientation * glm::vec3(0.0f, 0.0f, -1.0f);
   glm::vec3      cameraUp          = cameraOrientation * glm::vec3(0.0f, 1.0f, 0.0f);
   mAudioEngine->setListenerData(mCamera3.getPosition(), glm::vec3(0.0f, 0.0f, 0.0f), viewVector, cameraUp);

   // Load sounds
   //mAudioEngine->loadSound("resources/sounds/....wav");
#endif

   mScene = wabc::LoadScene("resources/animations/handy.abc");
   if (mScene)
   {
      std::cout << "Alembic scene loaded successfully!" << '\n';
   }
   else
   {
      std::cout << "Failed to load Alembic scene" << '\n';
   }

   mScene->seek(0.0);
   wabc::IMesh* mesh = mScene->getMesh();
   mAlembicMesh.InitializeBuffers(mesh);

   int positionsAttribLoc = mHandsShader->getAttributeLocation("position");
   int normalsAttribLoc   = mHandsShader->getAttributeLocation("normal");
   mAlembicMesh.ConfigureVAO(positionsAttribLoc, normalsAttribLoc);

   std::tuple<double, double> timeRange = mScene->getTimeRange();
   mAlembicAnimationDuration = static_cast<float>(std::get<1>(timeRange));
}

void PlayState::enter()
{
#ifdef __EMSCRIPTEN__
   mPlayer.setModelID(myPlayerModelID);
#endif
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

   // Adjust the distance between the player and the camera
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
   mAlembicAnimationPlaybackTime += deltaTime;
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

   // Remove translation from the view matrix before rendering the skybox
   mSky.Render(mCamera3.getPerspectiveProjectionMatrix() * glm::mat4(glm::mat3(mCamera3.getViewMatrix())));

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

   shader->setUniformVec3( "pointLights[0].worldPos",  glm::vec3(0.0f, 0.0f, 0.0f));
   shader->setUniformVec3( "pointLights[0].color",     glm::vec3(1.0f));
   shader->setUniformFloat("pointLights[0].linearAtt", 0.00115f);

   shader->setUniformInt("numPointLightsInScene", 1);
   shader->use(false);
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

   mHandsShader->use(true);
   mHandsShader->setUniformMat4("model",      glm::mat4(1.0f));
   mHandsShader->setUniformMat4("view",       mCamera3.getViewMatrix());
   mHandsShader->setUniformMat4("projection", mCamera3.getPerspectiveProjectionMatrix());

   mAlembicMesh.Render();

   mHandsShader->use(false);
}

void PlayState::resetCamera()
{
   mCamera3.reposition(4.5f, 45.0f, glm::vec3(0.0f), Q::quat(), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 10.0f, -90.0f, 90.0f);
   mCamera3.processMouseMovement(180.0f / 0.25f, 0.0f);
}
