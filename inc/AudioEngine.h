#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include <string>
#include <map>

#include "glm/glm.hpp"
#include "AL/alc.h"

#include "AudioSource.h"

class AudioEngine
{
public:

   AudioEngine();
   ~AudioEngine();

   AudioEngine(const AudioEngine&) = delete;
   AudioEngine& operator=(const AudioEngine&) = delete;

   AudioEngine(AudioEngine&& rhs) = delete;
   AudioEngine& operator=(AudioEngine&& rhs) = delete;

   bool         initialize();
   void         update();

   void         setListenerData(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& at, const glm::vec3& up);

   void         loadSound(const std::string& soundFilePath);
   void         unloadSound(const std::string& soundFilePath);

   int          playSound(const std::string& soundFilePath,
                          const glm::vec3& position,
                          const glm::vec3& velocity,
                          float volumedB = 0.0f,
                          float pitch = 1.0f,
                          bool  relativePosition = false);
   void         pauseSource(int sourceID);
   void         pauseAllSources();
   void         resumeSource(int sourceID);
   void         resumeAllSources();
   void         stopSource(int sourceID);
   void         stopAllSources();

   void         setSourceVolume(int sourceID, float volume);
   void         setSourcePitch(int sourceID, float pitch);
   void         setSourcePosition(int sourceID, const glm::vec3& position);
   void         setSourceVelocity(int sourceID, const glm::vec3& velocity);
   void         setSourceLooping(int sourceID, bool loop);

   bool         isPlaying(int sourceID) const;

private:

   ALCcontext* mContext;

   int mNextSourceID;

   typedef std::map<std::string, unsigned int> SoundMap;
   typedef std::map<int, AudioSource>          SourceMap;

   SoundMap  mSounds;
   SourceMap mSources;
};

float dBToVolume(float dB);
float volumeTodB(float volume);
float calcRandFloat(float min, float max);

#endif
