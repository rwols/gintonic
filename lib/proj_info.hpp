/**
 * @file proj_info.hpp
 * @brief Defines the projection info component.
 * @author Raoul Wols
 */

#ifndef gintonic_proj_info_hpp
#define gintonic_proj_info_hpp

#include "component.hpp"
#include "mat4f.hpp"

namespace gintonic {

/**
 * @brief Projection information component.
 * @details The projection information component is simply there to give the
 * renderer information on what type of projection matrix to use for the given
 * camera entity.
 */
class proj_info : public component
{
public:

	/// Default constructor.
	proj_info() = default;

	/// Destructor.
	virtual ~proj_info();

	/// Projection type. Either orthographic or perspective.
	enum projection_type
	{
		kOrthographicProjection,
		kPerspectiveProjection
	};

	/// The projection type.
	projection_type projection = kPerspectiveProjection;

	/// The field of view (FOV).
	float fieldofview = 1.22173048f;

	/// The current projection matrix.
	mat4f matrix;

	virtual void attach(entity&) final;

	virtual void detach(entity&) final;

	/**
	 * @brief Update this projection info component.
	 * @details The projection info component updates itself given the
	 * renderer's current viewport width and viewport height.
	 */
	void update() noexcept;

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond
};

} // namespace gintonic

#endif