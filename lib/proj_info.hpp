#ifndef gintonic_proj_info_hpp
#define gintonic_proj_info_hpp

#include "component.hpp"

namespace gintonic {

class proj_info : public component
{
public:

	proj_info() = default;
	virtual ~proj_info();

	enum projection_type
	{
		kOrthographicProjection,
		kPerspectiveProjection
	};

	projection_type projection = kPerspectiveProjection;

	float fieldofview = 1.22173048f;

	mat4f matrix;

	virtual void attach(entity&) final;
	virtual void detach(entity&) final;

	void update() BOOST_NOEXCEPT_OR_NOTHROW;

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
};

} // namespace gintonic

#endif