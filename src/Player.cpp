#include "glm/gtx/norm.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include "Player.h"
#include "Plane.h"

Player::Player()
   : mModelID(0)
   // Note how the starting position isn't the origin of the world
   // When that's the case, the rays that we shoot down don't intersect anything
   // It's not clear why this is the case, but to fix this problem we simply move the player a little along the X axis
   // TODO: Determine why this is the case
   , mModelTransform(glm::vec3(0.1f, 0.0f, 0.0f), Q::quat(), glm::vec3(1.0f))
   , mIsRunning(false)
   , mVelocity(0.0f)
   , mCamera3(10.0f, 10.0f, glm::vec3(0.1f, 0.0f, 0.0f), Q::quat(), glm::vec3(0.0f, 3.0f, 0.0f), 10.0f, 30.0f, 0.0f, 89.9f, 45.0f, 1280.0f / 720.0f, 1.0f, 1000.0f, 0.25f)
#ifdef ENABLE_FLY_CAM
   , mFlyCam(glm::vec3(0.0f, 100.0f, 0.0f))
#endif
   , mGravity(-10.0f)
{
   mCollisionData.eRadius = glm::vec3(0.8465f + 0.5f, 2.591f, 0.5765f + 1.0f);
   mEllipsoidVerticalOffset = glm::vec3(0.0f, 2.6f, 0.0f);
   mRespawnPoint = glm::vec3(0.0f, 106.878f, 0.0f);
}

void Player::processInput(const std::shared_ptr<Window>& window)
{
   // Switch between the 1st person mode and the 3rd person mode
   if (window->keyIsPressed(GLFW_KEY_R) && !window->keyHasBeenProcessed(GLFW_KEY_R))
   {
      window->setKeyAsProcessed(GLFW_KEY_R);
      if (mCamera3.getCameraMode() == Camera3::CameraMode::ThirdPerson)
      {
         mCamera3.enableFirstPersonMode();
      } 
      else
      {
         mCamera3.enableThirdPersonMode();
      }
   }

   // Switch between the free mode and the fixed mode
#ifndef __EMSCRIPTEN__
   if (window->keyIsPressed(GLFW_KEY_C) && !window->keyHasBeenProcessed(GLFW_KEY_C))
   {
      window->setKeyAsProcessed(GLFW_KEY_C);
      mCamera3.setFree(!mCamera3.isFree());

      if (mCamera3.isFree())
      {
         // Disable the cursor when camera is free
         window->enableCursor(false);
      }
      else
      {
         // Enable the cursor when camera is fixed
         window->enableCursor(true);
      }
   }
#endif

   // Process inputs from the mouse if the camera is free
   if (mCamera3.isFree())
   {
      if (window->mouseMoved())
      {
         // A change in the yaw affects the orientation model of the player
         updateOrientation(window->getCursorXOffset());

         // A change in the pitch affects the orientation of the camera
         mCamera3.processMouseMovement(window->getCursorYOffset());

         // Give the new orientation of the player to the camera
         mCamera3.setPlayerOrientation(mModelTransform.rotation);

#ifdef ENABLE_FLY_CAM
         mFlyCam.ProcessMouseMovement(window->getCursorXOffset(), window->getCursorYOffset());
#endif

         window->resetMouseMoved();
      }

      if (window->scrollWheelMoved())
      {
         // Adjust the distance between the player and the camera
         mCamera3.processScrollWheelMovement(window->getScrollYOffset());

#ifdef ENABLE_FLY_CAM
         mFlyCam.ProcessMouseScroll(window->getScrollYOffset());
#endif

         window->resetScrollWheelMoved();
      }

#ifdef ENABLE_FLY_CAM
      float deltaTime = 1.0f / 60.0f;
      if (window->keyIsPressed(GLFW_KEY_W))
         mFlyCam.ProcessKeyboard(FlyCam::FlyCamMovement::FORWARD, deltaTime);
      if (window->keyIsPressed(GLFW_KEY_S))
         mFlyCam.ProcessKeyboard(FlyCam::FlyCamMovement::BACKWARD, deltaTime);
      if (window->keyIsPressed(GLFW_KEY_A))
         mFlyCam.ProcessKeyboard(FlyCam::FlyCamMovement::LEFT, deltaTime);
      if (window->keyIsPressed(GLFW_KEY_D))
         mFlyCam.ProcessKeyboard(FlyCam::FlyCamMovement::RIGHT, deltaTime);
#endif
   }

   updateMovementFlags(window);
}

