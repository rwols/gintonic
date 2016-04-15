
#pragma once

#include "../Foundation/allocator.hpp"

#include "../Math/vec3f.hpp"
#include "../Math/quatf.hpp"
#include "../Math/SQT.hpp"

#include <vector>
#include <string>

namespace gintonic {

struct AnimKey
{
	float scale;
	quatf rotation;
	vec3f translation;
	float time;

	inline bool operator < (const AnimKey& other) const noexcept
	{
		return time < other.time;
	}

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
};

struct AnimCurve
{
	std::vector<AnimKey, allocator<AnimKey>> animKeys;

	inline void addAnimKey(const AnimKey& key)
	{
		animKeys.insert(std::lower_bound(animKeys.begin(), animKeys.end(), key), key);
	}

	inline void addAnimKey(AnimKey&& key)
	{
		animKeys.insert(std::lower_bound(animKeys.begin(), animKeys.end(), key), std::move(key));
	}

	inline SQT operator()(const float t)
	{
		AnimKey dummy;
		dummy.time = t;
		auto result = std::lower_bound(animKeys.begin(), animKeys.end(), dummy);
		result = (result == animKeys.end()) ? std::prev(result) : result;

		return SQT(result->scale, result->rotation, result->translation);
	}
};

struct AnimCurveNode
{

};

struct AnimLayer
{
	float weight;
	std::vector<AnimCurveNode> animCurveNodes;
};

struct AnimStack
{
	std::string description;
	std::vector<AnimLayer> animLayers;
};

} // namespace gintonic