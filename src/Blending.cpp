#include "Blending.h"

bool IsJointInHierarchy(const Pose& pose, unsigned int parentJointIndex, unsigned int potentialChildJointIndex)
{
   // If the child and the parent are the same, then the child is considered to be in the hierarchy of the parent
   if (potentialChildJointIndex == parentJointIndex)
   {
      return true;
   }

   // Loop over the chain of parents of the potential child until we either find the right parent or we reach the root
   int currParentJointIndex = pose.GetParent(potentialChildJointIndex);
   while (currParentJointIndex >= 0)
   {
      if (currParentJointIndex == static_cast<int>(parentJointIndex))
      {
         return true;
      }

      currParentJointIndex = pose.GetParent(currParentJointIndex);
   }

   return false;
}

void Blend(const Pose& a, const Pose& b, float t, int blendRoot, Pose& outBlendedPose)
{
   unsigned int numJoints = outBlendedPose.GetNumberOfJoints();

   // When the user wants to blend all the joints of the two poses, they pass a negative blendRoot (typically -1)
   // If that's the case, we don't need to perform a hierarchy check while looping over the joints
   if (blendRoot < 0)
   {
      for (unsigned int jointIndex = 0; jointIndex < numJoints; ++jointIndex)
      {
         // Blend the local transforms of the two joints and store the result in the output pose
         outBlendedPose.SetLocalTransform(jointIndex, mix(a.GetLocalTransform(jointIndex), b.GetLocalTransform(jointIndex), t));
      }
   }
   else
   {
      for (unsigned int jointIndex = 0; jointIndex < numJoints; ++jointIndex)
      {
         // If the current joint is not in the hierarchy of the blendRoot, we don't blend it
         if (!IsJointInHierarchy(outBlendedPose, static_cast<unsigned int>(blendRoot), jointIndex))
         {
            continue;
         }

         // Blend the local transforms of the two joints and store the result in the output pose
         outBlendedPose.SetLocalTransform(jointIndex, mix(a.GetLocalTransform(jointIndex), b.GetLocalTransform(jointIndex), t));
      }
   }
}

// An additive animation is used to modify other animations by adding additional joint movements
// An example of an additive animation is an animation that causes a character to lean left by bending its spine
// Such an animation can be added to a walk or run animation so that a character appears to be changing its direction,
// and its playback can be controlled not by time but by input from the user (e.g. by the position of a joystick)

// This function samples an additive clip at its start time to get an additive base pose,
// which is a pose that's used as a reference when performing additive blending
Pose GetAdditiveBasePoseFromAdditiveClip(Skeleton& skeleton, const Clip& additiveClip)
{
   Pose additiveBasePose = skeleton.GetRestPose();
   additiveClip.Sample(additiveBasePose, additiveClip.GetStartTime());
   return additiveBasePose;
}

// The additive blending formula is the following:
// outBlendedPose = animatedPose + (additivePose - additiveBasePose)
// By subtracting the additiveBasePose from the additivePose, we ensure that we only add the delta between the two to the animatedPose
// To understand why that works, one needs to remember that the equation above is evaluated with local transforms
// So if only one joint in the spine is changing to make a character lean left, then when we subtract the additiveBasePose from the additivePose,
// all the joints become "unit" joints except for the one that is changing
// Note that the additive blending formula can be used directly when blending positions and scales,
// but to blend rotations it must be adapted to work with quaternions as follows:
// - To calculate (additivePose - additiveBasePose) we need to ask ourselves:
//   What is the local or world rotation that can take us from additiveBasePose to additivePose?
//   Both approaches (local and world) are equally valid
//   Let's say that we decide to go for the local approach. In that case, the question above can be expressed and solved like this:
//
//   additiveBasePose * diffLocal = additivePose
//
//   diffLocal = additiveBasePose^-1 * additivePose
//
// - Note that in the equations above we are assuming that p * q isn't reversed, which is why diffLocal is multiplied from the right so that it's local
// - Finally, the diffLocal rotation can be combined with the animatedPose rotation by multiplying it from the right, since diffLocal is a local rotation:
//
//   outBlendedPose = animatedPose * (additiveBasePose^-1 * additivePose)
//
// - As a reference, here is what the world approach looks like:
//
//   diffWorld * additiveBasePose = additivePose
//
//   diffWorld = additivePose * additiveBasePose^-1
//
//   outBlendedPose = (additivePose * additiveBasePose^-1) * animatedPose
//
// This function implements the equation that results from the world approach
void AdditiveBlend(const Pose& animatedPose, const Pose& additivePose, const Pose& additiveBasePose, int blendRoot, Pose& outBlendedPose)
{
   unsigned int numJoints = additivePose.GetNumberOfJoints();

   // When the user wants to blend all the joints of the two poses, they pass a negative blendRoot (typically -1)
   // If that's the case, we don't need to perform a hierarchy check while looping over the joints
   if (blendRoot < 0)
   {
      for (unsigned int jointIndex = 0; jointIndex < numJoints; ++jointIndex)
      {
         Transform animated     = animatedPose.GetLocalTransform(jointIndex);
         Transform additive     = additivePose.GetLocalTransform(jointIndex);
         Transform additiveBase = additiveBasePose.GetLocalTransform(jointIndex);

         // Combine the position, rotation and scale of the transforms using the additive blending formula:
         // outBlendedPose = animatedPose + (additivePose - additiveBasePose)
         glm::vec3 position = animated.position + (additive.position - additiveBase.position);
         glm::vec3 scale = animated.scale + (additive.scale - additiveBase.scale);
         // NOTE: Reversed because q * p is implemented as p * q
         Q::quat rotation = normalized(animated.rotation * (Q::inverse(additiveBase.rotation) * additive.rotation));

         outBlendedPose.SetLocalTransform(jointIndex, Transform(position, rotation, scale));
      }
   }
   else
   {
      for (unsigned int jointIndex = 0; jointIndex < numJoints; ++jointIndex)
      {
         // If the current joint is not in the hierarchy of the blendRoot, we don't blend it
         if (!IsJointInHierarchy(additivePose, blendRoot, jointIndex))
         {
            continue;
         }

         Transform animated     = animatedPose.GetLocalTransform(jointIndex);
         Transform additive     = additivePose.GetLocalTransform(jointIndex);
         Transform additiveBase = additiveBasePose.GetLocalTransform(jointIndex);

         // Combine the position, rotation and scale of the transforms using the additive blending formula:
         // outBlendedPose = animatedPose + (additivePose - additiveBasePose)
         glm::vec3 position = animated.position + (additive.position - additiveBase.position);
         glm::vec3 scale = animated.scale + (additive.scale - additiveBase.scale);
         // NOTE: Reversed because q * p is implemented as p * q
         Q::quat rotation = normalized(animated.rotation * (Q::inverse(additiveBase.rotation) * additive.rotation));

         outBlendedPose.SetLocalTransform(jointIndex, Transform(position, rotation, scale));
      }
   }
}
