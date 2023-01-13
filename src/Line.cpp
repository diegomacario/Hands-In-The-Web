#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif
#include <glm/gtc/matrix_transform.hpp>

#include <array>

#include "line.h"

Line::Line(glm::vec3 startPoint,
           glm::vec3 endPoint,
           glm::vec3 color)
   : mStartPoint(startPoint)
   , mEndPoint(endPoint)
   , mPosition(glm::vec3(0.0f))
   , mRotation(Q::quat())
   , mScalingFactor(1.0f)
   , mColor(color)
   , mModelMatrix(1.0f)
   , mCalculateModelMatrix(true)
   , mVAO(0)
   , mVBO(0)
{
   calculateModelMatrix();
   configureVAO(startPoint, endPoint);
}

Line::Line(glm::vec3        startPoint,
           glm::vec3        endPoint,
           const glm::vec3& position,
           float            angleOfRotInDeg,
           const glm::vec3& axisOfRot,
           float            scalingFactor,
           glm::vec3        color)
   : mStartPoint(startPoint)
   , mEndPoint(endPoint)
   , mPosition(position)
   , mRotation(Q::angleAxis(glm::radians(angleOfRotInDeg), axisOfRot))
   , mScalingFactor(scalingFactor != 0.0f ? scalingFactor : 1.0f)
   , mColor(color)
   , mModelMatrix(1.0f)
   , mCalculateModelMatrix(true)
   , mVAO(0)
   , mVBO(0)
{
   calculateModelMatrix();
   configureVAO(startPoint, endPoint);
}

Line::~Line()
{
   glDeleteVertexArrays(1, &mVAO);
   glDeleteBuffers(1, &mVBO);
}

Line::Line(Line&& rhs) noexcept
   : mStartPoint(std::exchange(rhs.mStartPoint, glm::vec3(0.0f)))
   , mEndPoint(std::exchange(rhs.mEndPoint, glm::vec3(0.0f)))
   , mPosition(std::exchange(rhs.mPosition, glm::vec3(0.0f)))
   , mRotation(std::exchange(rhs.mRotation, Q::quat()))
   , mScalingFactor(std::exchange(rhs.mScalingFactor, 1.0f))
   , mColor(std::exchange(rhs.mColor, glm::vec3(0.0f)))
   , mModelMatrix(std::exchange(rhs.mModelMatrix, glm::mat4(1.0f)))
   , mCalculateModelMatrix(std::exchange(rhs.mCalculateModelMatrix, true))
   , mVAO(std::exchange(rhs.mVAO, 0))
   , mVBO(std::exchange(rhs.mVBO, 0))
{

}

Line& Line::operator=(Line&& rhs) noexcept
{
   mStartPoint = std::exchange(rhs.mStartPoint, glm::vec3(0.0f));
   mEndPoint = std::exchange(rhs.mEndPoint, glm::vec3(0.0f));
   mPosition = std::exchange(rhs.mPosition, glm::vec3(0.0f));
   mRotation = std::exchange(rhs.mRotation, Q::quat());
   mScalingFactor = std::exchange(rhs.mScalingFactor, 1.0f);
   mColor = std::exchange(rhs.mColor, glm::vec3(0.0f));
   mModelMatrix = std::exchange(rhs.mModelMatrix, glm::mat4(1.0f));
   mCalculateModelMatrix = std::exchange(rhs.mCalculateModelMatrix, true);
   mVAO = std::exchange(rhs.mVAO, 0);
   mVBO = std::exchange(rhs.mVBO, 0);
   return *this;
}

void Line::render(const std::shared_ptr<Shader>& shader) const
{
   if (mCalculateModelMatrix)
   {
      calculateModelMatrix();
   }

   shader->setUniformMat4("model", mModelMatrix);
   shader->setUniformVec3("color", mColor);

   // Render line
   glBindVertexArray(mVAO);
   glLineWidth(5);
   glDrawArrays(GL_LINES, 0, 2);
   glLineWidth(1);
   glBindVertexArray(0);
}

glm::vec3 Line::getStartPoint() const
{
   return mStartPoint;
}

glm::vec3 Line::getEndPoint() const
{
   return mEndPoint;
}

glm::vec3 Line::getPosition() const
{
   return mPosition;
}

void Line::setPosition(const glm::vec3& position)
{
   mPosition = position;
   mCalculateModelMatrix = true;
}

float Line::getScalingFactor() const
{
   return mScalingFactor;
}

void Line::setRotation(const Q::quat& rotation)
{
   mRotation = rotation;
   mCalculateModelMatrix = true;
}

void Line::translate(const glm::vec3& translation)
{
   mPosition += translation;
   mCalculateModelMatrix = true;
}

void Line::rotateByMultiplyingCurrentRotationFromTheLeft(const Q::quat& rotation)
{
   mRotation = rotation * mRotation;
   mCalculateModelMatrix = true;
}

void Line::rotateByMultiplyingCurrentRotationFromTheRight(const Q::quat& rotation)
{
   mRotation = mRotation * rotation;
   mCalculateModelMatrix = true;
}

void Line::scale(float scalingFactor)
{
   if (scalingFactor != 0.0f)
   {
      mScalingFactor *= scalingFactor;
      mCalculateModelMatrix = true;
   }
}

void Line::calculateModelMatrix() const
{
   // 3) Translate the model
   mModelMatrix = glm::translate(glm::mat4(1.0f), mPosition);

   // 2) Rotate the model
   mModelMatrix *= quatToMat4(mRotation);

   // 1) Scale the model
   mModelMatrix = glm::scale(mModelMatrix, glm::vec3(mScalingFactor));

   mCalculateModelMatrix = false;
}

void Line::configureVAO(glm::vec3 startPoint, glm::vec3 endPoint)
{
   glGenVertexArrays(1, &mVAO);
   glGenBuffers(1, &mVBO);

   std::array<float, 6> vertices = { startPoint.x, startPoint.y, startPoint.z, endPoint.x, endPoint.y, endPoint.z };

   std::array<unsigned int, 2> indices = { 0, 1 };

   // Configure the VAO of the line
   glBindVertexArray(mVAO);

   // Load the line's data into the buffers

   // Positions
   glBindBuffer(GL_ARRAY_BUFFER, mVBO);
   glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

   // Set the vertex attribute pointers
   // Positions
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

   glBindVertexArray(0);
}
