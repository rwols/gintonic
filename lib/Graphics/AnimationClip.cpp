#include "Graphics/AnimationClip.hpp"
#include "Graphics/Skeleton.hpp"
#include <algorithm>

namespace gintonic
{

mat4f AnimationClip::evaluate(const uint8_t jointIndex, const float startTime,
                              const float currentTime) const noexcept
{
    float lExactFrame;
    std::size_t lLowerFrame;
    std::size_t lUpperFrame;

#define FRAMECOUNTi frames[jointIndex].size()
#define FRAMECOUNTf static_cast<float>(FRAMECOUNTi)
#define FRAMES_PER_SECOND 24.0f

    if (isLooping)
    {
        lExactFrame = std::fmod(FRAMES_PER_SECOND * (currentTime - startTime),
                                FRAMECOUNTf);
        lLowerFrame = static_cast<std::size_t>(lExactFrame);
        lUpperFrame = (lLowerFrame + 1) % FRAMECOUNTi;
    }
    else
    {
        lExactFrame = std::min(FRAMES_PER_SECOND * (currentTime - startTime),
                               FRAMECOUNTf - 1.0f);
        lLowerFrame = static_cast<std::size_t>(lExactFrame);
        lUpperFrame = std::min(lLowerFrame + 1,
                               static_cast<std::size_t>(FRAMECOUNTi - 1));
    }

    mat4f lResult(skeleton->joints[jointIndex].inverseBindPose);
    const float lLambda = lExactFrame - static_cast<float>(lLowerFrame);
    uint8_t j = jointIndex;
    while (j != GT_JOINT_NONE)
    {
        const auto lCurrent =
            mat4f(mix(frames[j][lLowerFrame], frames[j][lUpperFrame], lLambda));
        // const auto lCurrent = mat4f(frames[j][lLowerFrame]);
        lResult = lCurrent * lResult;
        j = skeleton->joints[j].parent;
    }
    // return skeleton->joints[jointIndex].inverseBindPose * lResult;
    // return lResult * skeleton->joints[jointIndex].inverseBindPose;
    return lResult;
}

} // namespace gintonic
