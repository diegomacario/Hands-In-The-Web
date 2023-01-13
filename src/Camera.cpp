#include <utility>

#include "glm/gtc/matrix_transform.hpp"

#include "Camera.h"

Camera::Camera(glm::vec3 position,
               glm::vec3 target,
               glm::vec3 worldUp,
               float     fieldOfViewYInDeg,
               float     aspectRatio,
               float     near,
               float     far,
               float     movementSpeed,
               float     mouseSensitivity)
   : mPosition(position)
   , mFieldOfViewYInDeg(fieldOfViewYInDeg)
   , mAspectRatio(aspectRatio)
   , mNear(near)
   , mFar(far)
   , mMovementSpeed(movementSpeed)
   , mMouseSensitivity(mouseSensitivity)
   , mIsFree(false)
   , mViewMatrix()
   , mPerspectiveProjectionMatrix()
   , mPerspectiveProjectionViewMatrix()
   , mNeedToUpdateViewMatrix(true)
   , mNeedToUpdatePerspectiveProjectionMatrix(true)
   , mNeedToUpdatePerspectiveProjectionViewMatrix(true)
{
   // There are two important things to note here:
   // - In OpenGL, the camera always looks down the -Z axis, which means that the camera's Z axis is the opposite of the view direction
   // - Q::lookRotation calculates a quaternion that rotates from the world's Z axis to a desired direction,
   //   which makes that direction the Z axis of the rotated coordinate frame
   // That's why the orientation of the camera is calculated using the camera's Z axis instead of the view direction
   glm::vec3 cameraZ = glm::normalize(position - target);
   mOrientation      = Q::lookRotation(cameraZ, worldUp);
}

Camera::Camera(Camera&& rhs) noexcept
   : mPosition(std::exchange(rhs.mPosition, glm::vec3(0.0f)))
   , mOrientation(std::exchange(rhs.mOrientation, Q::quat()))
   , mFieldOfViewYInDeg(std::exchange(rhs.mFieldOfViewYInDeg, 0.0f))
   , mAspectRatio(std::exchange(rhs.mAspectRatio, 0.0f))
   , mNear(std::exchange(rhs.mNear, 0.0f))
   , mFar(std::exchange(rhs.mFar, 0.0f))
   , mMovementSpeed(std::exchange(rhs.mMovementSpeed, 0.0f))
   , mMouseSensitivity(std::exchange(rhs.mMouseSensitivity, 0.0f))
   , mIsFree(std::exchange(rhs.mIsFree, false))
   , mViewMatrix(std::exchange(rhs.mViewMatrix, glm::mat4(0.0f)))
   , mPerspectiveProjectionMatrix(std::exchange(rhs.mPerspectiveProjectionMatrix, glm::mat4(0.0f)))
   , mPerspectiveProjectionViewMatrix(std::exchange(rhs.mPerspectiveProjectionViewMatrix, glm::mat4(0.0f)))
   , mNeedToUpdateViewMatrix(std::exchange(rhs.mNeedToUpdateViewMatrix, true))
   , mNeedToUpdatePerspectiveProjectionMatrix(std::exchange(rhs.mNeedToUpdatePerspectiveProjectionMatrix, true))
   , mNeedToUpdatePerspectiveProjectionViewMatrix(std::exchange(rhs.mNeedToUpdatePerspectiveProjectionViewMatrix, true))
{

}

Camera& Camera::operator=(Camera&& rhs) noexcept
{
   mPosition                                    = std::exchange(rhs.mPosition, glm::vec3(0.0f));
   mOrientation                                 = std::exchange(rhs.mOrientation, Q::quat());
   mFieldOfViewYInDeg                           = std::exchange(rhs.mFieldOfViewYInDeg, 0.0f);
   mAspectRatio                                 = std::exchange(rhs.mAspectRatio, 0.0f);
   mNear                                        = std::exchange(rhs.mNear, 0.0f);
   mFar                                         = std::exchange(rhs.mFar, 0.0f);
   mMovementSpeed                               = std::exchange(rhs.mMovementSpeed, 0.0f);
   mMouseSensitivity                            = std::exchange(rhs.mMouseSensitivity, 0.0f);
   mIsFree                                      = std::exchange(rhs.mIsFree, false);
   mViewMatrix                                  = std::exchange(rhs.mViewMatrix, glm::mat4(0.0f));
   mPerspectiveProjectionMatrix                 = std::exchange(rhs.mPerspectiveProjectionMatrix, glm::mat4(0.0f));
   mPerspectiveProjectionViewMatrix             = std::exchange(rhs.mPerspectiveProjectionViewMatrix, glm::mat4(0.0f));
   mNeedToUpdateViewMatrix                      = std::exchange(rhs.mNeedToUpdateViewMatrix, true);
   mNeedToUpdatePerspectiveProjectionMatrix     = std::exchange(rhs.mNeedToUpdatePerspectiveProjectionMatrix, true);
   mNeedToUpdatePerspectiveProjectionViewMatrix = std::exchange(rhs.mNeedToUpdatePerspectiveProjectionViewMatrix, true);
   return *this;
}

