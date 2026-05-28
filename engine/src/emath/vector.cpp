#include "emath/vector.h"
#include "emath/matrix.h"
#include "emath/utility.h"
#include "emath/quaternion.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <rttr/registration.h>

RTTR_REGISTRATION
{
    rttr::registration::class_<Math::Vector2D>("Vector2D")
        .property("x", &Math::Vector2D::x)
        .property("y", &Math::Vector2D::y);

    rttr::registration::class_<Math::Vector3D>("Vector3D")
        .property("x", &Math::Vector3D::x)
        .property("y", &Math::Vector3D::y)
        .property("z", &Math::Vector3D::z);

    rttr::registration::class_<Math::Vector4D>("Vector4D")
        .property("x", &Math::Vector4D::x)
        .property("y", &Math::Vector4D::y)
        .property("z", &Math::Vector4D::z)
        .property("w", &Math::Vector4D::w);
}

#pragma region Math::Vector2D

const Math::Vector2D Math::Vector2D::Up = Math::Vector2D(0.f, 1.f);
const Math::Vector2D Math::Vector2D::Down = Math::Vector2D(0.f, -1.f);
const Math::Vector2D Math::Vector2D::Right = Math::Vector2D(1.f, 0.f);
const Math::Vector2D Math::Vector2D::Left = Math::Vector2D(-1.f, 0.f);
const Math::Vector2D Math::Vector2D::Zero = Math::Vector2D(0.f, 0.f);
const Math::Vector2D Math::Vector2D::One = Math::Vector2D(1.f, 1.f);

Math::Vector2D::Vector2D()
    : x(0.f), y(0.f)
{}

Math::Vector2D::Vector2D(float _x, float _y)
    : x(_x), y(_y)
{}

Math::Vector2D::Vector2D(float n)
    : x(n), y(n)
{}


Math::Vector2D::Vector2D(const Math::Vector2D& p1, const Math::Vector2D& p2)
    :x(p2.x - p1.x), y(p2.y - p1.y)
{}
Math::Vector2D Math::Vector2D::Opposite() const
{
    return { -x, -y };
}

Math::Vector2D Math::Vector2D::MidPoint(const Math::Vector2D& v) const
{
    return Math::Vector2D((x + v.x) / 2, (y + v.y) / 2);
}

float Math::Vector2D::Distance(const Math::Vector2D& v) const
{
    float dx = x - v.x;
    float dy = y - v.y;
    return sqrtf(dx * dx + dy * dy);
}

float Math::Vector2D::SquaredMagnitude() const
{
    return x * x + y * y;
}

float Math::Vector2D::Magnitude() const
{
    return sqrtf(SquaredMagnitude());
}

Math::Vector2D Math::Vector2D::Normalized() const
{
    float norm = Magnitude();
    if (IsEqualZero(norm))
        return Math::Vector2D(0.f, 0.f);

    float invNorm = 1.f / norm;
	return Math::Vector2D(x * invNorm, y * invNorm);
}

void Math::Vector2D::Normalize()
{
    float norm = Magnitude();
    if (IsEqualZero(norm))
        return;

    float invNorm = 1.f / norm;
    x *= invNorm;
    y *= invNorm;
}

Math::Vector2D Math::Vector2D::ClampMagnitude(float maxLength) const
{
    float n = Magnitude();
    if (n > maxLength)
        return Normalized() * maxLength;
    return *this;
}

float Math::Vector2D::DotProduct(const Math::Vector2D& v) const
{
    return x * v.x + y * v.y;
}

float Math::Vector2D::Angle(const Math::Vector2D& v) const
{
    float norm = Magnitude();
    float vMagnitude = v.Magnitude();
    if (IsEqualZero(norm) || IsEqualZero(vMagnitude))
        return 0.f;

    float cosTheta = DotProduct(v) / (norm * vMagnitude);
    cosTheta = std::clamp(cosTheta, -1.0f, 1.0f);
    return MyToDegrees(std::acos(cosTheta));
}

Math::Vector2D Math::Vector2D::Rotate(float angle) const
{
	float rad = MyToRadians(angle);
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);

    return Math::Vector2D( x * cosA - y * sinA, x * sinA + y * cosA );
}

Math::Vector2D Math::Vector2D::RotateAround(float angle, const Math::Vector2D& p) const
{
    float rad = MyToRadians(angle);
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);

    float tx = x - p.x;
    float ty = y - p.y;

    float rx = tx * cosA - ty * sinA;
    float ry = tx * sinA + ty * cosA;

    return { rx + p.x, ry + p.y };
}

