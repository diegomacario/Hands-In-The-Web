#ifndef CROSSFADE_CONTROLLER_MULTIPLE_H
#define CROSSFADE_CONTROLLER_MULTIPLE_H

#include "Skeleton.h"
#include "CrossFadeTarget.h"

template <typename CLIP>
class TCrossFadeControllerMultiple
{
public:

   TCrossFadeControllerMultiple();
   TCrossFadeControllerMultiple(Skeleton& skeleton);

   void SetSkeleton(Skeleton& skeleton);

   void Play(CLIP* clip, bool lock, float playbackSpeed = 1.0f);
   void FadeTo(CLIP* targetClip, float fadeDuration, bool lock, float playbackSpeed = 1.0f);
   void Update(float dt);
   void ClearTargets();

   CLIP* GetCurrentClip();
   Pose& GetCurrentPose();

   bool  IsCurrentClipFinished();
   bool  IsLocked();
   void  Unlock();

   float GetPlaybackTime();

private:

   CLIP*                               mCurrentClip;
   float                               mPlaybackTime;
   Skeleton                            mSkeleton;
   Pose                                mCurrentPose;
   bool                                mWasSkeletonSet;
   bool                                mLock;
   float                               mPlaybackSpeed;

   std::vector<TCrossFadeTarget<CLIP>> mTargets;
};

typedef TCrossFadeControllerMultiple<Clip> CrossFadeControllerMultiple;
typedef TCrossFadeControllerMultiple<FastClip> FastCrossFadeControllerMultiple;

#endif