glm::vec3 Camera::getPosition()
{
   return mPosition;
}

glm::mat4 Camera::getViewMatrix()
{
   if (mNeedToUpdateViewMatrix)
   {
      /*
         The camera's model matrix can be calculated as follows (first rotate, then translate):

            cameraModelMat = cameraTranslation * cameraRotation

         The view matrix is equal to the inverse of the camera's model matrix
         Think about it this way:
         - We know the translation and rotation necessary to place and orient the camera
         - If we apply the opposite translation and rotation to the world, it's as if we were looking at it through the camera

         So with that it mind, we can calculate the view matrix as follows:

            viewMat = cameraModelMat^-1 = (cameraTranslation * cameraRotation)^-1 = cameraRotation^-1 * cameraTranslation^-1

         Which looks like this in code:

            glm::mat4 inverseCameraRotation    = Q::quatToMat4(Q::conjugate(mOrientation));
            glm::mat4 inverseCameraTranslation = glm::translate(glm::mat4(1.0), -mPosition);

            mViewMatrix = inverseCameraRotation * inverseCameraTranslation;

         The implementation below is simply an optimized version of the above
      */

      mViewMatrix       = Q::quatToMat4(Q::conjugate(mOrientation));
      // Dot product of X axis with negated position
      mViewMatrix[3][0] = -(mViewMatrix[0][0] * mPosition.x + mViewMatrix[1][0] * mPosition.y + mViewMatrix[2][0] * mPosition.z);
      // Dot product of Y axis with negated position
      mViewMatrix[3][1] = -(mViewMatrix[0][1] * mPosition.x + mViewMatrix[1][1] * mPosition.y + mViewMatrix[2][1] * mPosition.z);
      // Dot product of Z axis with negated position
      mViewMatrix[3][2] = -(mViewMatrix[0][2] * mPosition.x + mViewMatrix[1][2] * mPosition.y + mViewMatrix[2][2] * mPosition.z);

      mNeedToUpdateViewMatrix = false;
   }

   return mViewMatrix;
}

glm::mat4 Camera::getPerspectiveProjectionMatrix()
{
   if (mNeedToUpdatePerspectiveProjectionMatrix)
   {
      mPerspectiveProjectionMatrix = glm::perspective(glm::radians(mFieldOfViewYInDeg),
                                                      mAspectRatio,
                                                      mNear,
                                                      mFar);
      mNeedToUpdatePerspectiveProjectionMatrix = false;
   }

   return mPerspectiveProjectionMatrix;
}

glm::mat4 Camera::getPerspectiveProjectionViewMatrix()
{
   if (mNeedToUpdatePerspectiveProjectionViewMatrix)
   {
      mPerspectiveProjectionViewMatrix = getPerspectiveProjectionMatrix() * getViewMatrix();
      mNeedToUpdatePerspectiveProjectionViewMatrix = false;
   }

   return mPerspectiveProjectionViewMatrix;
}

void Camera::reposition(const glm::vec3& position,
                        const glm::vec3& target,
                        const glm::vec3& worldUp,
                        float            fieldOfViewYInDeg)
{
   mPosition         = position;

   // There are two important things to note here:
   // - In OpenGL, the camera always looks down the -Z axis, which means that the camera's Z axis is the opposite of the view direction
   // - Q::lookRotation calculates a quaternion that rotates from the world's Z axis to a desired direction,
   //   which makes that direction the Z axis of the rotated coordinate frame
   // That's why the orientation of the camera is calculated using the camera's Z axis instead of the view direction
   glm::vec3 cameraZ = glm::normalize(position - target);
   mOrientation      = Q::lookRotation(cameraZ, worldUp);

   mFieldOfViewYInDeg = fieldOfViewYInDeg;

   mNeedToUpdateViewMatrix = true;
   mNeedToUpdatePerspectiveProjectionMatrix = true;
   mNeedToUpdatePerspectiveProjectionViewMatrix = true;
}

