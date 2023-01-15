#ifndef MOVEMENT_STATE_H
#define MOVEMENT_STATE_H

#include "State.h"
#include "FiniteStateMachine.h"
#include "Window.h"
#ifdef ENABLE_AUDIO
#include "AudioEngine.h"
#endif
#include "Camera3.h"
#include "Sky.h"
#include "WebAlembicViewer.h"
#include "AlembicMesh.h"
#include "StaticMesh.h"
#include "Texture.h"

class PlayState : public State
{
public:

#ifdef ENABLE_AUDIO
   PlayState(const std::shared_ptr<FiniteStateMachine>& finiteStateMachine,
             const std::shared_ptr<Window>& window,
             const std::shared_ptr<AudioEngine>& audioEngine);
#else
   PlayState(const std::shared_ptr<FiniteStateMachine>& finiteStateMachine,
             const std::shared_ptr<Window>& window);
#endif
   ~PlayState() = default;

   PlayState(const PlayState&) = delete;
   PlayState& operator=(const PlayState&) = delete;

   PlayState(PlayState&&) = delete;
   PlayState& operator=(PlayState&&) = delete;

   void enter() override;
   void processInput() override;
   void update(float deltaTime) override;
   void render() override;
   void exit() override;

#ifdef __EMSCRIPTEN__
   void setupCameraForTouchControl(bool touchControlsEnabled);
#endif

private:

   void configureLights(const std::shared_ptr<Shader>& shader);

   void loadHands();
   void loadGeisha();
   void loadSamurai();

#ifdef ENABLE_IMGUI
   void userInterface();
#endif

   void renderHands();
   void renderGeisha();
   void renderSamurai();

   void resetCamera();

   std::shared_ptr<FiniteStateMachine>          mFSM;

   std::shared_ptr<Window>                      mWindow;

#ifdef ENABLE_AUDIO
   std::shared_ptr<AudioEngine>                 mAudioEngine;
#endif

   Camera3                                      mCamera3;

   std::shared_ptr<Shader>                      mStaticMeshWithoutNormalsShader;
   std::shared_ptr<Shader>                      mStaticMeshWithNormalsShader;
   std::shared_ptr<Shader>                      mBlinnPhongShader;

   Sky                                          mSky;

   float                                        mPlaybackSpeed = 1.0f;

   wabc::IScenePtr                              mScene;

   AlembicMesh                                  mAlembicMesh;
   float                                        mAlembicAnimationDuration;
   float                                        mAlembicAnimationPlaybackTime = 0.0f;

   std::vector<StaticMesh>                      mGeishaMeshes;
   std::shared_ptr<Texture>                     mGeishaFaceTexture;
   std::shared_ptr<Texture>                     mGeishaEyesTexture;

   std::vector<StaticMesh>                      mSamuraiMeshes;
};

#endif
