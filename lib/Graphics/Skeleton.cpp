#include "Graphics/Skeleton.hpp"
#include <iostream>

namespace gintonic
{

std::ostream& operator<<(std::ostream& os, const Skeleton::Joint& joint)
{
    os << "\nJoint name: " << joint.name << '\n';
    os << "Inverse Bind Pose:\n" << joint.inverseBindPose << '\n';
    os << "Parent: ";
    if (joint.parent == GT_JOINT_NONE)
        os << "null\n";
    else
        os << static_cast<std::size_t>(joint.parent) << '\n';
    os << "Children: [ ";
    for (std::size_t c = 0; c < GT_JOINT_MAX_CHILDREN; ++c)
    {
        if (joint.child[c] == GT_JOINT_NONE)
            os << "null ";
        else
            os << static_cast<std::size_t>(joint.child[c]) << ' ';
    }
    return os << ']';
}

std::ostream& operator<<(std::ostream& os, const Skeleton& skeleton)
{
    os << "\nSkeleton name: " << skeleton.name << "\nJoints:\n";
    for (const auto& lJoint : skeleton.joints) os << lJoint << '\n';
    return os;
}

} // namespace gintonic