Math::Vector2D Math::Vector2D::Translate(const Math::Vector2D& t) const
{
    return { x + t.x, y + t.y };
}

Math::Vector2D Math::Vector2D::Translate(float dx, float dy) const
{
    return { x + dx, y + dy };
}

Math::Vector2D Math::Vector2D::Scale(float s) const
{
    return { x * s, y * s };
}

Math::Vector2D Math::Vector2D::Scale(const Math::Vector2D& s) const
{
    return { x * s.x, y * s.y };
}

Math::Vector2D Math::Vector2D::ScaleAround(float s, const Math::Vector2D& p) const
{
    return (*this - p) * s + p;
}

Math::Vector2D Math::Vector2D::ScaleAround(const Math::Vector2D& s, const Math::Vector2D& p) const
{
    return (*this - p) * s + p;
}

void Math::Vector2D::Print() const
{
    std::cout << "(" << (IsEqualZero(x) ? 0.f : x) << ", " << (IsEqualZero(y) ? 0.f : y) << ")" << std::endl;
}

Math::Vector2D Math::Vector2D::Lerp(Math::Vector2D start, Math::Vector2D end, float t)
{
    return start + (end - start) * t;
}

#pragma region Math::Vector2D Operators

Math::Vector2D Math::Vector2D::operator+(const Math::Vector2D& v) const
{
    return Math::Vector2D(x + v.x, y + v.y);
}

Math::Vector2D Math::Vector2D::operator-(const Math::Vector2D& v) const
{
    return Math::Vector2D(this->x - v.x, this->y - v.y);
}

Math::Vector2D Math::Vector2D::operator*(const Math::Vector2D& v) const
{
    return Math::Vector2D(this->x * v.x, this->y * v.y);
}

Math::Vector2D Math::Vector2D::operator/(const Math::Vector2D& v) const
{
    return Math::Vector2D(IsEqualZero(v.x) ? 0.f : this->x / v.x, IsEqualZero(v.y) ? 0.f : this->y / v.y);
}

void Math::Vector2D::operator+=(const Math::Vector2D& v)
{
	x += v.x;
	y += v.y;
}

void Math::Vector2D::operator-=(const Math::Vector2D& v)
{
	x -= v.x;
	y -= v.y;
}

void Math::Vector2D::operator*=(const Math::Vector2D& v)
{
	x *= v.x;
	y *= v.y;
}

void Math::Vector2D::operator/=(const Math::Vector2D& v)
{
	IsEqualZero(v.x) ? x = 0.f : x /= v.x;
    IsEqualZero(v.y) ? y = 0.f : y /= v.y;
}

Math::Vector2D Math::Vector2D::operator+(const float f) const
{
    return Math::Vector2D(x + f, y + f);
}

Math::Vector2D Math::Vector2D::operator-(const float f) const
{
    return Math::Vector2D(x - f, y - f);
}

Math::Vector2D Math::Vector2D::operator*(const float f) const
{
    return Math::Vector2D(x * f, y * f);
}

Math::Vector2D Math::Vector2D::operator/(const float f) const
{
    Math::Vector2D r;
    IsEqualZero(f) ? r = Math::Vector2D::Zero : r = Math::Vector2D(x / f, y / f);
    return r;
}

void Math::Vector2D::operator+=(const float f)
{
	x += f;
	y += f;
}

void Math::Vector2D::operator-=(const float f)
{
	x -= f;
	y -= f;
}

void Math::Vector2D::operator*=(const float f)
{
	x *= f;
	y *= f;
}

void Math::Vector2D::operator/=(const float f)
{
    if (IsEqualZero(f))
    {
        x = 0.f;
        y = 0.f;
    }
    else
    {
        x /= f;
        y /= f;
    }
}

float& Math::Vector2D::operator[](int index)
{
	return index == 0 ? x : y;
}

const float& Math::Vector2D::operator[](int index) const
{
    return index == 0 ? x : y;
}

bool Math::Vector2D::operator==(const Math::Vector2D& v) const
{
    return IsEqual(x, v.x) && IsEqual(y, v.y);
}

Math::Vector2D Math::Vector2D::operator-() const
{
	return Opposite();
}

#pragma endregion

#pragma endregion

