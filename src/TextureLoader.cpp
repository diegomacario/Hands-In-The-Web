#include <iostream>

#include "stb_image/stb_image.h"

#include "TextureLoader.h"

std::shared_ptr<Texture> TextureLoader::loadResource(const std::string& texFilePath,
                                                     int*               outWidth,
                                                     int*               outHeight,
                                                     unsigned int       wrapS,
                                                     unsigned int       wrapT,
                                                     unsigned int       minFilter,
                                                     unsigned int       magFilter,
                                                     bool               genMipmap) const
{
   int width, height, numComponents;
   std::unique_ptr<unsigned char, void(*)(void*)> texData(stbi_load(texFilePath.c_str(), &width, &height, &numComponents, STBI_rgb), stbi_image_free);

   if (!texData)
   {
      std::cout << "Error - TextureLoader::loadResource - The following texture could not be loaded: " << texFilePath << "\n";
      return nullptr;
   }

   if (numComponents != 3 && numComponents != 4)
   {
      std::cout << "Error - TextureLoader::loadResource - The texture has an invalid number of components: " << numComponents << "\n";
      return nullptr;
   }

   unsigned int texID = generateTexture(texData, width, height, 3, wrapS, wrapT, minFilter, magFilter, genMipmap);

   if (outWidth)
   {
      *outWidth = width;
   }

   if (outHeight)
   {
      *outHeight = height;
   }

   return std::make_shared<Texture>(texID);
}

std::shared_ptr<Texture> TextureLoader::loadResource(const unsigned char* texDataBuffer,
                                                     int                  texDataLength,
                                                     int*                 outWidth,
                                                     int*                 outHeight,
                                                     unsigned int         wrapS,
                                                     unsigned int         wrapT,
                                                     unsigned int         minFilter,
                                                     unsigned int         magFilter,
                                                     bool                 genMipmap) const
{
   int width, height, numComponents;
   std::unique_ptr<unsigned char, void(*)(void*)> texData(stbi_load_from_memory(texDataBuffer, texDataLength, &width, &height, &numComponents, STBI_rgb), stbi_image_free);

   if (!texData)
   {
      std::cout << "Error - TextureLoader::loadResource - Could not load texture from memory" << "\n";
      return nullptr;
   }

   if (numComponents != 3 && numComponents != 4)
   {
      std::cout << "Error - TextureLoader::loadResource - The texture has an invalid number of components: " << numComponents << "\n";
      return nullptr;
   }

   unsigned int texID = generateTexture(texData, width, height, 3, wrapS, wrapT, minFilter, magFilter, genMipmap);

   if (outWidth)
   {
      *outWidth = width;
   }

   if (outHeight)
   {
      *outHeight = height;
   }

   return std::make_shared<Texture>(texID);
}

unsigned int TextureLoader::generateTexture(const std::unique_ptr<unsigned char, void(*)(void*)>& texData,
                                            int          width,
                                            int          height,
                                            int          numComponents,
                                            unsigned int wrapS,
                                            unsigned int wrapT,
                                            unsigned int minFilter,
                                            unsigned int magFilter,
                                            bool         genMipmap) const
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
      std::cout << "Error - TextureLoader::generateTexture - The texture has an invalid number of components: " << numComponents << "\n";
   }

   unsigned int texID;
   glGenTextures(1, &texID);
   glBindTexture(GL_TEXTURE_2D, texID);
   glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texData.get());

   if (genMipmap)
   {
      glGenerateMipmap(GL_TEXTURE_2D);
   }

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

   glBindTexture(GL_TEXTURE_2D, 0);

   return texID;
}
