/**
 * @file box2f.hpp
 * @brief Defines a two-dimensional axis-aligned bounding box.
 * @author Raoul Wols
 */

#pragma once

#include <gintonic/Math/vec2f.hpp>

#include <gintonic/nsbegin.hpp>

/**
 * @brief A two-dimensional axis-aligned bounding box.
 */
struct box2f
{

    /// The minimum corner of the axis-aligned bounding box.
    vec2f minCorner;

    /// The maximum corner of the axis-aligned bounding box.
    vec2f maxCorner;

    /// Default constructor initalizes a point at the origin.
    box2f();

    /**
     * @brief Constructor.
     *
     * @param minCorner The minimum corner.
     * @param maxCorner The maximum corner.
     */
    box2f(const vec2f& minCorner, const vec2f& maxCorner);

    /**
     * @brief Check wether this bounding box contains a point.
     *
     * @param point Some point.
     * @return True if the point is inside the bounding box, false otherwise.
     * If the point is on the edge, then this is considered inside.
     */
    bool contains(const vec2f& point) const noexcept;

    /**
     * @brief Check wether this bounding box contains another bounding box.
     *
     * @param other Some bounding box.
     * @return True if other is completely inside the bounding box, false
     * otherwise. If one of the edges touch, then this is considered inside.
     */
    bool contains(const box2f& other) const noexcept;

    GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

  private:
    friend boost::serialization::access;
    template <class Archive>
    void serialize(Archive& archive, const unsigned int /*version */)
    {
        GT_PROFILE_FUNCTION;

        archive& minCorner& maxCorner;
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
bool intersects(const box2f& a, const box2f& b) noexcept;

/**
 * @brief Output stream support for box2f.
 *
 * @param os An output stream.
 * @param b Some bounding box.
 */
std::ostream& operator<<(std::ostream& os, const box2f& b);

/**
 * @brief Input stream support for box2f.
 *
 * @param is An input stream.
 * @param b Some bounding box.
 */
std::istream& operator>>(std::istream& is, box2f& b);

#include <gintonic/nsend.hpp>
