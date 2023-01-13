#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <map>
#include <vector>

#include "Window.h"
#include "World.h"
#include "Transform.h"
#include "Skeleton.h"
#include "CrossFadeControllerMultiple.h"
#include "Camera3.h"
#ifdef ENABLE_FLY_CAM
#include "FlyCam.h"
#endif

class Player
{
public:

   Player();

   void                          processInput(const std::shared_ptr<Window>& window);

   void                          move(const std::unique_ptr<World>&    world,
                                      std::map<std::string, FastClip>& clips,
                                      float                            jumpPlaybackSpeed);

   const Transform&              getModelTransform() const { return mModelTransform; }
#ifdef ENABLE_FLY_CAM
   glm::mat4                     getViewMatrix() const;
   glm::mat4                     getPerspectiveProjectionMatrix() const;
#else
   const glm::mat4&              getViewMatrix() const;
   const glm::mat4&              getPerspectiveProjectionMatrix() const;
#endif
   const glm::vec3&              getVelocity() const { return mVelocity; };
   const glm::vec3&              getCameraPosition() const;
   const Q::quat&                getCameraOrientation() const;
   Camera3::CameraMode           getCameraMode() const { return mCamera3.getCameraMode(); }
   void                          setCameraFree(bool free) { mCamera3.setFree(free); }

   unsigned int                  getModelID() const { return mModelID; }
   void                          setModelID(unsigned int id) { mModelID = id; }

   void                          initializeAnimation(const Skeleton& skeleton, FastClip& clip);
   void                          updatePose(float deltaTime);
   const std::vector<glm::mat4>& getSkinMatrices() { return mSkinMatrices; }

private:

   void                          updateOrientation(float xOffset);
   void                          updateMovementFlags(const std::shared_ptr<Window>& window);

   void                          fadeIfNecessary(std::map<std::string, FastClip>& clips,
                                                 float                            jumpPlaybackSpeed,
                                                 bool                             jumped,
                                                 bool                             isFlying);

   void                          collideAndSlide(float deltaTime, const std::unique_ptr<World>& world);
   glm::vec3                     collideWithWorld(const glm::vec3& position, const glm::vec3& translation, const std::unique_ptr<World>& world);

   Transform                       mModelTransform;

   bool                            mIsRunning = false;
   bool                            mFell = false;
   bool                            mJumped = false;

   unsigned int                    mModelID;

   unsigned char                   mMovementFlags = 0;

   Skeleton                        mSkeleton;
   FastCrossFadeControllerMultiple mCrossFadeController;
   std::vector<glm::mat4>          mPosePalette;
   std::vector<glm::mat4>          mSkinMatrices;

   glm::vec3                       mVelocity;

   Camera3                         mCamera3;

#ifdef ENABLE_FLY_CAM
   FlyCam                          mFlyCam;
#endif

   CollisionData                   mCollisionData;
   int                             mCollisionRecursionDepth;
   glm::vec3                       mEllipsoidVerticalOffset;

   float                           mGravity;

   glm::vec3                       mRespawnPoint;
};

#endif