void Player::move(const std::unique_ptr<World>& world, std::map<std::string, FastClip>& clips, float jumpPlaybackSpeed)
{
   glm::vec3 cameraFront = mModelTransform.rotation * glm::vec3(0.0f, 0.0f, 1.0f);
   cameraFront.y         = 0.0f;
   cameraFront           = glm::normalize(cameraFront);
   glm::vec3 cameraRight = mModelTransform.rotation * glm::vec3(-1.0f, 0.0f, 0.0f);

   bool jumped, isFlying;
   Utility::movePlayer(mModelTransform.position, mVelocity, cameraFront, cameraRight, mMovementFlags, world, &jumped, &isFlying);

   // Now that we have the velocity of the player, we can check if they are colliding with anything
   collideAndSlide(1.0f / 60.0f, world);

   fadeIfNecessary(clips, jumpPlaybackSpeed, jumped, isFlying);
}

#ifdef ENABLE_FLY_CAM
glm::mat4 Player::getViewMatrix() const
{
   return mFlyCam.GetViewMatrix();
}

glm::mat4 Player::getPerspectiveProjectionMatrix() const
{
   return mFlyCam.GetPerspectiveProjectionMatrix();
}
#else
const glm::mat4& Player::getViewMatrix() const
{
   return mCamera3.getViewMatrix();
}

const glm::mat4& Player::getPerspectiveProjectionMatrix() const
{
   return mCamera3.getPerspectiveProjectionMatrix();
}
#endif

const glm::vec3& Player::getCameraPosition() const
{
   return mCamera3.getPosition();
}

const Q::quat& Player::getCameraOrientation() const
{
   return mCamera3.getOrientation();
}

void Player::initializeAnimation(const Skeleton& skeleton, FastClip& clip)
{
   mSkeleton = skeleton;

   // Set the initial clip and initialize the crossfade controller
   mCrossFadeController.SetSkeleton(mSkeleton);
   mCrossFadeController.Play(&clip, false);
   mCrossFadeController.Update(0.0f);
   mCrossFadeController.GetCurrentPose().GetMatrixPalette(mPosePalette);

   updatePose(0.0f);
}

void Player::updatePose(float deltaTime)
{
   // Freeze the current animation while the player falls
   if (mFell)
   {
      return;
   }

   // Ask the crossfade controller to sample the current clip and fade with the next one if necessary
   mCrossFadeController.Update(deltaTime);

   // Get the palette of the pose
   mCrossFadeController.GetCurrentPose().GetMatrixPalette(mPosePalette);

   std::vector<glm::mat4>& inverseBindPose = mSkeleton.GetInvBindPose();

   // Generate the skin matrices
   mSkinMatrices.resize(mPosePalette.size());
   for (unsigned int i = 0,
      size = static_cast<unsigned int>(mPosePalette.size());
      i < size;
      ++i)
   {
      mSkinMatrices[i] = mPosePalette[i] * inverseBindPose[i];
   }
}

void Player::updateOrientation(float xOffset)
{
   // When the user...
   // moves the cursor right, we want the player to rotate right (CWISE around the Y axis of the world)
   // moves the cursor left,  we want the player to rotate left  (CCWISE around the Y axis of the world)

   // Cursor moves right (xOffset is positive) -> Need player to rotate CWISE
   // Cursor moves left  (xOffset is negative) -> Need player to rotate CCWISE
   // Since Q::angleAxis(yaw, ...) results in a CCWISE rotation when it's positive,
   // we need to negate the xOffset to achieve the behaviour that's described above
   float yawChangeOfCameraZInDeg = -xOffset * mCamera3.getMouseSensitivity();

   // The yaw is defined as a CCWISE rotation around the Y axis
   Q::quat yawRot = Q::angleAxis(glm::radians(yawChangeOfCameraZInDeg), glm::vec3(0.0f, 1.0f, 0.0f));

   // The yaw is applied globally
   // In other words, the yaw is applied with respect to the world's Y axis
   mModelTransform.rotation = Q::normalized(mModelTransform.rotation * yawRot);
}

