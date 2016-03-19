/**
 * @file SQT.hpp
 * @brief Defines the SQT (scale, quaternion, translation) class.
 * @author Raoul Wols
 */

#pragma once

#include "vec3f.hpp"
#include "quatf.hpp"
#include "mat4f.hpp"

namespace gintonic {

/**
 * @brief Defines a coordinate system.
 * @details Defines a coordinate system. That is, it defines
 * a scale in the X, Y and Z directions, it provides a quaternion
 * rotation and provides a translation for the origin.
 */
struct SQT
{
	/// The scale.
	vec3f scale;

	/// The rotation.
	quatf rotation;

	/// The translation.
	vec3f translation;

	/**
	 * @brief Default constructor.
	 * @details The default constructor constructs a standard basis
	 * of three-dimensional space with its origin located at [0,0,0].
	 */
	inline SQT()
	: scale(1.0f, 1.0f, 1.0f)
	, rotation(1.0f, 0.0f, 0.0f, 0.0f)
	, translation(0.0f, 0.0f, 0.0f)
	{
		/* Empty on purpose. */
	}

	/**
	 * @brief Constructor that takes a scale, a rotation and a translation.
	 * 
	 * @param scale The scaling in the X, Y and Z directions.
	 * @param rotation The rotation quaternion.
	 * @param translation The location of the origin.
	 */
	inline SQT(const vec3f& scale, const quatf& rotation, const vec3f& translation)
	: scale(scale)
	, rotation(rotation)
	, translation(translation)
	{
		/* Empty on purpose. */
	}

	/**
	 * @brief Constructor that takes an affine matrix.
	 * 
	 * @details It extracts the scale, rotation and translation from the matrix.
	 * This will silently fail if the supplied matrix is not an affine transformation.
	 * 
	 * @param assimpMatrix The affine matrix.
	 */
	SQT(const mat4f& affineTransformation);

	/**
	 * @brief Add two SQTs together.
	 * @details The scales get pointwise-multiplied, the rotations
	 * get multiplied (as quaternions), and the translations get added.
	 * Quaternion multiplication is non-commutative, so be wary
	 * about the order of multiplication.
	 * 
	 * @param other Another SQT.
	 * @return The added SQT.
	 */
	SQT operator % (const SQT& other) const noexcept;

	/**
	 * @brief Add an SQT to this SQT.
	 * @details The scales get pointwise-multiplied, the rotations
	 * get multiplied (as quaternions), and the translations get added.
	 * Quaternion multiplication is non-commutative, so be wary
	 * about the order of multiplication.
	 * 
	 * @param other Another SQT.
	 */
	SQT& operator %= (const SQT& other) noexcept;

	/**
	 * @brief Get the inverse of this SQT.
	 * @details The scales are inverted pointwise. The rotation
	 * quaternion is inverted and the negative of the translation
	 * is taken. Observe then, if `sqt` is an SQT, then `sqt.inverse() % sqt == SQT();`
	 * @return The inverse.
	 */
	SQT inverse() const noexcept;

	/**
	 * @brief Make this SQT the inverse of itself.
	 * @details The scales are inverted pointwise. The rotation
	 * quaternion is inverted and the negative of the translation
	 * is taken.
	 * @return This.
	 */
	SQT& invert() noexcept;

	/**
	 * @brief Make this SQT look at another SQT.
	 * @details The rotation quaternion of this SQT is set in such
	 * a way that, given this SQT's current translation and the
	 * other SQT's translation, the forward direction points from
	 * this SQT's translation to the other SQT's translation.
	 * 
	 * @param other The SQT to look at.
	 */
	inline void look_at(const SQT& other) noexcept
	{
		rotation = quatf::look_at(translation, other.translation, vec3f(0.0f, 1.0f, 0.0f));
	}

	/**
	 * @brief Moves this SQT in its forward direction.
	 * @details Given the current forward direction defined by
	 * its rotation quaternion, you can move the SQT with a given
	 * amount in the forward direction. The default forward direction
	 * of OpenGL is the vector [0,0,-1].
	 * 
	 * @param amount The amount of movement to apply.
	 */
	inline void move_forward(const float amount) noexcept
	{
		translation += amount * rotation.forward_direction();
	}

