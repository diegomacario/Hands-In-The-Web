#ifndef CROSSFADE_TARGET_H
#define CROSSFADE_TARGET_H

#include "Clip.h"
#include "Pose.h"

template <typename CLIP>
struct TCrossFadeTarget
{
   TCrossFadeTarget();
   TCrossFadeTarget(CLIP* clip, Pose& pose, float fadeDuration, bool lock, float playbackSpeed);

   CLIP* mClip;
   Pose  mPose;
   float mPlaybackTime;
   float mFadeDuration;
   float mFadeTime;
   bool  mLock;
   float mPlaybackSpeed;
};

typedef TCrossFadeTarget<Clip> CrossFadeTarget;
typedef TCrossFadeTarget<FastClip> FastCrossFadeTarget;

#endif
