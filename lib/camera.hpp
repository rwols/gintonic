/**
 * @file camera.hpp
 * @author Raoul Wols
 */

#ifndef gintonic_camera_hpp
#define gintonic_camera_hpp

#include "component.hpp"
#include "vec2f.hpp"

namespace gintonic {

/**
 * @brief Camera component class.
 */
class camera : public component
{
public:

	/// Default constructor.
	camera() = default;

	/**
	 * @brief Constructor.
	 * @param e The entity to attach the camera to.
	 */
	camera(entity& e);

	/// Destructor.
	virtual ~camera();

	/// The mouse angles.
	vec2f angles;

	/**
	 * @brief Add a mouse delta to the angles.
	 * 
	 * @param angles The mouse delta.
	 */
	void add_mouse(const vec2f& angles) BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void attach(entity&) final;
	virtual void detach(entity&) final;

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond
};

} // namespace gintonic

#endif