#ifndef gintonic_camera_hpp
#define gintonic_camera_hpp

#include "component.hpp"
#include "vec2f.hpp"

namespace gintonic {

class camera : public component
{
public:

	camera() = default;
	camera(entity&);
	virtual ~camera();

	vec2f angles;

	void add_mouse(const vec2f& angles) BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void attach(entity&) final;
	virtual void detach(entity&) final;

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
};

} // namespace gintonic

#endif