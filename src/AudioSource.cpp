#include <utility>

#include "AL/al.h"

#include "AudioSource.h"

AudioSource::AudioSource()
{
   alGenSources(1, &mSourceID);
}

AudioSource::AudioSource(const glm::vec3& position,
                         const glm::vec3& velocity,
                         float volume,
                         float pitch,
                         bool relativePosition)
{
   alGenSources(1, &mSourceID);

   setRelative(relativePosition);
   setPosition(position);
   setVelocity(velocity);
   setVolume(volume);
   setPitch(pitch);
}

AudioSource::~AudioSource()
{
   if (mSourceID != 0)
   {
      alDeleteSources(1, &mSourceID);
   }
}

AudioSource::AudioSource(AudioSource&& rhs) noexcept
   : mSourceID(std::exchange(rhs.mSourceID, 0))
{

}

AudioSource& AudioSource::operator=(AudioSource&& rhs) noexcept
{
   mSourceID = std::exchange(rhs.mSourceID, 0);
   return *this;
}

void AudioSource::play(unsigned int bufferID)
{
   alSourceStop(mSourceID);
   alSourcei(mSourceID, AL_BUFFER, bufferID);
   alSourcePlay(mSourceID);
}

void AudioSource::pause()
{
   alSourcePause(mSourceID);
}

void AudioSource::resume()
{
   alSourcePlay(mSourceID);
}

void AudioSource::stop()
{
   alSourceStop(mSourceID);
}

void AudioSource::setVolume(float volume)
{
   alSourcef(mSourceID, AL_GAIN, volume);
}

void AudioSource::setPitch(float pitch)
{
   alSourcef(mSourceID, AL_PITCH, pitch);
}

void AudioSource::setPosition(const glm::vec3& position)
{
   alSource3f(mSourceID, AL_POSITION, position.x, position.y, position.z);
}

void AudioSource::setVelocity(const glm::vec3& velocity)
{
   alSource3f(mSourceID, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}

void AudioSource::setLooping(bool loop)
{
   alSourcei(mSourceID, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

void AudioSource::setRelative(bool relative)
{
   alSourcei(mSourceID, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);
}

bool AudioSource::isPlaying() const
{
   int state;
   alGetSourcei(mSourceID, AL_SOURCE_STATE, &state);
   return (state == AL_PLAYING);
}
