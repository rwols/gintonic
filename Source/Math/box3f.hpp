/**
 * @file box3f.hpp
 * @brief Defines a three-dimensional axis-aligned bounding box.
 * @author Raoul Wols
 */

#pragma once

#include "vec3f.hpp"
#include "vec4f.hpp"

namespace gintonic {

/**
 * @brief A three-dimensional axis-aligned bounding box.
 */
struct box3f
{

	/// The minimum corner of the axis-aligned bounding box.
	vec3f minCorner;

	/// The maximum corner of the axis-aligned bounding box.
	vec3f maxCorner;

	/// Default constructor initalizes a point at the origin.
	box3f();

	/**
	 * @brief Constructor.
	 * 
	 * @param minCorner The minimum corner.
	 * @param maxCorner The maximum corner.
	 */
	box3f(const vec3f& minCorner, const vec3f& maxCorner);


	/**
	 * @brief Check wether this bounding box contains a point.
	 * @param point Some point.
	 * @return True if the point is inside the bounding box, false otherwise.
	 * If the point is on the edge, then this is considered inside.
	 * @sa addPoint
	 */
	bool contains(const vec3f& point) const noexcept;

	/**
	 * @brief Check wether this bounding box contains another bounding box.
	 * @param other Some bounding box.
	 * @return True if other is completely inside the bounding box, false
	 * otherwise. If one of the edges touch, then this is considered inside.
	 * @sa addPoint
	 */
	bool contains(const box3f& other) const noexcept;

	/**
	 * @brief Add a point to this box3f. The box3f enlarges itself such that
	 * it contains the added point. Note that there is no such thing as an
	 * array containing the added points; the only thing this method does is
	 * enlarge the box3f if necessary. After this method, box3f::contains
	 * returns true for the supplied point.
	 * @param point A point.
	 * @sa contains
	 */
	void addPoint(const vec3f& point) noexcept;

	/**
	 * @brief Add a point to this box3f. The box3f enlarges itself such that
	 * it contains the added point. Note that there is no such thing as an
	 * array containing the added points; the only thing this method does is
	 * enlarge the box3f if necessary. After this method, box3f::contains
	 * returns true for the supplied point.
	 * @param point A point.
	 * @sa contains
	 */
	void addPoint(const vec4f& point) noexcept;

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

private:
	
	friend boost::serialization::access;
	template <class Archive>
	void serialize(Archive& archive, const unsigned int /*version */)
	{
		archive & minCorner & maxCorner;
	}
};

/**
 * @brief Check wether two bounding boxes intersect.
 * 
 * @param a Some bounding box.
 * @param b Another bounding box.
 * 
 * @return True if the intersection of a and b is non-empty, false otherwise.
 * If one of the edges touch, then this is still considered a non-empty
 * intersection.
 */
bool intersects(const box3f& a, const box3f& b) noexcept;

/**
 * @brief Output stream support for box2f.
 * 
 * @param os An output stream.
 * @param b Some bounding box.
 */
std::ostream& operator << (std::ostream& os, const box3f& b);

/**
 * @brief Input stream support for box2f.
 * 
 * @param is An input stream.
 * @param b Some bounding box.
 */
std::istream& operator >> (std::istream& is, box3f& b);

} // namespace gintonic
