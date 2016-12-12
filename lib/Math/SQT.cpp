#include "Math/SQT.hpp"
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

	// BIG HACK!
	FbxNode* lMutable = const_cast<FbxNode*>(pFbxNode);
	const FbxAMatrix& lMatrix = lMutable->EvaluateLocalTransform();
	scale = lMatrix.GetS();
	rotation = lMatrix.GetQ();
	translation = lMatrix.GetT();
}

SQT::SQT(const FbxAMatrix& affineMatrix)
: scale(affineMatrix.GetS())
, rotation(affineMatrix.GetQ())
, translation(affineMatrix.GetT())
{
	GT_PROFILE_FUNCTION;
}

SQT& SQT::operator = (const FbxAMatrix& affineMatrix)
{
	GT_PROFILE_FUNCTION;

	scale = affineMatrix.GetS();
	rotation = affineMatrix.GetQ();
	translation = affineMatrix.GetT();

	return *this;
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

SQT mix(const SQT& u, const SQT& v, const float a) noexcept
{
	GT_PROFILE_FUNCTION;

	return SQT
	(
		mix(u.scale, v.scale, a), 
		// u.rotation,
		// mix(u.rotation, v.rotation, a), 
		slerp(u.rotation, v.rotation, a),
		mix(u.translation, v.translation, a)
	);
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