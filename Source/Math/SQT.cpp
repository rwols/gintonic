#include "SQT.hpp"
#include <fbxsdk.h>

namespace gintonic {

SQT::SQT(const mat4f& affineMatrix)
{
	GT_PROFILE_FUNCTION;

	affineMatrix.decompose(*this);
}

SQT::SQT(const FbxNode* pFbxNode)
{
	GT_PROFILE_FUNCTION;

	auto lValue = pFbxNode->LclScaling.Get();
	scale.x = static_cast<float>(lValue[0]);
	scale.y = static_cast<float>(lValue[1]);
	scale.z = static_cast<float>(lValue[2]);

	lValue = pFbxNode->LclRotation.Get();
	rotation = quatf::yaw_pitch_roll(
		static_cast<float>(lValue[0]), 
		static_cast<float>(lValue[1]), 
		static_cast<float>(lValue[2]));

	lValue = pFbxNode->LclTranslation.Get();
	translation.x = static_cast<float>(lValue[0]);
	translation.y = static_cast<float>(lValue[1]);
	translation.z = static_cast<float>(lValue[2]);
}

SQT::SQT(const FbxAMatrix& affineMatrix)
: scale(affineMatrix.GetS())
, rotation(affineMatrix.GetQ())
, translation(affineMatrix.GetT())
{
	GT_PROFILE_FUNCTION;
}

SQT SQT::operator % (const SQT& other) const noexcept
{
	GT_PROFILE_FUNCTION;

	return SQT(
		scale * other.scale, 
		rotation * other.rotation, 
		translation + other.translation);
}

SQT& SQT::operator %= (const SQT& other) noexcept
{
	GT_PROFILE_FUNCTION;

	scale *= other.scale;
	rotation *= other.rotation;
	translation += other.translation;
	return *this;
}

SQT SQT::inverse() const noexcept
{
	GT_PROFILE_FUNCTION;

	return SQT(
		vec3f(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z),
		rotation.conjugate(),
		-translation);
}

SQT& SQT::invert() noexcept
{
	GT_PROFILE_FUNCTION;

	scale.x = 1.0f / scale.x;
	scale.y = 1.0f / scale.y;
	scale.z = 1.0f / scale.z;
	rotation = rotation.conjugate();
	translation *= -1.0f;
	return *this;
}

std::ostream& operator << (std::ostream& os, const SQT& sqt)
{
	GT_PROFILE_FUNCTION;

	os << "Scale: " << sqt.scale 
		<< ", Rotation: " << sqt.rotation 
		<< ", Translation: " << sqt.translation;
	return os;
}

std::istream& operator >> (std::istream& is, SQT& sqt)
{
	GT_PROFILE_FUNCTION;
	
	is >> sqt.scale >> sqt.rotation >> sqt.translation;
	return is;
}

} // namespace gintonic