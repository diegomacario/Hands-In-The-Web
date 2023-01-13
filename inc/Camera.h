#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"

#include "Quat.h"

class Camera
{
public:

   Camera(glm::vec3 position,
          glm::vec3 target,
          glm::vec3 worldUp,
          float     fieldOfViewYInDeg,
          float     aspectRatio,
          float     near,
          float     far,
          float     movementSpeed,
          float     mouseSensitivity);
   ~Camera() = default;

   Camera(const Camera&) = default;
   Camera& operator=(const Camera&) = default;

   Camera(Camera&& rhs) noexcept;
   Camera& operator=(Camera&& rhs) noexcept;

   glm::vec3 getPosition();

   glm::mat4 getViewMatrix();
   glm::mat4 getPerspectiveProjectionMatrix();
   glm::mat4 getPerspectiveProjectionViewMatrix();

   void      reposition(const glm::vec3& position,
                        const glm::vec3& target,
                        const glm::vec3& worldUp,
                        float            fieldOfViewYInDeg);

   enum class MovementDirection
   {
      Forward,
      Backward,
      Left,
      Right
   };

   void      processKeyboardInput(MovementDirection direction, float deltaTime);
   void      processMouseMovement(float xOffset, float yOffset);
   void      processScrollWheelMovement(float yOffset);

   bool      isFree() const;
   void      setFree(bool free);

private:

   glm::vec3 mPosition;
   Q::quat   mOrientation;

   float     mFieldOfViewYInDeg;
   float     mAspectRatio;
   float     mNear;
   float     mFar;

   float     mMovementSpeed;
   float     mMouseSensitivity;

   bool      mIsFree;

   glm::mat4 mViewMatrix;
   glm::mat4 mPerspectiveProjectionMatrix;
   glm::mat4 mPerspectiveProjectionViewMatrix;

   bool      mNeedToUpdateViewMatrix;
   bool      mNeedToUpdatePerspectiveProjectionMatrix;
   bool      mNeedToUpdatePerspectiveProjectionViewMatrix;
};

#endif
