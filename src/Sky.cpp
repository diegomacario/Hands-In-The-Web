#include "stb_image/stb_image.h"

#include "Sky.h"
#include "ResourceManager.h"
#include "ShaderLoader.h"

Sky::Sky()
   : mSkyVAO(0)
   , mSkyVBO(0)
   , mSkyTexture(0)
   , mSkyShader()
{
   mSkyShader = ResourceManager<Shader>().loadUnmanagedResource<ShaderLoader>("resources/shaders/sky.vert",
                                                                              "resources/shaders/sky.frag");

   ConfigureVAO();

   std::vector<std::string> faces { "resources/models/sky/sky.png",    // Positive X
                                    "resources/models/sky/sky.png",    // Negative X
                                    "resources/models/sky/sky.png",    // Positive Y
                                    "resources/models/sky/sky.png",    // Negative Y
                                    "resources/models/sky/sky.png",    // Positive Z
                                    "resources/models/sky/moon.png" }; // Negative Z
   mSkyTexture = LoadCubemap(faces);
}

Sky::~Sky()
{
   glDeleteVertexArrays(1, &mSkyVAO);
   glDeleteBuffers(1, &mSkyVBO);
   glDeleteTextures(1, &mSkyTexture);
}

void Sky::Render(const glm::mat4& projectionView)
{
   glDepthFunc(GL_LEQUAL);

   mSkyShader->use(true);
   mSkyShader->setUniformMat4("projectionView", projectionView);

   // Bind cubemap
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_CUBE_MAP, mSkyTexture);
   mSkyShader->setUniformInt("skybox", 0);

   // Render
   glBindVertexArray(mSkyVAO);
   glDrawArrays(GL_TRIANGLES, 0, 36);
   glBindVertexArray(0);

   // Unbind cubemap
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

   mSkyShader->use(false);

   glDepthFunc(GL_LESS);
}

void Sky::ConfigureVAO()
{
   std::vector<float> vertices = { -1.0f,  1.0f, -1.0f,
                                   -1.0f, -1.0f, -1.0f,
                                    1.0f, -1.0f, -1.0f,
                                    1.0f, -1.0f, -1.0f,
                                    1.0f,  1.0f, -1.0f,
                                   -1.0f,  1.0f, -1.0f,

                                   -1.0f, -1.0f,  1.0f,
                                   -1.0f, -1.0f, -1.0f,
                                   -1.0f,  1.0f, -1.0f,
                                   -1.0f,  1.0f, -1.0f,
                                   -1.0f,  1.0f,  1.0f,
                                   -1.0f, -1.0f,  1.0f,

                                    1.0f, -1.0f, -1.0f,
                                    1.0f, -1.0f,  1.0f,
                                    1.0f,  1.0f,  1.0f,
                                    1.0f,  1.0f,  1.0f,
                                    1.0f,  1.0f, -1.0f,
                                    1.0f, -1.0f, -1.0f,

                                   -1.0f, -1.0f,  1.0f,
                                   -1.0f,  1.0f,  1.0f,
                                    1.0f,  1.0f,  1.0f,
                                    1.0f,  1.0f,  1.0f,
                                    1.0f, -1.0f,  1.0f,
                                   -1.0f, -1.0f,  1.0f,

                                   -1.0f,  1.0f, -1.0f,
                                    1.0f,  1.0f, -1.0f,
                                    1.0f,  1.0f,  1.0f,
                                    1.0f,  1.0f,  1.0f,
                                   -1.0f,  1.0f,  1.0f,
                                   -1.0f,  1.0f, -1.0f,

                                   -1.0f, -1.0f, -1.0f,
                                   -1.0f, -1.0f,  1.0f,
                                    1.0f, -1.0f, -1.0f,
                                    1.0f, -1.0f, -1.0f,
                                   -1.0f, -1.0f,  1.0f,
                                    1.0f, -1.0f,  1.0f
   };

   glGenVertexArrays(1, &mSkyVAO);
   glBindVertexArray(mSkyVAO);

   // Load the positions
   glGenBuffers(1, &mSkyVBO);
   glBindBuffer(GL_ARRAY_BUFFER, mSkyVBO);
   glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

   // Connect the positions with the correct attribute of the Sky shader
   int positionsAttribLoc = mSkyShader->getAttributeLocation("position");
   glVertexAttribPointer(positionsAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(positionsAttribLoc);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
}

unsigned int Sky::LoadCubemap(const std::vector<std::string>& faces)
{
   unsigned int cubemapTexture;
   glGenTextures(1, &cubemapTexture);
   glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

   int width, height, numComponents;
   for (unsigned int i = 0; i < faces.size(); i++)
   {
      unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &numComponents, 0);
      if (data)
      {
         GLenum format;
         switch (numComponents)
         {
         case 3:
            format = GL_RGB;
            break;
         case 4:
            format = GL_RGBA;
            break;
         default:
            std::cout << "Error - Sky::LoadCubemap - The texture has an invalid number of components: " << numComponents << "\n";
         }
         glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
         stbi_image_free(data);
      }
      else
      {
         std::cout << "Error - Sky::LoadCubemap - Failed to load texture: " << faces[i] << "\n";
         stbi_image_free(data);
      }
   }

   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

   glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

   return cubemapTexture;
}
