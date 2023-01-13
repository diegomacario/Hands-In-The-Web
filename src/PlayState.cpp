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
   , mPlayer()
{
   // Initialize the animated mesh shader
   mAnimatedMeshShader = ResourceManager<Shader>().loadUnmanagedResource<ShaderLoader>("resources/shaders/animated_mesh_with_pregenerated_skin_matrices.vert",
                                                                                       "resources/shaders/diffuse_illumination.frag");
   configureLights(mAnimatedMeshShader);

   // Initialize the static mesh without normals shader
   mStaticMeshWithoutNormalsShader = ResourceManager<Shader>().loadUnmanagedResource<ShaderLoader>("resources/shaders/static_mesh_without_normals.vert",
                                                                                                   "resources/shaders/baked_illumination.frag");

   // Initialize the static mesh with normals shader
   mStaticMeshWithNormalsShader = ResourceManager<Shader>().loadUnmanagedResource<ShaderLoader>("resources/shaders/static_mesh_with_normals.vert",
                                                                                                "resources/shaders/diffuse_illumination.frag");
   configureLights(mStaticMeshWithNormalsShader);

   loadCharacters();
   loadLevel();

   mWorld = std::make_unique<World>(mLevelCollisionGeometry);

   // Set the listener data
#ifdef ENABLE_AUDIO
   const Q::quat& cameraOrientation = mPlayer.getCameraOrientation();
   glm::vec3      viewVector        = cameraOrientation * glm::vec3(0.0f, 0.0f, -1.0f);
   glm::vec3      cameraUp          = cameraOrientation * glm::vec3(0.0f, 1.0f, 0.0f);
   mAudioEngine->setListenerData(mPlayer.getCameraPosition(), mPlayer.getVelocity(), viewVector, cameraUp);

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
   auto mesh = mScene->getMesh();
   std::cout << "Vertex count: " << static_cast<int>(mesh->getPoints().size()) << '\n';
}

void PlayState::enter()
{
#ifdef __EMSCRIPTEN__
   mPlayer.setModelID(myPlayerModelID);
#endif

   // Initialize the animation data of the player
   mPlayer.initializeAnimation(mCharacterBaseSkeletons[mPlayer.getModelID()],
                               mCharacterClips[mPlayer.getModelID()]["Idle"]);
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

   mPlayer.processInput(mWindow);
}

