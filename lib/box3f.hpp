/**
 * @file box3f.hpp
 * @author Raoul Wols
 */

#ifndef gintonic_box3f_hpp
#define gintonic_box3f_hpp

#include "vec3f.hpp"

namespace gintonic {

/**
 * @brief A three-dimensional axis-aligned bounding box.
 */
struct box3f
{

	/// The minimum corner of the axis-aligned bounding box.
	vec3f min_corner;

	/// The maximum corner of the axis-aligned bounding box.
	vec3f max_corner;

	/// Default constructor initalizes a point at the origin.
	box3f();

	/**
	 * @brief Constructor.
	 * 
	 * @param min_corner The minimum corner.
	 * @param max_corner The maximum corner.
	 */
	box3f(const vec3f& min_corner, const vec3f& max_corner);


	/**
	 * @brief Check wether this bounding box contains a point.
	 * 
	 * @param point Some point.
	 * @return True if the point is inside the bounding box, false otherwise.
	 * If the point is on the edge, then this is considered inside.
	 */
	bool contains(const vec3f& point) const BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Check wether this bounding box contains another bounding box.
	 * 
	 * @param other Some bounding box.
	 * @return True if other is completely inside the bounding box, false
	 * otherwise. If one of the edges touch, then this is considered inside.
	 */
	bool contains(const box3f& other) const BOOST_NOEXCEPT_OR_NOTHROW;

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond
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
bool intersects(const box3f& a, const box3f& b) BOOST_NOEXCEPT_OR_NOTHROW;

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

#endif