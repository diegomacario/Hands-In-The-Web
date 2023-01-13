#ifndef TEXTURE_H
#define TEXTURE_H

class Texture
{
public:

   explicit Texture(unsigned int texID);
   ~Texture();

   Texture(const Texture&) = delete;
   Texture& operator=(const Texture&) = delete;

   Texture(Texture&& rhs) noexcept;
   Texture& operator=(Texture&& rhs) noexcept;

   void bind(unsigned int textureUnit, int uniformLocation) const;
   void unbind(unsigned int textureUnit) const;

private:

   unsigned int mTexID;
};

#endif