#pragma region Math::Vector3D

const Math::Vector3D Math::Vector3D::Up = Math::Vector3D(0.f, 1.f, 0.f);
const Math::Vector3D Math::Vector3D::Down = Math::Vector3D(0.f, -1.f, 0.f);
const Math::Vector3D Math::Vector3D::Right = Math::Vector3D(1.f, 0.f, 0.f);
const Math::Vector3D Math::Vector3D::Left = Math::Vector3D(-1.f, 0.f, 0.f);
const Math::Vector3D Math::Vector3D::Forward = Math::Vector3D(0.f, 0.f, -1.f);
const Math::Vector3D Math::Vector3D::Backward = Math::Vector3D(0.f, 0.f, 1.f);
const Math::Vector3D Math::Vector3D::Zero = Math::Vector3D(0.f, 0.f, 0.f);
const Math::Vector3D Math::Vector3D::One = Math::Vector3D(1.f, 1.f, 1.f);

Math::Vector3D::Vector3D()
    :x(0.f), y(0.f), z(0.f)
{}

Math::Vector3D::Vector3D(float _x, float _y, float _z)
    : x(_x), y(_y), z(_z)
{}

Math::Vector3D::Vector3D(float n)
    :x(n), y(n), z(n)
{}

Math::Vector3D::Vector3D(const Math::Vector3D& p1, const Math::Vector3D& p2)
    :x(p2.x - p1.x), y(p2.y - p1.y), z(p2.z - p1.z)
{}

Math::Vector3D::Vector3D(const Vector4D & v)
    :x(v.x), y(v.y), z(v.z)
{
}

Math::Vector3D Math::Vector3D::Opposite() const
{
    return Math::Vector3D(-x, -y, -z);
}

Math::Vector3D Math::Vector3D::MidPoint(const Math::Vector3D& v) const
{
    return Math::Vector3D((x + v.x) / 2, (y + v.y) / 2, (z + v.z) / 2);
}

float Math::Vector3D::Distance(const Math::Vector3D& v) const
{
    float dx = x - v.x;
    float dy = y - v.y;
    float dz = z - v.z;
    return sqrtf(dx * dx + dy * dy + dz * dz);
}

float Math::Vector3D::SquaredMagnitude() const
{
    return x * x + y * y + z * z;
}

float Math::Vector3D::Magnitude() const
{
    return sqrtf(SquaredMagnitude());
}

Math::Vector3D Math::Vector3D::Normalized() const
{
    float norm = Magnitude();
    if (IsEqualZero(norm))
        return Math::Vector3D::Zero;

    float invNorm = 1.f / norm;
    return Math::Vector3D(x * invNorm, y * invNorm, z * invNorm);
}

void Math::Vector3D::Normalize()
{
    float norm = Magnitude();
    if (IsEqualZero(norm))
        return;

    float invNorm = 1.f / norm;
    x *= invNorm;
    y *= invNorm;
	z *= invNorm;
}

float Math::Vector3D::DotProduct(const Math::Vector3D& v) const
{
    return x * v.x + y * v.y + z * v.z;
}

