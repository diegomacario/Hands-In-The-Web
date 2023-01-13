#ifndef BLENDING_H
#define BLENDING_H

#include "Pose.h"
#include "Skeleton.h"
#include "Clip.h"

bool IsJointInHierarchy(const Pose& pose, unsigned int parentJointIndex, unsigned int potentialChildJointIndex);
void Blend(const Pose& a, const Pose& b, float t, int blendRoot, Pose& outBlendedPose);

Pose GetAdditiveBasePoseFromAdditiveClip(Skeleton& skeleton, const Clip& additiveClip);
void AdditiveBlend(const Pose& animatedPose, const Pose& additivePose, const Pose& additiveBasePose, int blendRoot, Pose& outBlendedPose);

#endif
