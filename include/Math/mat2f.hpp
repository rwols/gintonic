/**
 * @file mat2f.hpp
 * @brief Defines a two by two matrix.
 * @author Raoul Wols
 */

#pragma once

#include "../Foundation/Profiler.hpp"
#include "../Foundation/utilities.hpp"
#include "gintonic_export.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace gintonic {

union vec2f; // Forward declaration.

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#endif

/**
 * @brief Two by two matrix. Column-major storage.
 */
union GINTONIC_EXPORT alignas(__m128) mat2f
{
    /// The raw SSE type.
    __m128 data;

    struct
    {
        /// Entry (0,0)
        float m00;
        /// Entry (1,0)
        float m10;
        /// Entry (0,1)
        float m01;
        /// Entry (1,1)
        float m11;
    };

    /// Default constructor.
    inline mat2f() = default;

    /// Constructor.
    inline mat2f(const float m00, const float m01, const float m10,
                 const float m11)
        : m00(m00), m10(m10), m01(m01), m11(m11)
    {
        GT_PROFILE_FUNCTION;
    }

    /// Copy constructor.
    inline mat2f(const mat2f& m) : data(m.data) { GT_PROFILE_FUNCTION; }

    /// Move constructor.
    inline mat2f(mat2f&& m) : data(m.data) { GT_PROFILE_FUNCTION; }

    /// Constructor which takes a raw SSE type.
    inline mat2f(__m128 data) : data(data) { GT_PROFILE_FUNCTION; }

    /// Column constructor.
    mat2f(const vec2f& column0, const vec2f& column1);

    /// Initializer list constructor.
    mat2f(std::initializer_list<float> init);

    /// Copy assignment operator.
    inline mat2f& operator=(const mat2f& m)
    {
        GT_PROFILE_FUNCTION;

        data = m.data;
        return *this;
    }

    /// Move assignment operator.
    inline mat2f& operator=(mat2f&& m)
    {
        GT_PROFILE_FUNCTION;

        data = m.data;
        return *this;
    }

    /// Initializer list assignment operator.
    mat2f& operator=(std::initializer_list<float> init);

    /// Add-and-assign operator.
    inline mat2f& operator+=(const mat2f& v)
    {
        GT_PROFILE_FUNCTION;

        data = _mm_add_ps(data, v.data);
        return *this;
    }

    /// Substract-and-assign operator.
    inline mat2f& operator-=(const mat2f& v)
    {
        GT_PROFILE_FUNCTION;

        data = _mm_sub_ps(data, v.data);
        return *this;
    }

    /// Multiply-and-assign operator.
    inline mat2f& operator*=(const mat2f& v)
    {
        GT_PROFILE_FUNCTION;

        data = _mm_hadd_ps(
            _mm_mul_ps(data,
                       _mm_shuffle_ps(v.data, v.data, _MM_SHUFFLE(3, 1, 2, 0))),
            _mm_mul_ps(
                data, _mm_shuffle_ps(v.data, v.data, _MM_SHUFFLE(2, 0, 3, 1))));
        data = _mm_shuffle_ps(data, data, _MM_SHUFFLE(1, 3, 2, 0));
        return *this;
    }

    /// Multiply-and-assign operator for scalars.
    inline mat2f& operator*=(const float s)
    {
        GT_PROFILE_FUNCTION;

        data = _mm_mul_ps(data, _mm_load1_ps(&s));
        return *this;
    }

    /// Divide-and-assign operator for scalars.
    inline mat2f& operator/=(const float s)
    {
        GT_PROFILE_FUNCTION;

        data = _mm_div_ps(data, _mm_load1_ps(&s));
        return *this;
    }

    /// Equality comparison operator.
    inline bool operator==(const mat2f& m)
    {
        GT_PROFILE_FUNCTION;

        return m00 == m.m00 && m10 == m.m10 && m01 == m.m01 && m.m11;
    }

    /// Inequality comparison operator.
    inline bool operator!=(const mat2f& m)
    {
        GT_PROFILE_FUNCTION;

        return !operator==(m);
    }

  private:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int /*version*/)
    {
        GT_PROFILE_FUNCTION;

        ar& m00& m10& m01& m11;
    }
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

/// Addition operator.
inline mat2f operator+(const mat2f& lhs, const mat2f& rhs)
{
    GT_PROFILE_FUNCTION;

    return _mm_add_ps(lhs.data, rhs.data);
}

/// Subtraction operator.
inline mat2f operator-(const mat2f& lhs, const mat2f& rhs)
{
    GT_PROFILE_FUNCTION;

    return _mm_sub_ps(lhs.data, rhs.data);
}

/// Apply a matrix to a vector.
vec2f operator*(const mat2f& m, const vec2f& v);

/// Output stream support for two-dimensional matrices.
inline std::ostream& operator<<(std::ostream& os, const mat2f& m)
{
    GT_PROFILE_FUNCTION;

    return os << m.m00 << ' ' << m.m10 << ' ' << m.m01 << ' ' << m.m11;
}

/// Input stream support for two-dimensional matrices.
inline std::istream& operator>>(std::istream& is, mat2f& m)
{
    GT_PROFILE_FUNCTION;

    is >> m.m00 >> m.m10 >> m.m01 >> m.m11;
    return is;
}

} // namespace gintonic

// This macro is needed for boost::serialization because boost::serialization
// does not automatically assume unions are serializable.
BOOST_CLASS_IMPLEMENTATION(gintonic::mat2f,
                           boost::serialization::object_serializable);
