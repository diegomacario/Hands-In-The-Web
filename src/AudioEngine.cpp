#include <iostream>
#include <vector>

#include "AL/al.h"
#include "dr_libs/dr_wav.h"

#include "AudioEngine.h"

AudioEngine::AudioEngine()
   : mContext(nullptr)
   , mNextSourceID(0)
   , mSounds()
   , mSources()
{

}

AudioEngine::~AudioEngine()
{
   // Delete all the sources
   mSources.clear();

   // Delete all the buffers
   for (auto it = mSounds.begin(), itEnd = mSounds.end(); it != itEnd; ++it)
   {
      alDeleteBuffers(1, &(it->second));
   }

   ALCdevice* device = alcGetContextsDevice(mContext);
   alcMakeContextCurrent(nullptr);
   alcDestroyContext(mContext);
   alcCloseDevice(device);
}

bool AudioEngine::initialize()
{
   ALCdevice* device = alcOpenDevice(nullptr);
   if (!device)
   {
      std::cout << "Error - AudioEngine::initialize - Failed to open a handle to an audio device" << "\n";
      return false;
   }

   mContext = alcCreateContext(device, nullptr);
   if (!alcMakeContextCurrent(mContext))
   {
      std::cout << "Error - AudioEngine::initialize - Failed to create the OpenAL context" << "\n";
      return false;
   }

   return true;
}

void AudioEngine::update()
{
   std::vector<SourceMap::iterator> stoppedSources;
   for (auto it = mSources.begin(), itEnd = mSources.end(); it != itEnd; ++it)
   {
      if (!it->second.isPlaying())
      {
         stoppedSources.push_back(it);
      }
   }

   for (auto& it : stoppedSources)
   {
      mSources.erase(it);
   }
}

void AudioEngine::setListenerData(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& at, const glm::vec3& up)
{
   alListener3f(AL_POSITION, position.x, position.y, position.z);
   alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
   float orientation[] = { at.x, at.y, at.z, up.x, up.y, up.z };
   alListenerfv(AL_ORIENTATION, orientation);
}

void AudioEngine::loadSound(const std::string& soundFilePath)
{
   if (mSounds.find(soundFilePath) != mSounds.end())
   {
      // The requested sound has already been loaded
      return;
   }

   unsigned int numChannels = 0;
   unsigned int sampleRate = 0;
   drwav_uint64 totalPCMFrameCount = 0;
   drwav_int16* sampleData = drwav_open_file_and_read_pcm_frames_s16(soundFilePath.c_str(),
                                                                     &numChannels,
                                                                     &sampleRate,
                                                                     &totalPCMFrameCount,
                                                                     nullptr);
   if (!sampleData)
   {
      std::cerr << "Error - AudioEngine::loadSound - Failed to read wavefile" << std::endl;
      return;
   }

   drwav_uint64 numSamples = totalPCMFrameCount * numChannels;
   if (numSamples > std::numeric_limits<size_t>::max())
   {
      std::cerr << "Error - AudioEngine::loadSound - Wave file is too big to be stored in an std::vector" << std::endl;
      drwav_free(sampleData, nullptr);
   }

   std::vector<uint16_t> pcmData;
   pcmData.resize(numSamples);
   std::memcpy(pcmData.data(), sampleData, pcmData.size() * 2);

   drwav_free(sampleData, nullptr);

   unsigned int bufferID;
   alGenBuffers(1, &bufferID);
   alBufferData(bufferID, numChannels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, pcmData.data(), static_cast<ALsizei>(pcmData.size() * 2), sampleRate);

   mSounds[soundFilePath] = bufferID;
}

void AudioEngine::unloadSound(const std::string& soundFilePath)
{
   auto it = mSounds.find(soundFilePath);
   if (it == mSounds.end())
   {
      // The requested sound doesn't exist
      return;
   }

   alDeleteBuffers(1, &it->second);

   mSounds.erase(it);
}

int AudioEngine::playSound(const std::string& soundFilePath,
                           const glm::vec3& position,
                           const glm::vec3& velocity,
                           float volumedB,
                           float pitch,
                           bool relativePosition)
{
   int sourceID = mNextSourceID++;

   auto it = mSounds.find(soundFilePath);
   if (it == mSounds.end())
   {
      // The requested sound hasn't been loaded, so we try to load it
      loadSound(soundFilePath);

      it = mSounds.find(soundFilePath);
      if (it == mSounds.end())
      {
         // We failed to load the requested sound, so we exit without playing it
         return sourceID;
      }
   }

   mSources.emplace(std::piecewise_construct,
                    std::forward_as_tuple(sourceID),
                    std::forward_as_tuple(position, velocity, dBToVolume(volumedB), pitch, relativePosition));

   mSources[sourceID].play(it->second);

   return sourceID;
}

void AudioEngine::pauseSource(int sourceID)
{
   auto it = mSources.find(sourceID);
   if (it != mSources.end())
   {
      it->second.pause();
   }
}

void AudioEngine::pauseAllSources()
{
   for (auto it = mSources.begin(), itEnd = mSources.end(); it != itEnd; ++it)
   {
      it->second.pause();
   }
}

void AudioEngine::resumeSource(int sourceID)
{
   auto it = mSources.find(sourceID);
   if (it != mSources.end())
   {
      it->second.resume();
   }
}

void AudioEngine::resumeAllSources()
{
   for (auto it = mSources.begin(), itEnd = mSources.end(); it != itEnd; ++it)
   {
      it->second.resume();
   }
}

void AudioEngine::stopSource(int sourceID)
{
   auto it = mSources.find(sourceID);
   if (it != mSources.end())
   {
      it->second.stop();
   }
}

void AudioEngine::stopAllSources()
{
   for (auto it = mSources.begin(), itEnd = mSources.end(); it != itEnd; ++it)
   {
      it->second.stop();
   }
}

void AudioEngine::setSourceVolume(int sourceID, float volume)
{
   auto it = mSources.find(sourceID);
   if (it != mSources.end())
   {
      it->second.setVolume(volume);
   }
}

void AudioEngine::setSourcePitch(int sourceID, float pitch)
{
   auto it = mSources.find(sourceID);
   if (it != mSources.end())
   {
      it->second.setPitch(pitch);
   }
}

void AudioEngine::setSourcePosition(int sourceID, const glm::vec3& position)
{
   auto it = mSources.find(sourceID);
   if (it != mSources.end())
   {
      it->second.setPosition(position);
   }
}

void AudioEngine::setSourceVelocity(int sourceID, const glm::vec3& velocity)
{
   auto it = mSources.find(sourceID);
   if (it != mSources.end())
   {
      it->second.setVelocity(velocity);
   }
}

void AudioEngine::setSourceLooping(int sourceID, bool loop)
{
   auto it = mSources.find(sourceID);
   if (it != mSources.end())
   {
      it->second.setLooping(loop);
   }
}

bool AudioEngine::isPlaying(int sourceID) const
{
   auto it = mSources.find(sourceID);
   if (it != mSources.end())
   {
      return it->second.isPlaying();
   }

   return false;
}

float dBToVolume(float dB)
{
   return powf(10.0f, 0.05f * dB);
}

float volumeTodB(float volume)
{
   return 20.0f * log10f(volume);
}

float calcRandFloat(float min, float max)
{
   return min + (static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / (max - min)));
}
