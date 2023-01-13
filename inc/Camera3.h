#ifndef CAMERA3_H
#define CAMERA3_H

#include "glm/glm.hpp"

#include "Quat.h"

class Camera3
{
public:

   Camera3(float            distanceBetweenPlayerAndCamera,
           float            cameraPitch,
           const glm::vec3& playerPosition,
           const Q::quat&   playerOrientation,
           const glm::vec3& offsetFromPlayerPositionToCameraTarget,
           float            distanceBetweenPlayerAndCameraLowerLimit,
           float            distanceBetweenPlayerAndCameraUpperLimit,
           float            cameraPitchLowerLimit,
           float            cameraPitchUpperLimit,
           float            fieldOfViewYInDeg,
           float            aspectRatio,
           float            near,
           float            far,
           float            mouseSensitivity);
   ~Camera3() = default;

   Camera3(const Camera3&) = default;
   Camera3& operator=(const Camera3&) = default;

   Camera3(Camera3&& rhs) noexcept;
   Camera3& operator=(Camera3&& rhs) noexcept;

   const glm::vec3& getPosition() const;
   const Q::quat&   getOrientation() const;

   const glm::mat4& getViewMatrix() const;
   const glm::mat4& getPerspectiveProjectionMatrix() const;
   const glm::mat4& getPerspectiveProjectionViewMatrix() const;

   void             reposition(float            distanceBetweenPlayerAndCamera,
                               float            cameraPitch,
                               const glm::vec3& playerPosition,
                               const Q::quat&   playerOrientation,
                               const glm::vec3& offsetFromPlayerPositionToCameraTarget,
                               float            distanceBetweenPlayerAndCameraLowerLimit,
                               float            distanceBetweenPlayerAndCameraUpperLimit,
                               float            cameraPitchLowerLimit,
                               float            cameraPitchUpperLimit);

   enum class MovementDirection
   {
      Forward,
      Backward,
      Left,
      Right
   };

   void             processMouseMovement(float yOffset);
   void             processScrollWheelMovement(float yOffset);
   void             setPlayerPosition(const glm::vec3& playerPosition);
   void             setPlayerOrientation(const Q::quat& playerOrientation);

   bool             isFree() const { return mIsFree; }
   void             setFree(bool free) { mIsFree = free; }

   void             enableFirstPersonMode();
   void             enableThirdPersonMode();

   enum class CameraMode
   {
      FirstPerson,
      ThirdPerson
   };

   CameraMode       getCameraMode() const { return mCameraMode; };

   void             getViewAndUpVectors(glm::vec3& outView, glm::vec3& outUp) const;

   float            getMouseSensitivity() const { return mMouseSensitivity; }

private:

   void             calculateInitialCameraOrientationWRTPlayer();
   void             updatePositionAndOrientationOfCamera() const;
   void             updateViewMatrix() const;
   void             updatePerspectiveProjectionMatrix() const;
   void             updatePerspectiveProjectionViewMatrix() const;

   mutable glm::vec3 mCameraPosition;
   Q::quat           mCameraOrientationWRTPlayer;
   mutable Q::quat   mCameraGlobalOrientation;

   glm::vec3         mPlayerPosition;
   Q::quat           mPlayerOrientation;
   glm::vec3         mOffsetFromPlayerPositionToCameraTarget;

   float             mDistanceBetweenPlayerAndCamera;
   float             mCameraPitch;

   float             mDistanceBetweenPlayerAndCameraLowerLimit;
   float             mDistanceBetweenPlayerAndCameraUpperLimit;
   float             mCameraPitchLowerLimit;
   float             mCameraPitchUpperLimit;

   float             mOriginalFieldOfViewYInDeg;
   float             mFieldOfViewYInDeg;
   float             mAspectRatio;
   float             mNear;
   float             mFar;

   float             mMouseSensitivity;

   mutable glm::mat4 mViewMatrix;
   mutable glm::mat4 mPerspectiveProjectionMatrix;
   mutable glm::mat4 mPerspectiveProjectionViewMatrix;

   mutable bool      mNeedToUpdatePositionAndOrientationOfCamera;
   mutable bool      mNeedToUpdateViewMatrix;
   mutable bool      mNeedToUpdatePerspectiveProjectionMatrix;
   mutable bool      mNeedToUpdatePerspectiveProjectionViewMatrix;

   bool              mIsFree = false;

   CameraMode        mCameraMode = CameraMode::ThirdPerson;
};

#endif