void Player::updateMovementFlags(const std::shared_ptr<Window>& window)
{
   mMovementFlags = 0;

   if (window->keyIsPressed(GLFW_KEY_A))
   {
      mMovementFlags |= 0b00000010;
   }

   if (window->keyIsPressed(GLFW_KEY_D))
   {
      mMovementFlags |= 0b00001000;
   }

   if (window->keyIsPressed(GLFW_KEY_W))
   {
      mMovementFlags |= 0b00000001;
   }

   if (window->keyIsPressed(GLFW_KEY_S))
   {
      mMovementFlags |= 0b00000100;
   }

   if (window->keyIsPressed(GLFW_KEY_SPACE))
   {
      mMovementFlags |= 0b00010000;
   }

   if (window->isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
      mMovementFlags |= 0b00100000;
   }

   if (window->isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
      mMovementFlags |= 0b01000000;
   }
}

void Player::fadeIfNecessary(std::map<std::string, FastClip>& clips,
                             float                            jumpPlaybackSpeed,
                             bool                             jumped,
                             bool                             isFlying)
{
   // If the player had fallen off a platform and stopped flying,
   // or if the player was falling and jumped right when they touched the ground,
   // we consider that as not falling anymore
   if ((mFell && !isFlying) || (mFell && jumped && isFlying))
   {
      mFell = false;
   }

   // If the player had jumped and stopped flying, we consider that as not jumping anymore
   if (mJumped && !isFlying)
   {
      mJumped = false;
   }

   // Check if we should play a jump animation
   if (jumped)
   {
      // Only jumps lock the crossfade controller
      // The call to Unlock() below is necessary for the consecutive jumps that occur
      // when the player holds down the space bar for a long period of time
      // Without it, the jump animation doesn't play when the player bounces off the ground
      if (mCrossFadeController.IsLocked() && mCrossFadeController.IsCurrentClipFinished())
      {
         mCrossFadeController.Unlock();
      }

      // Clear all the targets since we want to fade to the jump animation as directly and quickly as possible
      mCrossFadeController.ClearTargets();

      mCrossFadeController.FadeTo(&clips["Jump"], 0.01f, true, jumpPlaybackSpeed);

      mJumped = true;
   }

   // Check if we finished playing a jump animation
   if (!isFlying)
   {
      if (mCrossFadeController.IsLocked() && mCrossFadeController.IsCurrentClipFinished())
      {
         mCrossFadeController.Unlock();

         if (mIsRunning)
         {
            mCrossFadeController.FadeTo(&clips["Run"], 0.15f, false);
         }
         else
         {
            mCrossFadeController.FadeTo(&clips["Idle"], 0.1f, false);
         }
      }
   }

   // Check if we should play the idle or running animations
   if (!isFlying)
   {
      bool aKeyPressed = ((mMovementFlags & 0b00000010) != 0);
      bool dKeyPressed = ((mMovementFlags & 0b00001000) != 0);
      bool wKeyPressed = ((mMovementFlags & 0b00000001) != 0);
      bool sKeyPressed = ((mMovementFlags & 0b00000100) != 0);
      bool movementKeyPressed = false;

      if (!(wKeyPressed && sKeyPressed)) // If both the W and S keys are pressed at the same time, they cancel each other out
      {
         if (wKeyPressed) // W
         {
            movementKeyPressed = true;
         }

         if (sKeyPressed) // S
         {
            movementKeyPressed = true;
         }
      }

      if (!movementKeyPressed) // No need to perform any additional checks if we already detected that a movement key was pressed
      {
         if (!(aKeyPressed && dKeyPressed)) // If both the A and D keys are pressed at the same time, they cancel each other out
         {
            if (aKeyPressed) // A
            {
               movementKeyPressed = true;
            }

            if (dKeyPressed) // D
            {
               movementKeyPressed = true;
            }
         }
      }

      if (movementKeyPressed)
      {
         if (!mIsRunning)
         {
            mCrossFadeController.FadeTo(&clips["Run"], 0.25f, false);
            mIsRunning = true;
         }
      }
      else
      {
         if (mIsRunning)
         {
            mCrossFadeController.FadeTo(&clips["Idle"], 0.25f, false);
            mIsRunning = false;
         }
      }
   }

   // If the player started flying and they didn't jump, they must have fallen off a platform
   if (isFlying && !mJumped)
   {
      mFell = true;
   }
}