void PlayState::update(float deltaTime)
{
   mPlayer.move(mWorld, mCharacterClips[mPlayer.getModelID()], mCharacterJumpPlaybackSpeeds[mPlayer.getModelID()]);

   mPlayer.updatePose(deltaTime);

   // Update the audio engine
   const Q::quat& cameraOrientation = mPlayer.getCameraOrientation();
   glm::vec3      viewVector        = cameraOrientation * glm::vec3(0.0f, 0.0f, -1.0f);
#ifdef ENABLE_AUDIO
   glm::vec3      cameraUp          = cameraOrientation * glm::vec3(0.0f, 1.0f, 0.0f);
   mAudioEngine->setListenerData(mPlayer.getCameraPosition(), mPlayer.getVelocity(), viewVector, cameraUp);
   mAudioEngine->update();
#endif
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

   renderLevel();

   renderPlayer();

   // Remove translation from the view matrix before rendering the skybox
   mSky.Render(mPlayer.getPerspectiveProjectionMatrix() * glm::mat4(glm::mat3(mPlayer.getViewMatrix())));

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

void PlayState::setCameraFree(bool free)
{
   mPlayer.setCameraFree(free);
#ifdef __EMSCRIPTEN__
   mWindow->setTouchControlsEnabled(false);
#endif
   mWindow->enableCursor(!free);
}

#ifdef __EMSCRIPTEN__
void PlayState::setupCameraForTouchControl(bool touchControlsEnabled)
{
   if (touchControlsEnabled) {
      mPlayer.setCameraFree(true);
      mWindow->setTouchControlsEnabled(true);
      mWindow->enableCursor(true);
   }
}
#endif

void PlayState::loadCharacters()
{
   std::vector<std::string> characterTextureFilePaths { "resources/models/humans/woman.png" };

   std::vector<std::string> characterModelFilePaths { "resources/models/humans/woman.glb" };

   mCharacterJumpPlaybackSpeeds = { 1.1f };

   // Load the textures of the animated characters
   mCharacterTextures.reserve(characterTextureFilePaths.size());
   for (const std::string& characterTextureFilePath : characterTextureFilePaths)
   {
      mCharacterTextures.emplace_back(ResourceManager<Texture>().loadUnmanagedResource<TextureLoader>(characterTextureFilePath));
   }

   // Load the animated characters
   size_t numModels = characterModelFilePaths.size();
   mCharacterBaseSkeletons.reserve(numModels);
   mCharacterMeshes.reserve(numModels);
   mCharacterClips.reserve(numModels);
   unsigned int modelIndex = 0;
   for (const std::string& characterModelFilePath : characterModelFilePaths)
   {
      // Load the animated character
      cgltf_data* data = LoadGLTFFile(characterModelFilePath.c_str());
      mCharacterBaseSkeletons.emplace_back(LoadSkeleton(data));
      mCharacterMeshes.emplace_back(LoadDracoAnimatedMeshes(data));
      std::vector<Clip> characterClips = LoadClips(data);
      FreeGLTFFile(data);

      // Rearrange the skeleton
      JointMap characterJointMap = RearrangeSkeleton(mCharacterBaseSkeletons[modelIndex]);

      // Rearrange the meshes
      for (unsigned int meshIndex = 0,
           numMeshes = static_cast<unsigned int>(mCharacterMeshes[modelIndex].size());
           meshIndex < numMeshes;
           ++meshIndex)
      {
         RearrangeMesh(mCharacterMeshes[modelIndex][meshIndex], characterJointMap);
         mCharacterMeshes[modelIndex][meshIndex].ClearMeshData();
      }

      // Optimize the clips, rearrange them and store them
      mCharacterClips.emplace_back(std::map<std::string, FastClip>());
      for (unsigned int clipIndex = 0,
           numClips = static_cast<unsigned int>(characterClips.size());
           clipIndex < numClips;
           ++clipIndex)
      {
         FastClip currClip = OptimizeClip(characterClips[clipIndex]);
         RearrangeFastClip(currClip, characterJointMap);
         mCharacterClips[modelIndex].insert(std::make_pair(currClip.GetName(), currClip));
      }

      mCharacterClips[modelIndex]["Jump"].SetLooping(false);

      // Configure the VAOs of the animated meshes
      int positionsAttribLocOfAnimatedShader  = mAnimatedMeshShader->getAttributeLocation("position");
      int normalsAttribLocOfAnimatedShader    = mAnimatedMeshShader->getAttributeLocation("normal");
      int texCoordsAttribLocOfAnimatedShader  = mAnimatedMeshShader->getAttributeLocation("texCoord");
      int weightsAttribLocOfAnimatedShader    = mAnimatedMeshShader->getAttributeLocation("weights");
      int influencesAttribLocOfAnimatedShader = mAnimatedMeshShader->getAttributeLocation("joints");

      for (unsigned int i = 0,
           size = static_cast<unsigned int>(mCharacterMeshes[modelIndex].size());
           i < size;
           ++i)
      {
         mCharacterMeshes[modelIndex][i].ConfigureVAO(positionsAttribLocOfAnimatedShader,
                                                      normalsAttribLocOfAnimatedShader,
                                                      texCoordsAttribLocOfAnimatedShader,
                                                      weightsAttribLocOfAnimatedShader,
                                                      influencesAttribLocOfAnimatedShader);
      }

      ++modelIndex;
   }
}

void PlayState::loadLevel()
{
   // Load the texture of the level
   mLevelTexture = ResourceManager<Texture>().loadUnmanagedResource<TextureLoader>("resources/models/uv/uv_grid.png");

   // Load the meshes of the level
   cgltf_data* data = LoadGLTFFile("resources/models/level/level.gltf");
   mLevelMeshes = LoadStaticMeshes(data);
   FreeGLTFFile(data);

   int positionsAttribLoc = mStaticMeshWithoutNormalsShader->getAttributeLocation("position");
   int texCoordsAttribLoc = mStaticMeshWithoutNormalsShader->getAttributeLocation("texCoord");

   for (unsigned int i = 0,
        size = static_cast<unsigned int>(mLevelMeshes.size());
        i < size;
        ++i)
   {
      mLevelMeshes[i].ConfigureVAO(positionsAttribLoc,
                                   -1,
                                   texCoordsAttribLoc);
   }

   // Load the collision geometry of the level
   data = LoadGLTFFile("resources/models/level/level.gltf");
   mLevelCollisionGeometry = LoadSimpleMeshes(data);
   FreeGLTFFile(data);
}

void PlayState::configureLights(const std::shared_ptr<Shader>& shader)
{
   shader->use(true);

   shader->setUniformVec3( "pointLights[0].worldPos",  glm::vec3(-300.0f, 150.0f, -300.0f));
   shader->setUniformVec3( "pointLights[0].color",     glm::vec3(1.0f));
   shader->setUniformFloat("pointLights[0].linearAtt", 0.00115f);

   shader->setUniformVec3( "pointLights[1].worldPos",  glm::vec3(-300.0f, 150.0f, 300.0f));
   shader->setUniformVec3( "pointLights[1].color",     glm::vec3(1.0f));
   shader->setUniformFloat("pointLights[1].linearAtt", 0.00115f);

   shader->setUniformVec3( "pointLights[2].worldPos",  glm::vec3(300.0f, 150.0f, 300.0f));
   shader->setUniformVec3( "pointLights[2].color",     glm::vec3(1.0f));
   shader->setUniformFloat("pointLights[2].linearAtt", 0.00115f);

   shader->setUniformVec3( "pointLights[3].worldPos",  glm::vec3(300.0f, 150.0f, -300.0f));
   shader->setUniformVec3( "pointLights[3].color",     glm::vec3(1.0f));
   shader->setUniformFloat("pointLights[3].linearAtt", 0.00115f);

   shader->setUniformInt("numPointLightsInScene", 4);
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

void PlayState::renderLevel()
{
   mStaticMeshWithoutNormalsShader->use(true);
   mStaticMeshWithoutNormalsShader->setUniformMat4("model",         glm::mat4(1.0f));
   mStaticMeshWithoutNormalsShader->setUniformMat4("view",          mPlayer.getViewMatrix());
   mStaticMeshWithoutNormalsShader->setUniformMat4("projection",    mPlayer.getPerspectiveProjectionMatrix());

   // Loop over the level meshes and render each one
   mLevelTexture->bind(0, mStaticMeshWithoutNormalsShader->getUniformLocation("diffuseTex"));
   for (unsigned int i = 0,
      size = static_cast<unsigned int>(mLevelMeshes.size());
      i < size;
      ++i)
   {
      mLevelMeshes[i].Render();
   }
   mLevelTexture->unbind(0);

   mStaticMeshWithoutNormalsShader->use(false);
}

void PlayState::renderPlayer()
{
   mAnimatedMeshShader->use(true);
   mAnimatedMeshShader->setUniformMat4("view",          mPlayer.getViewMatrix());
   mAnimatedMeshShader->setUniformMat4("projection",    mPlayer.getPerspectiveProjectionMatrix());

   if (mPlayer.getCameraMode() == Camera3::CameraMode::ThirdPerson)
   {
      unsigned int modelID = mPlayer.getModelID();
      mCharacterTextures[modelID]->bind(0, mAnimatedMeshShader->getUniformLocation("diffuseTex"));
      mAnimatedMeshShader->setUniformMat4Array("animated[0]", mPlayer.getSkinMatrices());
      mAnimatedMeshShader->setUniformMat4("model", transformToMat4(mPlayer.getModelTransform()));

      // Loop over the meshes and render each one
      for (unsigned int i = 0, size = static_cast<unsigned int>(mCharacterMeshes[modelID].size()); i < size; ++i)
      {
         mCharacterMeshes[modelID][i].Render();
      }
      mCharacterTextures[modelID]->unbind(0);
   }

   mAnimatedMeshShader->use(false);
}
