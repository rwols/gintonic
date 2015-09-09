/**
 * @file mesh.hpp
 * @brief Defines the mesh base class component.
 * @author Raoul Wols
 */

#ifndef gintonic_mesh_hpp
#define gintonic_mesh_hpp

#include "opengl/glad.hpp"
#include "component.hpp"
#include "mat3f.hpp"
#include "mat4f.hpp"
#include "allocator.hpp"
#include <vector>

namespace FBX
{
	class FbxVector2; // Forward declaration.
	class FbxVector4; // Forward declaration.
} // namespace FBX

namespace gintonic {

/**
 * @brief Mesh component.
 * @details You can either draw the mesh instanced or non-intanced. When
 * drawing the mesh non-instanced, you should use the renderer for managing
 * the current modelview and modelviewprojection matrices. When rendering
 * the mesh instanced, you need to supply the matrices yourself.
 */
class mesh : public component
{
public:

	/// The type of mesh. Either static or dynamic.
	enum mesh_type
	{
		kStaticMesh,
		kDynamicMesh
	};

	virtual void attach(entity&);
	virtual void detach(entity&);

	/**
	 * @brief Draw the mesh.
	 */
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW = 0;

	/**
	 * @brief Draw the mesh instanced.
	 * @details For reach instance, you need to supply a PVM matrix, a VM
	 * matrix and an N matrix. You need to make sure that each of these
	 * arrays is the same size.
	 * 
	 * @param PVM_matrices An array of PVM matrices.
	 * @param VM_matrices An array of VM matrices.
	 * @param N_matrices An array of N matrices.
	 */
	virtual void draw(
		const std::vector<mat4f, allocator<mat4f>>& PVM_matrices,
		const std::vector<mat4f, allocator<mat4f>>& VM_matrices,
		const std::vector<mat3f, allocator<mat3f>>& N_matrices);

	/// The type of the mesh.
	inline mesh_type type() const BOOST_NOEXCEPT_OR_NOTHROW { return m_type; }

	/// Destructor.
	virtual ~mesh() BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief An additional vector class that doesn't use SSE types.
	 * @details You can use this class to build meshes internally.
	 */
	struct vec2f
	{
		/// The X-coordinate.
		GLfloat x;

		/// The Y-coordinate.
		GLfloat y;

		/// Default constructor.
		vec2f() = default;

		/// Takes an FbxVector2.
		vec2f(const FBX::FbxVector2&);

		/// Constructor that takes an X and a Y value.
		vec2f(const GLfloat x, const GLfloat y);

		/// Equality comparison operator.
		bool operator == (const vec2f&) const BOOST_NOEXCEPT_OR_NOTHROW;

		/// Inequality comparison operator.
		bool operator != (const vec2f&) const BOOST_NOEXCEPT_OR_NOTHROW;

		/**
		 * @brief Enables this vector type in the current OpenGL array buffer.
		 * @details When an OpenGL array buffer is bound, normally you would
		 * call glVertexAttribPointer and glEnableAttribute. This static
		 * method does it for you so that one doesn't have to care about the
		 * correct arguments.
		 * 
		 * @param index The index.
		 */
		static void enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW;
	};

	/**
	 * @brief An additional vector class that doesn't use SSE types.
	 * @details You can use this class to build meshes internally.
	 */
	struct vec3f
	{
		/// The X-coordinate.
		GLfloat x;

		/// The Y-coordinate.
		GLfloat y;

		/// The Z-coordinate.
		GLfloat z;

		/// Default constructor.
		vec3f() = default;

		/**
		 * @brief Constructor that takes an FbxVector4.
		 * @details Only the first three components of the FbxVector4 are
		 * used. The fourth component is ignored.
		 * 
		 * @param v The FbxVector4.
		 */
		vec3f(const FBX::FbxVector4& v);

		/// Constructor that takes an X, Y and Z value.
		vec3f(const GLfloat x, const GLfloat y, const GLfloat z);

		/// Equality comparison operator.
		bool operator == (const vec3f&) const BOOST_NOEXCEPT_OR_NOTHROW;

		/// Inequality comparison operator.
		bool operator != (const vec3f&) const BOOST_NOEXCEPT_OR_NOTHROW;

		/**
		 * @brief Enables this vector type in the current OpenGL array buffer.
		 * @details When an OpenGL array buffer is bound, normally you would
		 * call glVertexAttribPointer and glEnableAttribute. This static
		 * method does it for you so that one doesn't have to care about the
		 * correct arguments.
		 * 
		 * @param index The index.
		 */
		static void enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW;
	};

	/**
	 * @brief An additional vector class that doesn't use SSE types.
	 * @details You can use this class to build meshes internally.
	 */
	struct vec4f
	{
		/// The X-coordinate.
		GLfloat x;

		/// The Y-coordinate.
		GLfloat y;

		/// The Z-coordinate.
		GLfloat z;

		/// The W-coordinate.
		GLfloat w;

		/// Constructor.
		vec4f() = default;

		/// Constructor that takes an FbxVector4.
		vec4f(const FBX::FbxVector4&);

		/// Constructor that takes an X, Y, Z and W value.
		vec4f(const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w);

		/// Equality comparison operator.
		bool operator == (const vec4f&) const BOOST_NOEXCEPT_OR_NOTHROW;
	
		/// Inequality comparison operator.
		bool operator != (const vec4f&) const BOOST_NOEXCEPT_OR_NOTHROW;

		/**
		 * @brief Enables this vector type in the current OpenGL array buffer.
		 * @details When an OpenGL array buffer is bound, normally you would
		 * call glVertexAttribPointer and glEnableAttribute. This static
		 * method does it for you so that one doesn't have to care about the
		 * correct arguments.
		 * 
		 * @param index The index.
		 */
		static void enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW;
	};

protected:

	/// Constructor.
	inline mesh(const mesh_type t) : m_type(t) {}

private:

	mesh_type m_type;

};

} // namespace gintonic

#endif