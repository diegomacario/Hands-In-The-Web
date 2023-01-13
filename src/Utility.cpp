#include "glm/gtx/norm.hpp"

#include "Utility.h"

// This helper function is identical to glm::normalize, except that it checks if the length of the vector
// we want to normalize is zero, and if it is then it doesn't normalize it
glm::vec2 Utility::normalizeWithZeroLengthCheck(const glm::vec2& v)
{
   float squaredLen = glm::length2(v);
   if (squaredLen < Utility::epsilon)
   {
      return v;
   }

   return v / glm::sqrt(squaredLen);
}

// This helper function is identical to glm::normalize, except that it checks if the length of the vector
// we want to normalize is zero, and if it is then it doesn't normalize it
glm::vec3 Utility::normalizeWithZeroLengthCheck(const glm::vec3& v)
{
   float squaredLen = glm::length2(v);
   if (squaredLen < Utility::epsilon)
   {
      return v;
   }

   return v / glm::sqrt(squaredLen);
}

void Utility::movePlayer(glm::vec3&                    ioPosition,
                         glm::vec3&                    ioVelocity,
                         const glm::vec3&              cameraFront,
                         const glm::vec3&              cameraRight,
                         unsigned int                  movementFlags,
                         const std::unique_ptr<World>& world,
                         bool*                         outJumped,
                         bool*                         outIsFlying)
{
   ioVelocity.x *= 0.9f;
   ioVelocity.y *= 0.99f;
   ioVelocity.z *= 0.9f;

   bool aKeyPressed = (movementFlags & 0b00000010) != 0;
   bool dKeyPressed = (movementFlags & 0b00001000) != 0;
   bool wKeyPressed = (movementFlags & 0b00000001) != 0;
   bool sKeyPressed = (movementFlags & 0b00000100) != 0;

   // Move and orient the character
   glm::vec3 wishVelocity(0.0f);

   bool processedForwardAndBackwardMovement = false;
   bool horizontalMovementCancelsOut = (aKeyPressed && dKeyPressed);
   if (!horizontalMovementCancelsOut) // If both the A and D keys are pressed at the same time, they cancel each other out
   {
      if (aKeyPressed) // A (left)
      {
         bool forwardLeftDiagonal            = (wKeyPressed && aKeyPressed);
         bool backwardLeftDiagonal           = (sKeyPressed && aKeyPressed);
         bool leftDiagonalMovementCancelsOut = (forwardLeftDiagonal && backwardLeftDiagonal);
         if (!leftDiagonalMovementCancelsOut)
         {
            if (forwardLeftDiagonal)
            {
               // Forward left diagonal
               wishVelocity += Utility::runningSpeed * glm::normalize(cameraFront - cameraRight);
            }
            else if (backwardLeftDiagonal)
            {
               // Backward left diagonal
               wishVelocity += Utility::runningSpeed * glm::normalize(-cameraFront - cameraRight);
            }
            else
            {
               // Left
               wishVelocity -= Utility::runningSpeed * cameraRight;
            }
         }
         else // If the left diagonal movement cancels out, we just move to the left
         {
            // Left
            wishVelocity -= Utility::runningSpeed * cameraRight;
         }

         processedForwardAndBackwardMovement = true;
      }
      else if (dKeyPressed) // D (right)
      {
         bool forwardRightDiagonal            = (wKeyPressed && dKeyPressed);
         bool backwardRightDiagonal           = (sKeyPressed && dKeyPressed);
         bool rightDiagonalMovementCancelsOut = (forwardRightDiagonal && backwardRightDiagonal);
         if (!rightDiagonalMovementCancelsOut)
         {
            if (forwardRightDiagonal)
            {
               // Forward right diagonal
               wishVelocity += Utility::runningSpeed * glm::normalize(cameraFront + cameraRight);
            }
            else if (backwardRightDiagonal)
            {
               // Backward right diagonal
               wishVelocity += Utility::runningSpeed * glm::normalize(-cameraFront + cameraRight);
            }
            else
            {
               // Right
               wishVelocity += Utility::runningSpeed * cameraRight;
            }
         }
         else // If the right diagonal movement cancels out, we just move to the right
         {
            // Right
            wishVelocity += Utility::runningSpeed * cameraRight;
         }

         processedForwardAndBackwardMovement = true;
      }
   }

   if (!processedForwardAndBackwardMovement)
   {
      bool forwardMovementCancelsOut = (wKeyPressed && sKeyPressed);
      if (!forwardMovementCancelsOut) // If both the W and S keys are pressed at the same time, they cancel each other out
      {
         if (wKeyPressed) // W
         {
            wishVelocity += Utility::runningSpeed * cameraFront;
         }
         else if (sKeyPressed) // S
         {
            wishVelocity -= Utility::runningSpeed * cameraFront;
         }
      }
   }

   float wishSpeed = glm::length(wishVelocity);
   glm::vec3 wishDir = Utility::normalizeWithZeroLengthCheck(wishVelocity);

   float currSpeed = glm::dot(ioVelocity, wishDir);
   float addSpeed = wishSpeed - currSpeed;
   float accelSpeed = 0.1f * wishSpeed;

   if (accelSpeed > addSpeed)
   {
      accelSpeed = addSpeed;
   }

   ioVelocity += (wishDir * accelSpeed);

   // Check if the player is flying
   float height = 0.0f;
   bool  isFlying = false;
   if (distanceToGround(ioPosition, world, height))
   {
      isFlying = (height > 2.25f);
   }
   else
   {
      // Ray didn't hit anything so we assume that the player is flying in the void
      isFlying = true;
   }

   if (!isFlying && ((movementFlags & 0b00010000) != 0)) // Space bar
   {
      ioVelocity.y = 42.5f;

      isFlying = true;
      if (outJumped)
      {
         *outJumped = true;
      }
   }
   else if (outJumped)
   {
      *outJumped = false;
   }

   ioVelocity.y += -30.0f * (1.0f / 60.0f);

   if (!isFlying)
   {
      ioVelocity.y = 0.0f;
   }

   if (outIsFlying)
   {
      *outIsFlying = isFlying;
   }
}

bool Utility::distanceToGround(const glm::vec3&              playerPosition,
                               const std::unique_ptr<World>& world,
                               float&                        outDistance)
{
   // Note how we shoot the ray from 1 unit above the position of the player's feet
   // That means that the actual distance from the player's feet to the ground is outDistance - 1
   glm::vec3 origin = playerPosition + glm::vec3(0.0f, 1.0f, 0.0f);
   Ray       downRay(origin, glm::vec3(0.0f, -1.0f, 0.0f));
   glm::vec3 intersectionPoint(0.0f);

   if (world->checkIntersection(downRay, intersectionPoint))
   {
      outDistance = glm::length(intersectionPoint - origin);
      return true;
   }

   return false;
}
