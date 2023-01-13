#ifndef FLY_CAM_H
#define FLY_CAM_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class FlyCam
{
public:

   FlyCam(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
          glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f),
          float yaw          = -90.0f,
          float pitch        = 0.0f)
      : mFront(glm::vec3(0.0f, 0.0f, -1.0f))
      , mMovementSpeed(20.0f)
      , mMouseSensitivity(0.2f)
      , mZoom(45.0f)
   {
      mPosition = position;
      mWorldUp  = up;
      mYaw      = yaw;
      mPitch    = pitch;

      UpdateCameraVectors();
   }

   glm::mat4 GetViewMatrix() const
   {
      return glm::lookAt(mPosition, mPosition + mFront, mUp);
   }

   glm::mat4 GetPerspectiveProjectionMatrix() const
   {
      return glm::perspective(glm::radians(mZoom), 1280.0f / 720.0f, 1.0f, 1000.0f);
   }

   enum FlyCamMovement
   {
      FORWARD,
      BACKWARD,
      LEFT,
      RIGHT
   };

   void ProcessKeyboard(FlyCamMovement direction, float deltaTime)
   {
      float velocity = mMovementSpeed * deltaTime;
      if (direction == FORWARD)
         mPosition += mFront * velocity;
      if (direction == BACKWARD)
         mPosition -= mFront * velocity;
      if (direction == LEFT)
         mPosition -= mRight * velocity;
      if (direction == RIGHT)
         mPosition += mRight * velocity;
   }

   void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
   {
      xoffset *= mMouseSensitivity;
      yoffset *= mMouseSensitivity;

      mYaw   += xoffset;
      mPitch += yoffset;

      if (constrainPitch)
      {
         if (mPitch > 89.0f)
            mPitch = 89.0f;
         if (mPitch < -89.0f)
            mPitch = -89.0f;
      }

      UpdateCameraVectors();
   }

   void ProcessMouseScroll(float yoffset)
   {
      mZoom -= (float)yoffset;
      if (mZoom < 1.0f)
         mZoom = 1.0f;
      if (mZoom > 45.0f)
         mZoom = 45.0f;
   }

private:

   void UpdateCameraVectors()
   {
      glm::vec3 front;
      front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
      front.y = sin(glm::radians(mPitch));
      front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
      mFront  = glm::normalize(front);
      mRight  = glm::normalize(glm::cross(mFront, mWorldUp));
      mUp     = glm::normalize(glm::cross(mRight, mFront));
   }

   glm::vec3 mPosition;
   glm::vec3 mFront;
   glm::vec3 mUp;
   glm::vec3 mRight;
   glm::vec3 mWorldUp;

   float     mYaw;
   float     mPitch;

   float     mMovementSpeed;
   float     mMouseSensitivity;
   float     mZoom;
};

#endif
