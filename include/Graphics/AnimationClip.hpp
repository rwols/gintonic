#pragma once

#include "../ForwardDeclarations.hpp"

#include "../Foundation/allocator.hpp"

#include "../Math/SQT.hpp"

#include <boost/serialization/access.hpp>
#include <boost/serialization/tracking.hpp>

#include <vector>

namespace gintonic
{

class AnimationClip
{
  public:
    class BakeLayersException : public std::exception
    {
      public:
        BakeLayersException() = delete;
        virtual ~BakeLayersException() noexcept = default;
        inline const char* what() const noexcept
        {
            return "BakeLayersException";
        }

      private:
        friend class AnimationClip;
    };

    std::string name;

    std::shared_ptr<Skeleton> skeleton;

    float framesPerSecond = 30.0f;

    bool isLooping = true;

    std::vector<std::vector<SQT, allocator<SQT>>> frames;

    AnimationClip() = default;

    inline uint8_t jointCount() const noexcept
    {
        return static_cast<uint8_t>(frames.size());
    }

    mat4f evaluate(const uint8_t jointIndex, const float startTime,
                   const float currentTime) const noexcept;

  private:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& archive, const unsigned /*version*/)
    {
        archive& name& skeleton& framesPerSecond& isLooping& frames;
    }
};

} // namespace gintonic

BOOST_CLASS_TRACKING(gintonic::AnimationClip,
                     boost::serialization::track_always);
