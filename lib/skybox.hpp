/**
 * @file skybox.hpp
 * @brief Defines a skybox class.
 * @author Raoul Wols
 */

#ifndef gintonic_skybox_hpp
#define gintonic_skybox_hpp

#include "opengl/textures.hpp"

namespace gintonic {

/**
 * @brief Encapsulates a skybox.
 * 
 * @see skybox_shader
 */
class skybox
{
public:

	/// Default constructor.
	skybox() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param diffuse_texture A diffuse cube texture for the skybox.
	 */
	skybox(opengl::cube_texture diffuse_texture);

	/// Copy constructor.
	skybox(const skybox&) = default;

	/// Copy assignment operator.
	skybox& operator = (const skybox&) = default;

	/// Move constructor.
	skybox(skybox&&);

	/// Move assignment operator.
	skybox& operator = (skybox&&);

	/// The diffuse cube texture for the skybox.
	opengl::cube_texture diffuse_texture;

	/**
	 * @brief Draw the skybox.
	 * 
	 * @details This method will only bind the cube texture and draw an
	 * inverted unit cube. You still need to activate the correct shader.
	 * 
	 * @see skybox_shader
	 */
	void draw() const noexcept;

private:

};

} // namespace gintonic

#endif