#include "CrossFadeControllerMultiple.h"
#include "Blending.h"

template <typename CLIP>
TCrossFadeControllerMultiple<CLIP>::TCrossFadeControllerMultiple()
   : mCurrentClip(nullptr)
   , mPlaybackTime(0.0f)
   , mSkeleton()
   , mCurrentPose()
   , mWasSkeletonSet(false)
   , mLock(false)
   , mPlaybackSpeed(1.0f)
   , mTargets()
{

}

template <typename CLIP>
TCrossFadeControllerMultiple<CLIP>::TCrossFadeControllerMultiple(Skeleton& skeleton)
   : mCurrentClip(nullptr)
   , mPlaybackTime(0.0f)
   , mSkeleton()
   , mCurrentPose()
   , mWasSkeletonSet(false)
   , mLock(false)
   , mPlaybackSpeed(1.0f)
   , mTargets()
{
   SetSkeleton(skeleton);
}

template <typename CLIP>
void TCrossFadeControllerMultiple<CLIP>::SetSkeleton(Skeleton& skeleton)
{
   mSkeleton       = skeleton;
   mCurrentPose    = mSkeleton.GetRestPose();
   mWasSkeletonSet = true;
}

template <typename CLIP>
void TCrossFadeControllerMultiple<CLIP>::Play(CLIP* clip, bool lock, float playbackSpeed)
{
   // When asked to play a clip, we clear all the crossfade targets
   mTargets.clear();

   mCurrentClip   = clip;
   mPlaybackTime  = clip->GetStartTime();
   mCurrentPose   = mSkeleton.GetRestPose();
   mLock          = lock;
   mPlaybackSpeed = playbackSpeed;
}

template <typename CLIP>
void TCrossFadeControllerMultiple<CLIP>::FadeTo(CLIP* targetClip, float fadeDuration, bool lock, float playbackSpeed)
{
   if (mLock)
   {
      return;
   }

   // If no clip has been set, simply play the target clip since there is no clip to fade from
   if (mCurrentClip == nullptr)
   {
      Play(targetClip, lock, playbackSpeed);
      return;
   }

   if (mTargets.size() >= 1)
   {
      // If the last clip in the queue is the same as the target clip, then don't add the target clip to the queue
      // Otherwise, we would eventually fade between identical clips
      if (mTargets[mTargets.size() - 1].mClip == targetClip)
      {
         return;
      }
   }
   else
   {
      // If there are no clips in the queue and the current clip is the same as the target clip,
      // then don't add the target clip to the queue unless the current clip is already finished
      if (mCurrentClip == targetClip)
      {
         if (mCurrentClip->IsTimePastEnd(mPlaybackTime))
         {
            mTargets.emplace_back(targetClip, mSkeleton.GetRestPose(), fadeDuration, lock, playbackSpeed);
         }

         return;
      }
   }

   // Add the target clip to the queue
   mTargets.emplace_back(targetClip, mSkeleton.GetRestPose(), fadeDuration, lock, playbackSpeed);
}

template <typename CLIP>
void TCrossFadeControllerMultiple<CLIP>::Update(float dt)
{
   // We cannot update without a current clip or a skeleton
   if (mCurrentClip == nullptr || !mWasSkeletonSet)
   {
      return;
   }

   if (mLock)
   {
      mPlaybackTime = mCurrentClip->Sample(mCurrentPose, mPlaybackTime + (dt * mPlaybackSpeed));
   }
   else
   {
      unsigned int numTargets = static_cast<unsigned int>(mTargets.size());
      for (unsigned int targetIndex = 0; targetIndex < numTargets; ++targetIndex)
      {
         if (mTargets[targetIndex].mFadeTime >= mTargets[targetIndex].mFadeDuration)
         {
            mCurrentClip   = mTargets[targetIndex].mClip;
            mPlaybackTime  = mTargets[targetIndex].mPlaybackTime;
            mCurrentPose   = mTargets[targetIndex].mPose;
            mLock          = mTargets[targetIndex].mLock;
            mPlaybackSpeed = mTargets[targetIndex].mPlaybackSpeed;

            mTargets.erase(mTargets.begin() + targetIndex);

            // If the new clip locks the crossfade controller, we sample it and return immediately to avoid blending it with the targets
            if (mLock)
            {
               mPlaybackTime = mCurrentClip->Sample(mCurrentPose, mPlaybackTime + (dt * mPlaybackSpeed));
               return;
            }

            break;
         }
      }

      mPlaybackTime = mCurrentClip->Sample(mCurrentPose, mPlaybackTime + (dt * mPlaybackSpeed));

      numTargets = static_cast<unsigned int>(mTargets.size());
      for (unsigned int targetIndex = 0; targetIndex < numTargets; ++targetIndex)
      {
         TCrossFadeTarget<CLIP>& target = mTargets[targetIndex];
         target.mPlaybackTime = target.mClip->Sample(target.mPose, target.mPlaybackTime + (dt * target.mPlaybackSpeed));
         target.mFadeTime += (dt * target.mPlaybackSpeed);
         float t = target.mFadeTime / target.mFadeDuration;
         if (t > 1.0f)
         {
            t = 1.0f;
         }

         Blend(mCurrentPose, target.mPose, t, -1, mCurrentPose);
      }
   }
}

template <typename CLIP>
void TCrossFadeControllerMultiple<CLIP>::ClearTargets()
{
   mTargets.clear();
}

template <typename CLIP>
CLIP* TCrossFadeControllerMultiple<CLIP>::GetCurrentClip()
{
   return mCurrentClip;
}

template <typename CLIP>
Pose& TCrossFadeControllerMultiple<CLIP>::GetCurrentPose()
{
   return mCurrentPose;
}

template <typename CLIP>
bool TCrossFadeControllerMultiple<CLIP>::IsCurrentClipFinished()
{
   return mCurrentClip->IsTimePastEnd(mPlaybackTime);
}

template <typename CLIP>
bool TCrossFadeControllerMultiple<CLIP>::IsLocked()
{
   return mLock;
}

template <typename CLIP>
void TCrossFadeControllerMultiple<CLIP>::Unlock()
{
   mLock = false;
}

template <typename CLIP>
float TCrossFadeControllerMultiple<CLIP>::GetPlaybackTime()
{
   return mPlaybackTime;
}

// Instantiate the desired CrossFadeControllerMultiple classes from the CrossFadeControllerMultiple class template
template class TCrossFadeControllerMultiple<Clip>;
template class TCrossFadeControllerMultiple<FastClip>;