Math::Vector3D Math::Vector3D::CrossProduct(const Math::Vector3D& v) const
{
    return Math::Vector3D(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

float Math::Vector3D::Angle(const Math::Vector3D& v) const
{
    float norm = Magnitude();
    float vMagnitude = v.Magnitude();
    if (IsEqualZero(norm) || IsEqualZero(vMagnitude))
        return 0.f;

    float cosTheta = DotProduct(v) / (norm * vMagnitude);
    cosTheta = std::clamp(cosTheta, -1.0f, 1.0f);
    return MyToDegrees(std::acos(cosTheta));
}

Math::Vector3D Math::Vector3D::GetSafeUpVector(Math::Vector3D dir) 
{
    Math::Vector3D absDir = { std::abs(dir.x), std::abs(dir.y), std::abs(dir.z) };

    if (absDir.x <= absDir.y && absDir.x <= absDir.z)
        return Math::Vector3D(1, 0, 0);
    if (absDir.y <= absDir.x && absDir.y <= absDir.z)
        return Math::Vector3D(0, 1, 0);
    return Math::Vector3D(0, 0, 1);
}

void Math::Vector3D::Print() const
{
    std::cout << "(" << (IsEqualZero(x) ? 0.f : x) << ", " << (IsEqualZero(y) ? 0.f : y) << ", " << (IsEqualZero(z) ? 0.f : z) << ")" << std::endl;
}

Math::Vector3D Math::Vector3D::ClampMagnitude(float max) const
{
    float sqrMag = SquaredMagnitude();
    float maxSqr = max * max;

    if (sqrMag > maxSqr)
    {
        float invMag = 1.0f / sqrtf(sqrMag);
        return Math::Vector3D(x * invMag * max,
            y * invMag * max,
            z * invMag * max);
    }
    return *this;
}

Math::Vector3D Math::Vector3D::Translate(const Math::Vector3D& t) const
{
    return { x + t.x, y + t.y, z + t.z };
}

Math::Vector3D Math::Vector3D::Translate(float dx, float dy, float dz) const
{
    return { x + dx, y + dy, z + dz };
}

Math::Vector3D Math::Vector3D::Rotate(float angle, const Math::Vector3D& axe) const
{
    Quaternion q = Quaternion::FromAxisAngle(axe, angle);
    return q.RotateVector(*this);
}

Math::Vector3D Math::Vector3D::Rotate(const Quaternion& q) const
{
    return q.RotateVector(*this);
}

Math::Vector3D Math::Vector3D::RotateAround(float angle, const Math::Vector3D& axis, const Math::Vector3D& p) const
{
    Quaternion q = Quaternion::FromAxisAngle(axis, angle);

    Math::Vector3D translated = *this - p;
    Math::Vector3D rotated = q.RotateVector(translated);

    return rotated + p;
}

Math::Vector3D Math::Vector3D::RotateAround(const Quaternion& q, const Math::Vector3D& p) const
{
    Math::Vector3D translated = *this - p;
    Math::Vector3D rotated = q.RotateVector(translated);
    return rotated + p;
}

Math::Vector3D Math::Vector3D::Scale(float s) const
{
    return { x * s, y * s, z * s };
}

Math::Vector3D Math::Vector3D::Scale(const Math::Vector3D& v) const
{
    return { x * v.x, y * v.y, z * v.z };
}

Math::Vector3D Math::Vector3D::ScaleAround(float s, const Math::Vector3D& p) const
{
    Math::Vector3D t = *this - p;
    return t * s + p;
}

Math::Vector3D Math::Vector3D::ScaleAround(const Math::Vector3D& s, const Math::Vector3D& p) const
{
    Math::Vector3D t = *this - p;
    return t * s + p;
}


Math::Vector3D Math::Vector3D::Lerp(const Math::Vector3D& start, const Math::Vector3D& end, float t)
{
    return start + (end - start) * t;
}

#pragma region Math::Vector3D Operators

Math::Vector3D Math::Vector3D::operator-() const
{
	return Opposite();
}

Math::Vector3D Math::Vector3D::operator+(const Math::Vector3D& v) const
{
    return {x + v.x, y + v.y, z + v.z};
}

Math::Vector3D Math::Vector3D::operator-(const Math::Vector3D& v) const
{
    return { x - v.x, y - v.y, z - v.z };
}

Math::Vector3D Math::Vector3D::operator*(const Math::Vector3D& v) const
{
    return { x * v.x, y * v.y, z * v.z };
}

Math::Vector3D Math::Vector3D::operator/(const Math::Vector3D& v) const
{
    return { IsEqualZero(v.x) ? 0.f : x / v.x, IsEqualZero(v.y) ? 0.f : y / v.y, IsEqualZero(v.z) ? 0.f : z / v.z };
}

void Math::Vector3D::operator+=(const Math::Vector3D& v)
{
    *this = { x + v.x, y + v.y, z + v.z };
}

void Math::Vector3D::operator-=(const Math::Vector3D& v)
{
    *this = { x - v.x, y - v.y, z - v.z };
}

void Math::Vector3D::operator*=(const Math::Vector3D& v)
{
    *this = { x * v.x, y * v.y, z * v.z };
}

void Math::Vector3D::operator/=(const Math::Vector3D& v)
{
    *this = { IsEqualZero(v.x) ? 0.f : x / v.x, IsEqualZero(v.y) ? 0.f : y / v.y, IsEqualZero(v.z) ? 0.f : z / v.z };
}

Math::Vector3D Math::Vector3D::operator+(const float f) const
{
    return { x + f, y + f, z + f };
}

Math::Vector3D Math::Vector3D::operator-(const float f) const
{
    return { x - f, y - f, z - f };
}

Math::Vector3D Math::Vector3D::operator*(const float f) const
{
    return { x * f, y * f, z * f };
}

Math::Vector3D Math::Vector3D::operator/(const float f) const
{
    Math::Vector3D r;
	IsEqualZero(f) ? r = Math::Vector3D::Zero : r = Math::Vector3D(x / f, y / f, z / f);
    return r;
}

void Math::Vector3D::operator+=(const float f)
{
    x += f;
    y += f;
    z += f;
}

void Math::Vector3D::operator-=(const float f)
{
    x -= f;
    y -= f;
    z -= f;
}

void Math::Vector3D::operator*=(const float f)
{
    x *= f;
    y *= f;
    z *= f;
}

void Math::Vector3D::operator/=(const float f)
{
    if (IsEqualZero(f))
        *this = Math::Vector3D::Zero;
    else
    {
        x /= f;
        y /= f;
        z /= f;
    }
}

float& Math::Vector3D::operator[](int index)
{
    switch (index)
    {
    case 0:
        return x;
        break;
    case 1:
        return y;
        break;
    default:
        return z;
        break;
    }
}

const float& Math::Vector3D::operator[](int index) const
{
    switch (index)
    {
    case 0:
        return x;
        break;
    case 1:
        return y;
        break;
    default:
        return z;
        break;
    }
}

bool Math::Vector3D::operator==(const Math::Vector3D& v) const
{
    return IsEqual(x, v.x) && IsEqual(y, v.y) && IsEqual(z, v.z);
}

#pragma endregion

#pragma endregion

#pragma region Math::Vector4D

const Math::Vector4D Math::Vector4D::Zero = Math::Vector4D(0.f, 0.f, 0.f, 0.f);
const Math::Vector4D Math::Vector4D::One = Math::Vector4D(1.f, 1.f, 1.f, 1.f);

Math::Vector4D::Vector4D()
    :x(0.f), y(0.f), z(0.f), w(0.f)
{
}

Math::Vector4D::Vector4D(float _x, float _y, float _z, float _w)
    : x(_x), y(_y), z(_z), w(_w)
{}

Math::Vector4D::Vector4D(const Math::Vector3D& v, float _w) 
    : x(v.x), y(v.y), z(v.z), w(_w)
{}

Math::Vector4D Math::Vector4D::Normalized() const
{
    float norm = Magnitude();
    if (IsEqualZero(norm))
        return Math::Vector4D::Zero;

    float invNorm = 1.f / norm;
    return Math::Vector4D(x * invNorm, y * invNorm, z * invNorm, w * invNorm);
}

void Math::Vector4D::Normalize()
{
    float norm = Magnitude();
    if (IsEqualZero(norm))
        return;

    float invNorm = 1.0f / norm;
    y *= invNorm;
    z *= invNorm;
    x *= invNorm;
    w *= invNorm;
}

Math::Vector4D Math::Vector4D::Opposite() const
{
    return Math::Vector4D(-x, -y, -z, -w);
}

Math::Vector4D Math::Vector4D::MidPoint(const Math::Vector4D& v) const
{
    return Math::Vector4D((x + v.x) / 2, (y + v.y) / 2, (z + v.z) / 2, (w + v.w) / 2);
}

Math::Vector4D Math::Vector4D::Lerp(const Math::Vector4D& start, const Math::Vector4D& end, float t)
{
    return start + (end - start) * t;
}

float Math::Vector4D::Distance(const Math::Vector4D& v) const
{
    return sqrtf(powf(x - v.x, 2) + powf(y - v.y, 2) + powf(z - v.z, 2) + powf(w - v.w, 2));
}

float Math::Vector4D::SquaredMagnitude() const
{
    return x * x + y * y + z * z + w * w;
}

float Math::Vector4D::Magnitude() const
{
    return sqrtf(SquaredMagnitude());
}

float Math::Vector4D::DotProduct(const Math::Vector4D& v) const
{
    return x * v.x + y * v.y + z * v.z + w * v.w;
}

Math::Vector4D Math::Vector4D::Translate(const Math::Vector4D& t) const
{
    return Math::Vector4D(x + t.x, y + t.y, z + t.z, w + t.w);
}

Math::Vector4D Math::Vector4D::Translate(float dx, float dy, float dz, float dw) const
{
    return Math::Vector4D(x + dx, y + dy, z + dz, w + dw);
}

Math::Vector4D Math::Vector4D::Scale(float s) const
{
    return Math::Vector4D(x * s, y * s, z * s, w * s);
}

Math::Vector4D Math::Vector4D::Scale(const Math::Vector4D& s) const
{
    return Math::Vector4D(x * s.x, y * s.y, z * s.z, w * s.w);
}

void Math::Vector4D::Print() const
{
    std::cout << "(" << (IsEqualZero(x) ? 0.f : x) << ", " << (IsEqualZero(y) ? 0.f : y) << ", " << (IsEqualZero(z) ? 0.f : z) << ", " << (IsEqualZero(w) ? 0.f : w) << ")" << std::endl;
}

#pragma region Math::Vector4D Operators

Math::Vector4D Math::Vector4D::operator+(const Math::Vector4D& v) const
{
    return { x + v.x, y + v.y, z + v.z, w + v.w };
}

Math::Vector4D Math::Vector4D::operator-(const Math::Vector4D& v) const
{
    return { x - v.x, y - v.y, z - v.z, w - v.w };
}

Math::Vector4D Math::Vector4D::operator*(const Math::Vector4D& v) const
{
    return { x * v.x, y * v.y, z * v.z, w * v.w };
}

Math::Vector4D Math::Vector4D::operator/(const Math::Vector4D& v) const
{
    return { 
        (IsEqualZero(v.x) ? 0.f : x / v.x), 
        (IsEqualZero(v.y) ? 0.f : y / v.y),
        (IsEqualZero(v.z) ? 0.f : z / v.z),
        (IsEqualZero(v.w) ? 0.f : w / v.w)
    };
}

void Math::Vector4D::operator+=(const Math::Vector4D& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
}

void Math::Vector4D::operator-=(const Math::Vector4D& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
}

void Math::Vector4D::operator*=(const Math::Vector4D& v)
{
    x *= v.x;
    y *= v.y;
    z *= v.z;
    w *= v.w;
}

void Math::Vector4D::operator/=(const Math::Vector4D& v)
{
    IsEqualZero(v.x) ? 0.f : x /= v.x;
    IsEqualZero(v.y) ? 0.f : y /= v.y;
    IsEqualZero(v.z) ? 0.f : z /= v.z;
    IsEqualZero(v.w) ? 0.f : w /= v.w;
}

Math::Vector4D Math::Vector4D::operator+(const float f) const
{
    return { x + f, y + f, z + f, w + f };
}

Math::Vector4D Math::Vector4D::operator-(const float f) const
{
    return { x - f, y - f, z - f, w - f };
}

Math::Vector4D Math::Vector4D::operator*(const float f) const
{
    return { x * f, y * f, z * f, w * f };
}

Math::Vector4D Math::Vector4D::operator/(const float f) const
{
    Math::Vector4D r{};
    IsEqualZero(f) ? r = Math::Vector4D::Zero : r = Math::Vector4D(x / f, y / f, z / f, w / f);
    return r;
}

void Math::Vector4D::operator+=(const float f)
{
    x += f;
    y += f;
    z += f;
    w += f;
}

void Math::Vector4D::operator-=(const float f)
{
    x -= f;
    y -= f;
    z -= f;
    w -= f;
}

void Math::Vector4D::operator*=(const float f)
{
    x *= f;
    y *= f;
    z *= f;
    w *= f;
}

void Math::Vector4D::operator/=(const float f)
{
    IsEqualZero(f) ? *this = Math::Vector4D::Zero : Math::Vector4D(x += f, y += f, z += f, w += f);
}

Math::Vector4D Math::Vector4D::operator-()
{
    return Opposite();
}

float& Math::Vector4D::operator[](int index)
{
    switch (index)
    {
    case 0:
        return x;
        break;
    case 1:
        return y;
        break;
    case 2:
        return z;
        break;
    default:
        return w;
        break;
    }
}

const float& Math::Vector4D::operator[](int index) const
{
    switch (index)
    {
    case 0:
        return x;
        break;
    case 1:
        return y;
        break;
    case 2:
        return z;
        break;
    default:
        return w;
        break;
    }
}

bool Math::Vector4D::operator==(const Math::Vector4D& v) const
{
    return IsEqual(x, v.x) && IsEqual(y, v.y) && IsEqual(z, v.z) && IsEqual(w, v.w);
}

#pragma endregion

#pragma endregion