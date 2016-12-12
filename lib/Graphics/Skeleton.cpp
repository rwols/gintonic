#include "Graphics/Skeleton.hpp"
#include <iostream>
#include <fbxsdk.h>

namespace gintonic {


std::ostream& operator << (std::ostream& os, const Skeleton::Joint& joint)
{
	os << "\nJoint name: " << joint.name << '\n';
	os << "Inverse Bind Pose:\n" << joint.inverseBindPose << '\n';
	os << "Parent: ";
	if (joint.parent == GT_JOINT_NONE) os << "null\n";
	else os << static_cast<std::size_t>(joint.parent) << '\n';
	os << "Children: [ ";
	for (std::size_t c = 0; c < GT_JOINT_MAX_CHILDREN; ++c)
	{
		if (joint.child[c] == GT_JOINT_NONE) os << "null ";
		else os << static_cast<std::size_t>(joint.child[c]) << ' ';
	}
	return os << ']';
}

Skeleton::Joint::Joint(const FbxNode* pFbxNode, const uint8_t parentIndex)
: name(pFbxNode->GetName())
, inverseBindPose(1.0f)
, parent(parentIndex)
{
	for (std::size_t c = 0; c < GT_JOINT_MAX_CHILDREN; ++c)
	{
		child[c] = GT_JOINT_NONE;
	}
}

Skeleton::Skeleton(const FbxScene* pFbxScene) : Skeleton(pFbxScene->GetRootNode()) {}

Skeleton::Skeleton(const FbxNode* pFbxNode)
: name(pFbxNode->GetName())
{
	constructJointsRecursive(pFbxNode, GT_JOINT_NONE);
}

std::ostream& operator << (std::ostream& os, const Skeleton& skeleton)
{
	os << "\nSkeleton name: " << skeleton.name << "\nJoints:\n";
	for (const auto& lJoint : skeleton.joints) os << lJoint << '\n';
	return os;
}

void Skeleton::constructJointsRecursive(const FbxNode* pNode, const uint8_t parentIndex)
{
	uint8_t lNewJointIndex = GT_JOINT_NONE;
	if (pNode->GetNodeAttribute() &&
		pNode->GetNodeAttribute()->GetAttributeType() &&
		pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		if (joints.size() >= GT_JOINT_NONE)
		{
			throw TooManyJointsException(pNode);
		}
		lNewJointIndex = static_cast<uint8_t>(joints.size());
		Joint lNewJoint(pNode, parentIndex);
		if (parentIndex != GT_JOINT_NONE)
		{
			bool lTooManyChildren = true;
			for (std::size_t c = 0; c < GT_JOINT_MAX_CHILDREN; ++c)
			{
				if (joints[parentIndex].child[c] == GT_JOINT_NONE)
				{
					joints[parentIndex].child[c] = lNewJointIndex;
					lTooManyChildren = false;
					break;
				}
			}
			if (lTooManyChildren)
			{
				throw TooManyChildJointsException(pNode);
			}
		}
		// jointNameToIndexMap[lNewJoint.name] = lNewJointIndex;
		joints.push_back(lNewJoint);
	}
	for (int i = 0; i < pNode->GetChildCount(); ++i)
	{
		constructJointsRecursive(pNode->GetChild(i), lNewJointIndex);
	}
}

} // namespace gintonic
