#include "vec2f.hpp"
#define FBXSDK_NEW_API
#include <fbxsdk/core/math/fbxvector2.h>

namespace gintonic {

vec2f::vec2f(const GINTONIC_NAMESPACE_FBX::FbxVector2& v) BOOST_NOEXCEPT_OR_NOTHROW
: x(static_cast<float>(v[0]))
, y(static_cast<float>(v[1]))
{
	/* Empty on purpose. */
}

vec2f& vec2f::operator=(const GINTONIC_NAMESPACE_FBX::FbxVector2& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	x = static_cast<float>(v[0]);
	y = static_cast<float>(v[1]);
	return *this;
}

vec2f::operator GINTONIC_NAMESPACE_FBX::FbxVector2() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return GINTONIC_NAMESPACE_FBX::FbxVector2(x, y);
}

} // namespace gintonic