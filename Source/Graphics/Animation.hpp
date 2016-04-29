
#pragma once

#include "../Foundation/allocator.hpp"

#include "../Math/vec3f.hpp"
#include "../Math/quatf.hpp"
#include "../Math/SQT.hpp"

#include <vector>
#include <string>

namespace gintonic {

template <class T> struct AnimationKey
{
	T value;
	float time;

	template <class S>
	inline bool operator < (const AnimationKey<S>& other) const noexcept
	{
		return time < other.time;
	}

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
};

template <class T> struct AnimationCurve
{
	std::vector<AnimationKey<T>, allocator<AnimationKey<T>>> AnimationKeys;

	inline void addAnimationKey(const AnimationKey<T>& key)
	{
		AnimationKeys.insert(std::lower_bound(AnimationKeys.begin(), AnimationKeys.end(), key), key);
	}

	inline void addAnimationKey(AnimationKey<T>&& key)
	{
		AnimationKeys.insert(std::lower_bound(AnimationKeys.begin(), AnimationKeys.end(), key), std::move(key));
	}

	T operator()(const float timepoint)
	{
		AnimationKey<T> lDummy;
		lDummy.time = timepoint;
		const auto lResult = std::lower_bound(AnimationKeys.begin(), AnimationKeys.end(), lDummy);
		const auto lNext = lResult + 1;
		if (lNext == AnimationKeys.end())
		{

		}
		else
		{

		}
		return lDummy.value;
	}
};

// template <class Type>
// struct Property
// {
// 	Type value;
// 	AnimationCurve<Type> curve;

// 	inline Type evaluate(const float timepoint) const noexcept
// 	{
// 		return curve(timepoint);
// 	};

// 	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
// 	template <class Archive>
// 	void serialize(Archive& ar, const unsigned int /*version*/)
// 	{
// 		ar & value & curve;
// 	}
// };

// class Material
// {
// 	std::string name;
// 	struct ColorChannel
// 	{
// 		Property<vec4f> color;
// 		std::shared_ptr<Texture2D> texture;
// 		GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
// 		template <class Archive>
// 		void serialize(Archive& ar, const unsigned int /*version*/)
// 		{
// 			ar & color & texture;
// 		}
// 	};

// 	ColorChannel diffuse;
// 	ColorChannel specular;
// 	ColorChannel emissive;
// 	ColorChannel normal;

// 	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
// 	template <class Archive>
// 	void serialize(Archive& ar, const unsigned int /*version*/)
// 	{
// 		ar & name & diffuse & specular & emissive & normal;
// 	}
// };

// struct Light
// {
// 	enum Type
// 	{
// 		kAmbient = 0,
// 		kDirectional,
// 		kPoint,
// 		kSpot
// 	};

// 	std::string name;
	
// 	Type type;

// 	Property<vec4f> intensity;
// 	Property<vec4f> intensity;
// 	Property<vec4f> attenuation;
// 	Property<vec4f> position;
// 	Property<vec4f> direction;
// 	Property<float> cosineHalfAngle;

// 	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

// 	template <class Archive>
// 	void serialize(Archive& ar, const unsigned int /*version*/)
// 	{
// 		ar & name & type & intensity & attenuation & position & direction & cosineHalfAngle;
// 	}
// };

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