	/**
	 * @brief Moves this SQT in its backward direction.
	 * @details Given the current forward direction defined by
	 * its rotation quaternion, you can move the SQT with a given
	 * amount in the backward direction. This is just the negative of
	 * the forward direction. The default forward direction
	 * of OpenGL is the vector [0,0,-1].
	 * 
	 * @param amount The amount of movement to apply.
	 */
	inline void move_backward(const float amount) noexcept
	{
		translation -= amount * rotation.forward_direction();
	}

	/**
	 * @brief Moves this SQT in its right direction.
	 * @details Given the current right direction defined by
	 * its rotation quaternion, you can move the SQT with a given
	 * amount in the forward direction. The default forward direction
	 * of OpenGL is the vector [1,0,0].
	 * 
	 * @param amount The amount of movement to apply.
	 */
	inline void move_right(const float amount) noexcept
	{
		translation += amount * rotation.right_direction();
	}

	/**
	 * @brief Moves this SQT in its left direction.
	 * @details Given the current left direction defined by
	 * its rotation quaternion, you can move the SQT with a given
	 * amount in the left direction. This is just the negative of
	 * the right direction. The default right direction
	 * of OpenGL is the vector [1,0,0].
	 * 
	 * @param amount The amount of movement to apply.
	 */
	inline void move_left(const float amount) noexcept
	{
		translation -= amount * rotation.right_direction();
	}

	/**
	 * @brief Moves this SQT in its up direction.
	 * @details Given the current right direction defined by
	 * its rotation quaternion, you can move the SQT with a given
	 * amount in the up direction. The default up direction
	 * of OpenGL is the vector [0,1,0].
	 * 
	 * @param amount The amount of movement to apply.
	 */
	inline void move_up(const float amount) noexcept
	{
		translation += amount * rotation.up_direction();
	}

	/**
	 * @brief Moves this SQT in its down direction.
	 * @details Given the current down direction defined by
	 * its rotation quaternion, you can move the SQT with a given
	 * amount in the down direction. This is just the negative of
	 * the up direction. The default up direction
	 * of OpenGL is the vector [0,1,0].
	 * 
	 * @param amount The amount of movement to apply.
	 */
	inline void move_down(const float amount) noexcept
	{
		translation -= amount * rotation.up_direction();
	}

	/**
	 * @brief Adds a mouse delta to the rotation quaternion.
	 * 
	 * @param delta The vector of angles.
	 * @deprecated Don't use this method.
	 */
	inline void add_mousedelta(const vec2f& delta) noexcept
	{
		rotation.add_mousedelta(delta);
	}

	/**
	 * @brief Applies the SQT to a point.
	 * @details The SQT first scales the point, then rotates the point, and
	 * finally adds the translation to the point.
	 * 
	 * @param point A point.
	 * @return The scaled, rotated and translated point.
	 */
	inline vec3f apply_to_point(const vec3f& point) const noexcept
	{
		return rotation.apply_to(point) * scale + translation;
	}

	/**
	 * @brief Applies the SQT to a direction.
	 * @details The SQT first scales the point and then rotates it.
	 * 
	 * @param direction A direction.
	 * @return The scaled and rotated direction.
	 */
	inline vec3f apply_to_direction(const vec3f& direction) const noexcept
	{
		return rotation.apply_to(direction);
	}

	//! @cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//! @endcond

private:

	//!@cond
	// We need to give boost::serialization access to this class.
	friend boost::serialization::access;
	//!@endcond

	template <class Archive>
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(scale) 
			& BOOST_SERIALIZATION_NVP(rotation) 
			& BOOST_SERIALIZATION_NVP(translation);
	}
};

/**
 * @brief Computes the squared distance between two SQTs.
 * @details Given the translation of a and b, computes the squared
 * distance between the two origins.
 * 
 * @param a An SQT.
 * @param b Another SQT.
 * @return The squared distance from a to b.
 */
inline float distance2(const SQT& a, const SQT& b) noexcept
{
	return distance2(a.translation, b.translation);
}

/**
 * @brief Computes the distance between two SQTs.
 * @details Given the translation of a and b, computes the distance
 * between the two origins.
 * 
 * @param a An SQT.
 * @param b Another SQT.
 * @return The distance from a to b.
 */
inline float distance(const SQT& a, const SQT& b) noexcept
{
	return distance(a.translation, b.translation);
}

/**
 * @brief Output an SQT to an output stream.
 */
std::ostream& operator << (std::ostream&, const SQT&);

/**
 * @brief Get an SQT from an input stream.
 */
std::istream& operator >> (std::istream&, SQT&);

} // namespace gintonic