void Camera::processKeyboardInput(MovementDirection direction, float deltaTime)
{
   float distanceToMove = mMovementSpeed * deltaTime;

   // When the W/S keys are pressed we want to move along the view direction
   // The view direction is equal to the camera's -Z axis, so to calculate it we can simply rotate the world's -Z axis using the orientation of the camera
   glm::vec3 viewDir = mOrientation * glm::vec3(0.0f, 0.0f, -1.0f);

   // When the D/A keys are pressed we want to move along a vector that points to the right when looking down the view direction
   // That vector happens to be the camera's X axis, so to calculate it we can simply rotate the world's X axis using the orientation of the camera
   glm::vec3 cameraX = mOrientation * glm::vec3(1.0f, 0.0f, 0.0f);

   switch (direction)
   {
   case MovementDirection::Forward:
      mPosition += viewDir * distanceToMove;
      break;
   case MovementDirection::Backward:
      mPosition -= viewDir * distanceToMove;
      break;
   case MovementDirection::Right:
      mPosition += cameraX * distanceToMove;
      break;
   case MovementDirection::Left:
      mPosition -= cameraX * distanceToMove;
      break;
   }

   mNeedToUpdateViewMatrix = true;
   mNeedToUpdatePerspectiveProjectionViewMatrix = true;
}

void Camera::processMouseMovement(float xOffset, float yOffset)
{
   // The xOffset corresponds to a change in the yaw of the view direction
   // If the xOffset is positive, the view direction is moving right
   // If the xOffset is negative, the view direction is moving left

   // The yOffset corresponds to a change in the pitch of the view direction
   // If the yOffset is positive, the view direction is moving up
   // If the yOffset is negative, the view direction is moving down

   // Since the camera's orientation is calculated using the camera's Z axis, which is the opposite the view direction,
   // we need to negate the xOffset and yOffset so that they correspond to changes in the yaw/pitch of the camera's Z axis instead of the view direction

   // Here are some examples to illustrate that idea:
   // - If the xOffset is equal to 1.0f, the view direction is moving right by that amount while the camera's Z axis is moving left by the same amount
   //   Since the camera's Z vector is moving left, its xOffset is equal to -1.0f
   // - If the yOffset is equal to -1.0f, the view direction is moving down by that amount while the camera's Z axis is moving up by the same amount
   //   Since the camera's Z vector is moving up, its yOffset is equal to 1.0f

   float yawChangeOfCameraZInDeg   = -xOffset * mMouseSensitivity;
   float pitchChangeOfCameraZInDeg = -yOffset * mMouseSensitivity;

   // The yaw is defined as a CCWISE rotation around the Y axis
   Q::quat yawRot   = Q::angleAxis(glm::radians(yawChangeOfCameraZInDeg),   glm::vec3(0.0f, 1.0f, 0.0f));
   // The pitch is defined as a CWISE rotation around the X axis
   // Since the rotation is CWISE, the angle is negated in the call to Q::angleAxis below
   Q::quat pitchRot = Q::angleAxis(glm::radians(-pitchChangeOfCameraZInDeg), glm::vec3(1.0f, 0.0f, 0.0f));

   // To avoid introducing roll, the yaw is applied globally while the pitch is applied locally
   // In other words, the yaw is applied with respect to the world's Y axis, while the pitch is applied with respect to the camera's X axis
   mOrientation = Q::normalized(pitchRot * mOrientation * yawRot);

   mNeedToUpdateViewMatrix = true;
   mNeedToUpdatePerspectiveProjectionViewMatrix = true;
}

void Camera::processScrollWheelMovement(float yOffset)
{
   // The larger the FOV, the smaller things appear on the screen
   // The smaller the FOV, the larger things appear on the screen
   if (mFieldOfViewYInDeg >= 1.0f && mFieldOfViewYInDeg <= 45.0f)
   {
      mFieldOfViewYInDeg -= yOffset;
   }
   else if (mFieldOfViewYInDeg < 1.0f)
   {
      mFieldOfViewYInDeg = 1.0f;
   }
   else if (mFieldOfViewYInDeg > 45.0f)
   {
      mFieldOfViewYInDeg = 45.0f;
   }

   mNeedToUpdatePerspectiveProjectionMatrix = true;
   mNeedToUpdatePerspectiveProjectionViewMatrix = true;
}

bool Camera::isFree() const
{
   return mIsFree;
}

void Camera::setFree(bool free)
{
   mIsFree = free;
}
