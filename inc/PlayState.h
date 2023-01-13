#ifndef MOVEMENT_STATE_H
#define MOVEMENT_STATE_H

#include "State.h"
#include "FiniteStateMachine.h"
#include "Window.h"
#ifdef ENABLE_AUDIO
#include "AudioEngine.h"
#endif
#include "Texture.h"
#include "AnimatedMesh.h"
#include "Clip.h"
#include "StaticMesh.h"
#include "World.h"
#include "Player.h"
#include "Sky.h"

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

   void setCameraFree(bool free);
#ifdef __EMSCRIPTEN__
   void setupCameraForTouchControl(bool touchControlsEnabled);
#endif

private:

   void loadCharacters(const std::shared_ptr<Texture>& rainbowUVCheckerMap);
   void loadLevel(const std::shared_ptr<Texture>& rainbowUVCheckerMap);
   void configureLights(const std::shared_ptr<Shader>& shader);

#ifdef ENABLE_IMGUI
   void userInterface();
#endif

   void renderLevel();
   void renderPlayer();

   std::shared_ptr<FiniteStateMachine>          mFSM;

   std::shared_ptr<Window>                      mWindow;

#ifdef ENABLE_AUDIO
   std::shared_ptr<AudioEngine>                 mAudioEngine;
#endif

   std::shared_ptr<Shader>                      mAnimatedMeshShader;
   std::shared_ptr<Shader>                      mStaticMeshWithoutNormalsShader;
   std::shared_ptr<Shader>                      mStaticMeshWithNormalsShader;

   // Character data
   std::vector<std::shared_ptr<Texture>>        mCharacterTextures;
   std::vector<Skeleton>                        mCharacterBaseSkeletons;
   std::vector<std::vector<AnimatedMesh>>       mCharacterMeshes;
   std::vector<std::map<std::string, FastClip>> mCharacterClips;
   std::vector<float>                           mCharacterJumpPlaybackSpeeds;

   // Level data
   std::shared_ptr<Texture>                     mLevelTexture;
   std::vector<StaticMesh>                      mLevelMeshes;
   std::vector<SimpleMesh>                      mLevelCollisionGeometry;
   std::unique_ptr<World>                       mWorld;

   Player                                       mPlayer;

   Sky                                          mSky;

   float                                        mPlaybackSpeed = 1.0f;
};

#endif
