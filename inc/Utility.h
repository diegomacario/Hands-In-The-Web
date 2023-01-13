#ifndef UTILITY_H
#define UTILITY_H

#include <memory>

#include "glm/glm.hpp"

#include "World.h"

namespace Utility
{
   const float epsilon = 0.000001f;
   const float runningSpeed = 16.0f;

   glm::vec2 normalizeWithZeroLengthCheck(const glm::vec2& v);
   glm::vec3 normalizeWithZeroLengthCheck(const glm::vec3& v);

   void      movePlayer(glm::vec3&                    ioPosition,
                        glm::vec3&                    ioVelocity,
                        const glm::vec3&              cameraFront,
                        const glm::vec3&              cameraRight,
                        unsigned int                  movementFlags,
                        const std::unique_ptr<World>& world,
                        bool*                         outJumped   = nullptr,
                        bool*                         outIsFlying = nullptr);

   bool      distanceToGround(const glm::vec3&              playerPosition,
                              const std::unique_ptr<World>& world,
                              float&                        outDistance);
}

#endif
