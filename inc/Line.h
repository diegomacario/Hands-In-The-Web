#ifndef LINE_H
#define LINE_H

#include <glm/glm.hpp>

#include <memory>

#include "shader.h"
#include "quat.h"

class Line
{
public:

   Line(glm::vec3 startPoint,
        glm::vec3 endPoint,
        glm::vec3 color);

   Line(glm::vec3        startPoint,
        glm::vec3        endPoint,
        const glm::vec3& position,
        float            angleOfRotInDeg,
        const glm::vec3& axisOfRot,
        float            scalingFactor,
        glm::vec3        color);

   ~Line();

   Line(const Line&) = delete;
   Line& operator=(const Line&) = delete;

   Line(Line&& rhs) noexcept;
   Line& operator=(Line&& rhs) noexcept;

   void      render(const std::shared_ptr<Shader>& shader) const;

   glm::vec3 getStartPoint() const;
   glm::vec3 getEndPoint() const;

   glm::vec3 getPosition() const;
   void      setPosition(const glm::vec3& position);

   float     getScalingFactor() const;

   void      setRotation(const Q::quat& rotation);

   void      translate(const glm::vec3& translation);
   void      rotateByMultiplyingCurrentRotationFromTheLeft(const Q::quat& rotation);
   void      rotateByMultiplyingCurrentRotationFromTheRight(const Q::quat& rotation);
   void      scale(float scalingFactor);

private:

   void      configureVAO(glm::vec3 startPoint, glm::vec3 endPoint);

   void      calculateModelMatrix() const;

   glm::vec3         mStartPoint;
   glm::vec3         mEndPoint;

   glm::vec3         mPosition;
   Q::quat           mRotation;
   float             mScalingFactor;

   glm::vec3         mColor;

   mutable glm::mat4 mModelMatrix;
   mutable bool      mCalculateModelMatrix;

   unsigned int      mVAO;
   unsigned int      mVBO;
};

#endif
