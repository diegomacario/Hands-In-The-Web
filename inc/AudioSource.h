#ifndef AUDIO_SOURCE_H
#define AUDIO_SOURCE_H

#include "glm/glm.hpp"

class AudioSource
{
public:

   AudioSource();
   AudioSource(const glm::vec3& position,
               const glm::vec3& velocity,
               float volumedB,
               float pitch,
               bool relativePosition);
   ~AudioSource();

   AudioSource(const AudioSource&) = delete;
   AudioSource& operator=(const AudioSource&) = delete;

   AudioSource(AudioSource&& rhs) noexcept;
   AudioSource& operator=(AudioSource&& rhs) noexcept;

   void play(unsigned int bufferID);
   void pause();
   void resume();
   void stop();

   void setVolume(float volume);
   void setPitch(float pitch);
   void setPosition(const glm::vec3& position);
   void setVelocity(const glm::vec3& velocity);
   void setLooping(bool loop);
   void setRelative(bool relative);

   bool isPlaying() const;

private:

   unsigned int mSourceID;
};

#endif
