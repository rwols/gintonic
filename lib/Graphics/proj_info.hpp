/**
 * @file proj_info.hpp
 * @brief Defines the projection info component.
 * @author Raoul Wols
 */

#pragma once

#include "../Math/mat4f.hpp"
#include "../Math/vec2f.hpp"

#include "../component.hpp"

namespace gintonic {

/**
 * @brief Projection information component.
 * @details The projection information component is simply there to give the
 * renderer information on what type of projection matrix to use for the given
 * camera entity.
 */
class proj_info : public std::enable_shared_from_this<proj_info>
{
public:

	// /// Default constructor.
	// camera() = default;

	// /// Destructor.
	// virtual ~camera();

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
	float width;
	float height;
	float nearplane;
	float farplane;

	/// The current projection matrix.
	mat4f projection_matrix;

	vec2f angles;

	void add_mouse(const vec2f& mouse_angles) noexcept;

	// virtual void attach(entity&) final;

	// virtual void detach(entity&) final;

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