void Player::collideAndSlide(float deltaTime, const std::unique_ptr<World>& world)
{
   mCollisionData.positionInR3 = mModelTransform.position + mEllipsoidVerticalOffset;
   mCollisionData.translationInR3 = mVelocity * deltaTime;

   // Convert the position and translation of the player to Ellipsoid space
   glm::vec3 eSpacePosition = mCollisionData.positionInR3 / mCollisionData.eRadius;
   glm::vec3 eSpaceTranslation = mCollisionData.translationInR3 / mCollisionData.eRadius;

   mCollisionRecursionDepth = 0;
   glm::vec3 finalPosition = collideWithWorld(eSpacePosition, eSpaceTranslation, world);

   // Gravity
   mCollisionData.positionInR3 = finalPosition * mCollisionData.eRadius;
   mCollisionData.translationInR3 = glm::vec3(0.0f, mGravity * deltaTime, 0.0f);
   eSpaceTranslation = mCollisionData.translationInR3 / mCollisionData.eRadius;
   mCollisionRecursionDepth = 0;
   finalPosition = collideWithWorld(finalPosition, eSpaceTranslation, world);

   // Convert the final position of the player to R3
   finalPosition = (finalPosition * mCollisionData.eRadius) - mEllipsoidVerticalOffset;

   if (finalPosition.y < -115.0f)
   {
      // Player died, so we move them back to the origin of the world
      finalPosition = mRespawnPoint;
   }

   // Move the player
   mModelTransform.position = finalPosition;
   mCamera3.setPlayerPosition(mModelTransform.position);
}

glm::vec3 Player::collideWithWorld(const glm::vec3& position, const glm::vec3& translation, const std::unique_ptr<World>& world)
{
   float veryCloseDistance = 0.005f;

   if (mCollisionRecursionDepth > 5)
   {
      return position;
   }

   mCollisionData.translation = translation;
   mCollisionData.normalizedTranslation = glm::normalize(translation);
   mCollisionData.basePoint = position;
   mCollisionData.foundCollision = false;

   world->checkCollision(mCollisionData);

   // If no collision we just move along the translation vector
   if (mCollisionData.foundCollision == false)
   {
      return position + translation;
   }

   // *** Collision occured ***

   // The original destination point
   glm::vec3 destinationPoint = position + translation;
   glm::vec3 newBasePoint = position;

   // Only update if we are not already very close
   // If we do update, we only move very close to intersection (not to the exact spot)
   if (mCollisionData.nearestDistance >= veryCloseDistance)
   {
      // v has the direction of the translation vector and a length of (mCollisionData.nearestDistance - veryCloseDistance)
      glm::vec3 v = glm::normalize(translation) * (mCollisionData.nearestDistance - veryCloseDistance);

      newBasePoint = mCollisionData.basePoint + v;

      // Adjust polygon intersection point (so sliding plane will be unaffected by the fact that we move slightly less than collision tells us)
      v = glm::normalize(v);
      mCollisionData.intersectionPoint -= veryCloseDistance * v;
   }

   // Determine the sliding plane
   glm::vec3 slidePlaneOrigin = mCollisionData.intersectionPoint;
   glm::vec3 slidePlaneNormal = glm::normalize(newBasePoint - mCollisionData.intersectionPoint);
   Plane slidingPlane(slidePlaneOrigin, slidePlaneNormal);

   glm::vec3 newDestinationPoint = destinationPoint - (slidingPlane.signedDistanceTo(destinationPoint) * slidePlaneNormal);

   // Generate the slide vector, which will become our new translation vector for the next iteration
   glm::vec3 newTranslationVector = newDestinationPoint - mCollisionData.intersectionPoint;

   // Don't recurse if the new translation is very small
   if (glm::length(newTranslationVector) < veryCloseDistance)
   {
      return newBasePoint;
   }

   mCollisionRecursionDepth++;
   return collideWithWorld(newBasePoint, newTranslationVector, world);
}
