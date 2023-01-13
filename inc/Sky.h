#ifndef SKY_H
#define SKY_H

#include <memory>

#include "Shader.h"

class Sky
{
public:

   Sky();
   ~Sky();

   void         Render(const glm::mat4& projectionView);

private:

   void         ConfigureVAO();
   unsigned int LoadCubemap(const std::vector<std::string>& faces);

   unsigned int             mSkyVAO;
   unsigned int             mSkyVBO;
   unsigned int             mSkyTexture;

   std::shared_ptr<Shader>  mSkyShader;
};

#endif
