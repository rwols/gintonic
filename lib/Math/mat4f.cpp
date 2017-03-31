#include <gintonic/Math/mat4f.hpp>

#include <gintonic/Math/SQT.hpp>
#include <gintonic/Math/mat3f.hpp>
#include <gintonic/Math/quatf.hpp>
#include <gintonic/Math/vec3f.hpp>
#include <gintonic/Math/vec4f.hpp>
#include <gintonic/Entity.hpp>
#include <iomanip> // for operator <<
#include <sstream> // for operator <<

using namespace gintonic;

mat4f mat4f::zero = mat4f(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
mat4f mat4f::identity = mat4f(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

#ifdef BOOST_MSVC // NOTE: Consider removing this when on MSVC2015

mat4f::mat4f(const vec3f& translation)
{
    GT_PROFILE_FUNCTION;
    data[0] = _mm_set1_ps(0.0f);
    data[1] = _mm_set1_ps(0.0f);
    data[2] = _mm_set1_ps(0.0f);
    data[3] = translation.data;
    m33 = 1.0f;
}

#else

mat4f::mat4f(const vec3f& translation)
    : m00(1.0f), m10(0.0f), m20(0.0f), m30(0.0f), m01(0.0f), m11(1.0f),
      m21(0.0f), m31(0.0f), m02(0.0f), m12(0.0f), m22(1.0f), m32(0.0f),
      m03(translation.x), m13(translation.y), m23(translation.z), m33(1.0f)
{
    GT_PROFILE_FUNCTION;
}

#endif

mat4f::mat4f(const quatf& rotation)
{
    GT_PROFILE_FUNCTION;

    // SIMDify this...
    const auto n = rotation.length2();
    const auto s = (n == 0.0f) ? 0.0f : 2.0f / n;
    const auto wx = s * rotation.w * rotation.x;
    const auto wy = s * rotation.w * rotation.y;
    const auto wz = s * rotation.w * rotation.z;
    const auto xx = s * rotation.x * rotation.x;
    const auto xy = s * rotation.x * rotation.y;
    const auto xz = s * rotation.x * rotation.z;
    const auto yy = s * rotation.y * rotation.y;
    const auto yz = s * rotation.y * rotation.z;
    const auto zz = s * rotation.z * rotation.z;

    m00 = 1.0f - (yy + zz);
    m10 = xy + wz;
    m20 = xz - wy;
    m30 = 0.0f;

    m01 = xy - wz;
    m11 = 1.0f - (xx + zz);
    m21 = yz + wx;
    m31 = 0.0f;

    m02 = xz + wy;
    m12 = yz - wx;
    m22 = 1.0f - (xx + yy);
    m32 = 0.0f;

    m03 = 0.0f;
    m13 = 0.0f;
    m23 = 0.0f;
    m33 = 1.0f;
}

mat4f::mat4f(const SQT& transform)
{
    GT_PROFILE_FUNCTION;

    // SIMDify this...
    const auto n = transform.rotation.length2();
    const auto s = (n == 0.0f) ? 0.0f : 2.0f / n;
    const auto wx = s * transform.rotation.w * transform.rotation.x;
    const auto wy = s * transform.rotation.w * transform.rotation.y;
    const auto wz = s * transform.rotation.w * transform.rotation.z;
    const auto xx = s * transform.rotation.x * transform.rotation.x;
    const auto xy = s * transform.rotation.x * transform.rotation.y;
    const auto xz = s * transform.rotation.x * transform.rotation.z;
    const auto yy = s * transform.rotation.y * transform.rotation.y;
    const auto yz = s * transform.rotation.y * transform.rotation.z;
    const auto zz = s * transform.rotation.z * transform.rotation.z;

    m00 = 1.0f - (yy + zz);
    m10 = xy + wz;
    m20 = xz - wy;
    m30 = 0.0f;

    m01 = xy - wz;
    m11 = 1.0f - (xx + zz);
    m21 = yz + wx;
    m31 = 0.0f;

    m02 = xz + wy;
    m12 = yz - wx;
    m22 = 1.0f - (xx + yy);
    m32 = 0.0f;

    // #ifdef BOOST_MSVC

    // auto copy = data[0];
    // auto scaling = transform.scale.data;
    // data[0] = _mm_mul_ps(copy, scaling);
    // copy = data[1];
    // data[1] = _mm_mul_ps(copy, scaling);
    // copy = data[2];
    // data[2] = _mm_mul_ps(copy, scaling);

    // #else

    data[0] = _mm_mul_ps(data[0], transform.scale.data);
    data[1] = _mm_mul_ps(data[1], transform.scale.data);
    data[2] = _mm_mul_ps(data[2], transform.scale.data);

    // #endif

    data[3] = transform.translation.data;
    m33 = 1.0f;
}

mat4f::mat4f(const Entity& e) : mat4f(e.globalTransform())
{
    GT_PROFILE_FUNCTION;
}

mat4f::mat4f(const box3f& box)
{
    GT_PROFILE_FUNCTION;

    const float lLeft = box.minCorner.x;
    const float lRight = box.maxCorner.x;
    const float lTop = box.maxCorner.y;
    const float lBottom = box.minCorner.y;
    const float lNearPlane = box.minCorner.z;
    const float lFarPlane = box.maxCorner.z;
    const float lRightMinusLeft = lRight - lLeft;
    const float lTopMinusBottom = lTop - lBottom;
    const float lFarMinusNear = lFarPlane - lNearPlane;

    const auto lDiff = box.maxCorner - box.minCorner;
    const auto lSum = box.maxCorner + box.minCorner;
    data[3] = (lSum / lDiff).data;
    m33 = 1.0f;

    data[0] = _mm_set1_ps(0.0f);
    data[1] = _mm_set1_ps(0.0f);
    data[2] = _mm_set1_ps(0.0f);
    data[3] = _mm_set1_ps(0.0f);

    m00 = 0.5f * lRightMinusLeft;
    m11 = 0.5f * lTopMinusBottom;
    m22 = 0.5f * lFarMinusNear;
    m03 = (lRight + lLeft) / lRightMinusLeft;
    m13 = (lTop + lBottom) / lTopMinusBottom;
    m23 = (lFarPlane + lNearPlane) / lFarMinusNear;
    m33 = 1.0f;
}

mat4f::mat4f(const mat3f& rotation_part)
{
    GT_PROFILE_FUNCTION;

    data[0] = _mm_set_ps(rotation_part.data[0], rotation_part.data[1],
                         rotation_part.data[2], 0.0f);
    data[1] = _mm_set_ps(rotation_part.data[3], rotation_part.data[4],
                         rotation_part.data[5], 0.0f);
    data[2] = _mm_set_ps(rotation_part.data[6], rotation_part.data[7],
                         rotation_part.data[8], 0.0f);

    m30 = 0.0f;
    m31 = 0.0f;
    m32 = 0.0f;

    m03 = 0.0f;
    m13 = 0.0f;
    m23 = 0.0f;
    m33 = 1.0f;
}

#ifdef BOOST_MSVC // NOTE: Consider removing this when on MSVC2015

mat4f::mat4f(const vec4f& column0, const vec4f& column1, const vec4f& column2,
             const vec4f& column3)
{
    GT_PROFILE_FUNCTION;

    data[0] = column0.data;
    data[1] = column1.data;
    data[2] = column2.data;
    data[3] = column3.data;
}

#else

mat4f::mat4f(const vec4f& column0, const vec4f& column1, const vec4f& column2,
             const vec4f& column3)
    : data{column0.data, column1.data, column2.data, column3.data}
{
    GT_PROFILE_FUNCTION;
}

#endif

vec4f mat4f::operator*(const vec4f& v) const
{
    GT_PROFILE_FUNCTION;

    const auto m0 = _mm_mul_ps(data[0], _mm_replicate_x_ps(v.data));
    const auto m1 = _mm_mul_ps(data[1], _mm_replicate_y_ps(v.data));
    const auto m2 = _mm_mul_ps(data[2], _mm_replicate_z_ps(v.data));
    const auto m3 = _mm_mul_ps(data[3], _mm_replicate_w_ps(v.data));
    return _mm_add_ps(_mm_add_ps(m0, m1), _mm_add_ps(m2, m3));
}

mat4f mat4f::operator*(const mat4f& other) const
{
    GT_PROFILE_FUNCTION;

#ifdef BOOST_MSVC

    mat4f r;
    __m128 m0, m1, m2, m3, other0, other1, other2, other3;

    other0 = other.data[0];
    other1 = other.data[1];
    other2 = other.data[2];
    other3 = other.data[3];

    m0 = _mm_mul_ps(data[0], _mm_replicate_x_ps(other0));
    m1 = _mm_mul_ps(data[1], _mm_replicate_y_ps(other0));
    m2 = _mm_mul_ps(data[2], _mm_replicate_z_ps(other0));
    m3 = _mm_mul_ps(data[3], _mm_replicate_w_ps(other0));
    r.data[0] = _mm_add_ps(_mm_add_ps(m0, m1), _mm_add_ps(m2, m3));

    m0 = _mm_mul_ps(data[0], _mm_replicate_x_ps(other1));
    m1 = _mm_mul_ps(data[1], _mm_replicate_y_ps(other1));
    m2 = _mm_mul_ps(data[2], _mm_replicate_z_ps(other1));
    m3 = _mm_mul_ps(data[3], _mm_replicate_w_ps(other1));
    r.data[1] = _mm_add_ps(_mm_add_ps(m0, m1), _mm_add_ps(m2, m3));

    m0 = _mm_mul_ps(data[0], _mm_replicate_x_ps(other2));
    m1 = _mm_mul_ps(data[1], _mm_replicate_y_ps(other2));
    m2 = _mm_mul_ps(data[2], _mm_replicate_z_ps(other2));
    m3 = _mm_mul_ps(data[3], _mm_replicate_w_ps(other2));
    r.data[2] = _mm_add_ps(_mm_add_ps(m0, m1), _mm_add_ps(m2, m3));

    m0 = _mm_mul_ps(data[0], _mm_replicate_x_ps(other3));
    m1 = _mm_mul_ps(data[1], _mm_replicate_y_ps(other3));
    m2 = _mm_mul_ps(data[2], _mm_replicate_z_ps(other3));
    m3 = _mm_mul_ps(data[3], _mm_replicate_w_ps(other3));
    r.data[3] = _mm_add_ps(_mm_add_ps(m0, m1), _mm_add_ps(m2, m3));

    return r;

#else

    mat4f r;
    __m128 m0, m1, m2, m3;

    m0 = _mm_mul_ps(data[0], _mm_replicate_x_ps(other.data[0]));
    m1 = _mm_mul_ps(data[1], _mm_replicate_y_ps(other.data[0]));
    m2 = _mm_mul_ps(data[2], _mm_replicate_z_ps(other.data[0]));
    m3 = _mm_mul_ps(data[3], _mm_replicate_w_ps(other.data[0]));
    r.data[0] = _mm_add_ps(_mm_add_ps(m0, m1), _mm_add_ps(m2, m3));

    m0 = _mm_mul_ps(data[0], _mm_replicate_x_ps(other.data[1]));
    m1 = _mm_mul_ps(data[1], _mm_replicate_y_ps(other.data[1]));
    m2 = _mm_mul_ps(data[2], _mm_replicate_z_ps(other.data[1]));
    m3 = _mm_mul_ps(data[3], _mm_replicate_w_ps(other.data[1]));
    r.data[1] = _mm_add_ps(_mm_add_ps(m0, m1), _mm_add_ps(m2, m3));

    m0 = _mm_mul_ps(data[0], _mm_replicate_x_ps(other.data[2]));
    m1 = _mm_mul_ps(data[1], _mm_replicate_y_ps(other.data[2]));
    m2 = _mm_mul_ps(data[2], _mm_replicate_z_ps(other.data[2]));
    m3 = _mm_mul_ps(data[3], _mm_replicate_w_ps(other.data[2]));
    r.data[2] = _mm_add_ps(_mm_add_ps(m0, m1), _mm_add_ps(m2, m3));

    m0 = _mm_mul_ps(data[0], _mm_replicate_x_ps(other.data[3]));
    m1 = _mm_mul_ps(data[1], _mm_replicate_y_ps(other.data[3]));
    m2 = _mm_mul_ps(data[2], _mm_replicate_z_ps(other.data[3]));
    m3 = _mm_mul_ps(data[3], _mm_replicate_w_ps(other.data[3]));
    r.data[3] = _mm_add_ps(_mm_add_ps(m0, m1), _mm_add_ps(m2, m3));

    return r;

#endif
}

mat4f& mat4f::operator*=(const mat4f& other)
{
    GT_PROFILE_FUNCTION;

    *this = *this * other;
    return *this;
}

mat4f::mat4f(const vec3f& axis, const float rotation_angle)
{
    GT_PROFILE_FUNCTION;

    vec3f v;
    const float sin = std::sin(rotation_angle);
    const float cos = std::cos(rotation_angle);
    const float omcos = 1.0f - cos;

    v.x = cos + axis.x * axis.x * omcos;
    v.y = axis.x * axis.y * omcos - axis.z * sin;
    v.z = axis.x * axis.z * omcos + axis.y * sin;
    v.normalize();

    m00 = v.x;
    m01 = v.y;
    m02 = v.z;
    m03 = 0.0f;

    v.x = axis.x * axis.y * omcos + axis.z * sin;
    v.y = cos + axis.y * axis.y * omcos;
    v.z = axis.y * axis.z * omcos - axis.x * sin;
    v.normalize();

    m10 = v.x;
    m11 = v.y;
    m12 = v.z;
    m13 = 0.0f;

    v.x = axis.x * axis.z * omcos - axis.y * sin;
    v.y = axis.y * axis.z * omcos + axis.x * sin;
    v.z = cos + axis.z * axis.z * omcos;
    v.normalize();

    m20 = v.x;
    m21 = v.y;
    m22 = v.z;
    m23 = 0.0f;

    m30 = m31 = m32 = 0.0f;
    m33 = 1.0f;
#undef m
}

mat4f& mat4f::set_orthographic(const float left, const float right,
                               const float bottom, const float top,
                               const float nearplane, const float farplane)
{
    GT_PROFILE_FUNCTION;

    const float rl = right - left;
    const float tb = top - bottom;
    const float fn = farplane - nearplane;

    data[0] = _mm_set1_ps(0.0f);
    data[1] = _mm_set1_ps(0.0f);
    data[2] = _mm_set1_ps(0.0f);
    data[3] = _mm_set1_ps(0.0f);

    // m00 = 2.0f * nearplane / rl;
    // m11 = 2.0f * nearplane / tb;
    // m02 = (right + left) / rl;
    // m12 = (top + bottom) / tb;
    // m22 = -(farplane + nearplane) / fn;
    // m32 = 0.0f;
    // m23 = -(2.0f * farplane * nearplane) / fn;
    // m33 = 1.0f;

    m00 = 2.0f / rl;
    m11 = 2.0f / tb;
    m22 = -2.0f / fn;
    m03 = -(right + left) / rl;
    m13 = -(top + bottom) / tb;
    m23 = -(farplane + nearplane) / fn;
    m33 = 1.0f;

    return *this;
}

mat4f& mat4f::set_orthographic(const float width, const float height,
                               const float nearplane, const float farplane)
{
    GT_PROFILE_FUNCTION;

    const float fn = farplane - nearplane;

    data[0] = _mm_set1_ps(0.0f);
    data[1] = _mm_set1_ps(0.0f);
    data[2] = _mm_set1_ps(0.0f);
    data[3] = _mm_set1_ps(0.0f);

    m00 = 1.0f / width;
    m11 = 1.0f / height;
    m22 = -2.0f / fn;
    m23 = -(farplane + nearplane) / fn;
    m33 = 1.0f;

    return *this;

    // data[0] = _mm_set1_ps(0.0f);
    // data[1] = _mm_set1_ps(0.0f);
    // data[2] = _mm_set1_ps(0.0f);
    // data[3] = _mm_set1_ps(0.0f);

    // m00 = 1.0f / width;
    // m11 = 1.0f / height;
    // m22 = -2.0f / (farplane - nearplane);
    // m23 = -(farplane + nearplane) / (farplane - nearplane);
    // m33 = 1.0f;
    // return *this;
}

mat4f::mat4f(const vec3f& eye_location, const vec3f& subject_location,
             const vec3f& up_direction)
{
    GT_PROFILE_FUNCTION;

    const auto f = (subject_location - eye_location).normalize();
    const auto s = cross(f, up_direction).normalize();
    const auto u = cross(s, f).normalize();

    m00 = s.x;
    m10 = u.x;
    m20 = -f.x;
    m30 = 0.0f;

    m01 = s.y;
    m11 = u.y;
    m21 = -f.y;
    m31 = 0.0f;

    m02 = s.z;
    m12 = u.z;
    m22 = -f.z;
    m32 = 0.0f;

    m03 = -dot(eye_location, s);
    m13 = -dot(eye_location, u);
    m23 = dot(eye_location, f);
    m33 = 1.0f;
}

mat4f& mat4f::set_perspective(const float fieldofview, const float aspectratio,
                              const float nearplane, const float farplane)
{
    GT_PROFILE_FUNCTION;

    data[0] = _mm_set1_ps(0.0f);
    data[1] = _mm_set1_ps(0.0f);
    data[2] = _mm_set1_ps(0.0f);
    data[3] = _mm_set1_ps(0.0f);

    const float f = 1.0f / std::tan(fieldofview / 2.0f);

    m00 = f / aspectratio;
    m11 = f;
    m22 = (farplane + nearplane) / (nearplane - farplane);
    m32 = -1.0f;
    m23 = 2.0f * farplane * nearplane / (nearplane - farplane);

    return *this;
}

mat4f& mat4f::set_perspective_infinite(const float fieldofview,
                                       const float aspectratio,
                                       const float nearplane)
{
    GT_PROFILE_FUNCTION;

    data[0] = _mm_set1_ps(0.0f);
    data[1] = _mm_set1_ps(0.0f);
    data[2] = _mm_set1_ps(0.0f);
    data[3] = _mm_set1_ps(0.0f);

    const float f = 1.0f / std::tan(fieldofview / 2.0f);

    m00 = f / aspectratio;
    m11 = f;
    m22 = -1.0f;
    m32 = -1.0f;
    m23 = -2.0f * nearplane;

    return *this;
}

mat4f& mat4f::set_inverse_perspective(const float fieldofview,
                                      const float aspectratio,
                                      const float nearplane,
                                      const float farplane)
{
    GT_PROFILE_FUNCTION;

    const float b = 1.0f / std::tan(fieldofview / 2.0f);
    const float nf2 = 2.0f * nearplane * farplane;

    data[0] = _mm_set1_ps(0.0f);
    data[1] = _mm_set1_ps(0.0f);
    data[2] = _mm_set1_ps(0.0f);
    data[3] = _mm_set1_ps(0.0f);

    m00 = aspectratio / b;
    m11 = 1.0f / b;
    m32 = (nearplane - farplane) / nf2;
    m23 = 1.0f;
    m33 = (nearplane + farplane) / nf2;

    return *this;
}

void mat4f::unproject_perspective(float& fieldofview, float& aspectratio,
                                  float& nearplane, float& farplane)
{
    GT_PROFILE_FUNCTION;

    fieldofview = 2.0f * std::atan(1.0f / m11);
    aspectratio = m11 / m00;
    nearplane = m23 / (m22 - 1.0f);
    farplane = m23 / (m22 + 1.0f);
}

void mat4f::decompose(vec3f& scale, quatf& rotation, vec3f& translation) const
{
    GT_PROFILE_FUNCTION;

    // SIMDify this...

    mat4f tmp(*this);
    translation = tmp.data[3];
    translation.dummy = 0.0f;
    tmp.data[3] = _mm_set1_ps(0.0f);
    vec3f column(tmp.data[0]);
    column.dummy = 0.0f;
    scale.x = column.length();
    column = tmp.data[1];
    column.dummy = 0.0f;
    scale.y = column.length();
    column = tmp.data[2];
    column.dummy = 0.0f;
    scale.z = column.length();

    tmp.data[0] = _mm_mul_ps(_mm_set1_ps(1.0f / scale.x), tmp.data[0]);
    tmp.data[1] = _mm_mul_ps(_mm_set1_ps(1.0f / scale.y), tmp.data[1]);
    tmp.data[2] = _mm_mul_ps(_mm_set1_ps(1.0f / scale.z), tmp.data[2]);

    // Converting a rotation matrix to a quaternion is a bitch!
    // http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
    const float trace = tmp.m00 + tmp.m11 + tmp.m22;
    if (trace > 0)
    {
        const float S = std::sqrt(trace + 1.0f) * 2.0f; // S=4*qw
        rotation.w = 0.25f * S;
        rotation.x = (tmp.m21 - tmp.m12) / S;
        rotation.y = (tmp.m02 - tmp.m20) / S;
        rotation.z = (tmp.m10 - tmp.m01) / S;
    }
    else if ((tmp.m00 > tmp.m11) && (tmp.m00 > tmp.m22))
    {
        const float S = std::sqrt(1.0f + tmp.m00 - tmp.m11 - tmp.m22) *
                        2.0f; // S=4*rotation.x
        rotation.w = (tmp.m21 - tmp.m12) / S;
        rotation.x = 0.25f * S;
        rotation.y = (tmp.m01 + tmp.m10) / S;
        rotation.z = (tmp.m02 + tmp.m20) / S;
    }
    else if (tmp.m11 > tmp.m22)
    {
        const float S = std::sqrt(1.0f + tmp.m11 - tmp.m00 - tmp.m22) *
                        2.0f; // S=4*rotation.y
        rotation.w = (tmp.m02 - tmp.m20) / S;
        rotation.x = (tmp.m01 + tmp.m10) / S;
        rotation.y = 0.25f * S;
        rotation.z = (tmp.m12 + tmp.m21) / S;
    }
    else
    {
        const float S = std::sqrt(1.0f + tmp.m22 - tmp.m00 - tmp.m11) *
                        2.0f; // S=4*rotation.z
        rotation.w = (tmp.m10 - tmp.m01) / S;
        rotation.x = (tmp.m02 + tmp.m20) / S;
        rotation.y = (tmp.m12 + tmp.m21) / S;
        rotation.z = 0.25f * S;
    }
}

void mat4f::decompose(SQT& sqt) const
{
    GT_PROFILE_FUNCTION;

    decompose(sqt.scale, sqt.rotation, sqt.translation);
}

vec3f mat4f::apply_to_point(const vec3f& point) const noexcept
{
    GT_PROFILE_FUNCTION;

    const auto tmp = *this * vec4f(point, 1.0f);
    return vec3f(tmp.x, tmp.y, tmp.z);
}

vec3f mat4f::apply_to_direction(const vec3f& direction) const noexcept
{
    GT_PROFILE_FUNCTION;

    const auto tmp = *this * vec4f(direction, 0.0f);
    return vec3f(tmp.x, tmp.y, tmp.z);
}

#define GT_UPPER_LEFT_33_VERSION 1

mat3f mat4f::upperLeft33() const
{
    GT_PROFILE_FUNCTION;

#if GT_UPPER_LEFT_33_VERSION == 1

    return mat3f(m00, m01, m02, m10, m11, m12, m20, m21, m22);

#else // GT_UPPER_LEFT_33_VERSION

    return mat3f(vec3f(data[0]), vec3f(data[1]), vec3f(data[2]));

#endif // GT_UPPER_LEFT_33_VERSION
}

std::ostream& operator<<(std::ostream& os, const mat4f& m)
{
    GT_PROFILE_FUNCTION;

#ifdef BOOST_MSVC

    return os << m.m00 << ' ' << m.m10 << ' ' << m.m20 << ' ' << m.m30 << ' '
              << m.m01 << ' ' << m.m11 << ' ' << m.m21 << ' ' << m.m31 << ' '
              << m.m02 << ' ' << m.m12 << ' ' << m.m22 << ' ' << m.m32 << ' '
              << m.m03 << ' ' << m.m13 << ' ' << m.m23 << ' ' << m.m33;

#else

    const auto lTransposedCopy = mat4f(m).transpose();
    std::aligned_storage<4 * sizeof(float), 16>::type lFloats;
    std::char_traits<std::stringstream::char_type>::pos_type lFieldWidth{0};
    for (int i = 0; i < 4; ++i)
    {
        _mm_store_ps(reinterpret_cast<float*>(&lFloats),
                     lTransposedCopy.data[i]);
        for (int j = 0; j < 4; ++j)
        {
            std::stringstream lField;
            lField << *(reinterpret_cast<float*>(&lFloats) + j);
            lField.seekg(0, std::ios::end);
            if (lFieldWidth < lField.tellg()) lFieldWidth = lField.tellg();
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        _mm_store_ps(reinterpret_cast<float*>(&lFloats),
                     lTransposedCopy.data[i]);
        os << '[' << std::setw(lFieldWidth)
           << *(reinterpret_cast<float*>(&lFloats) + 0) << ' '
           << std::setw(lFieldWidth)
           << *(reinterpret_cast<float*>(&lFloats) + 1) << ' '
           << std::setw(lFieldWidth)
           << *(reinterpret_cast<float*>(&lFloats) + 2) << ' '
           << std::setw(lFieldWidth)
           << *(reinterpret_cast<float*>(&lFloats) + 3) << ']';
        if (i != 3) os << '\n';
    }

    return os;

#endif
}
