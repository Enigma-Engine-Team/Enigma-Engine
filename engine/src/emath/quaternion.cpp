#include "emath/quaternion.h"
#include "emath/utility.h"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <rttr/registration.h>

RTTR_REGISTRATION
{
	rttr::registration::class_<Math::Quaternion>("Quaternion")
		 .property("x", &Math::Quaternion::x)
		 .property("y", &Math::Quaternion::y)
		 .property("z", &Math::Quaternion::z)
		 .property("w", &Math::Quaternion::w);
}

const Math::Quaternion Math::Quaternion::Identity = Quaternion(0.f, 0.f, 0.f, 1.f);

Math::Quaternion::Quaternion()
{
	x = 0.f;
	y = 0.f;
	z = 0.f;
	w = 1.f;
}

Math::Quaternion::Quaternion(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

Math::Quaternion::Quaternion(const Vector4D& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
}

Math::Quaternion Math::Quaternion::Add(Quaternion q)
{
	return { x + q.x, y + q.y, z + q.z, w + q.w };
}

Math::Quaternion Math::Quaternion::Conjugate()
{
	return {-x, -y, -z, w};
}

Math::Quaternion Math::Quaternion::Opposite()
{
	return { -x, -y, -z, -w };
}

Math::Vector3D Math::Quaternion::RotateVector(const Vector3D& v) const
{
	Vector3D qVec(x, y, z);

	Vector3D t = qVec.CrossProduct(v) * 2.0f;
	return v + t * w + qVec.CrossProduct(t);
}

Math::Matrix3x3 Math::Quaternion::ToRotMat3()
{
	Quaternion q1 = this->Normalized();

	Matrix3x3 m
	(
		1 - 2 * (q1.y * q1.y + q1.z * q1.z),
		2 * (q1.x * q1.y - q1.z * q1.w),
		2 * (q1.x * q1.z + q1.y * q1.w),

		2 * (q1.x * q1.y + q1.z * q1.w),
		1 - 2 * (q1.x * q1.x + q1.z * q1.z),
		2 * (q1.y * q1.z - q1.x * q1.w),

		2 * (q1.x * q1.z - q1.y * q1.w),
		2 * (q1.y * q1.z + q1.x * q1.w),
		1 - 2 * (q1.x * q1.x + q1.y * q1.y)
	);

	return m;
}

Math::Quaternion Math::Quaternion::FromRotationMatrix(const Matrix3x3& m)
{
	Quaternion q;
	float trace = m.m[0] + m.m[4] + m.m[8];

	if (trace > 0.f)
	{
		float s = 0.5f / sqrtf(trace + 1.f);
		q.w = 0.25f / s;
		q.x = (m.m[7] - m.m[5]) * s;
		q.y = (m.m[2] - m.m[6]) * s;
		q.z = (m.m[3] - m.m[1]) * s;
	}
	else if (m.m[0] > m.m[4] && m.m[0] > m.m[8])
	{
		float s = 2.f * sqrtf(1.f + m.m[0] - m.m[4] - m.m[8]);
		q.w = (m.m[7] - m.m[5]) / s;
		q.x = 0.25f * s;
		q.y = (m.m[1] + m.m[3]) / s;
		q.z = (m.m[2] + m.m[6]) / s;
	}
	else if (m.m[4] > m.m[8])
	{
		float s = 2.f * sqrtf(1.f + m.m[4] - m.m[0] - m.m[8]);
		q.w = (m.m[2] - m.m[6]) / s;
		q.x = (m.m[1] + m.m[3]) / s;
		q.y = 0.25f * s;
		q.z = (m.m[5] + m.m[7]) / s;
	}
	else
	{
		float s = 2.f * sqrtf(1.f + m.m[8] - m.m[0] - m.m[4]);
		q.w = (m.m[3] - m.m[1]) / s;
		q.x = (m.m[2] + m.m[6]) / s;
		q.y = (m.m[5] + m.m[7]) / s;
		q.z = 0.25f * s;
	}

	return q.Normalized();
}

Math::Matrix4x4 Math::Quaternion::ToRotMat4()
{
	Quaternion q1 = this->Normalized();

	Matrix4x4 m
	(
		1.f - 2.f * (q1.y * q1.y + q1.z * q1.z),
		2.f * (q1.x * q1.y - q1.z * q1.w),
		2.f * (q1.x * q1.z + q1.y * q1.w),
		0.f,

		2.f * (q1.x * q1.y + q1.z * q1.w),
		1.f - 2.f * (q1.x * q1.x + q1.z * q1.z),
		2.f * (q1.y * q1.z - q1.x * q1.w),
		0.f,

		2.f * (q1.x * q1.z - q1.y * q1.w),
		2.f * (q1.y * q1.z + q1.x * q1.w),
		1.f - 2.f * (q1.x * q1.x + q1.y * q1.y),
		0.f,

		0.f, 0.f, 0.f, 1.f
	);

	return m;
}

Math::Matrix4x4 Math::Quaternion::TRS(Vector3D translation, Quaternion q, Vector3D scale)
{
	Matrix3x3 r = q.Normalized().ToRotMat3();

	return Matrix4x4(
		r.m[0] * scale.x, r.m[3] * scale.x, r.m[6] * scale.x, 0.0f,
		r.m[1] * scale.y, r.m[4] * scale.y, r.m[7] * scale.y, 0.0f,
		r.m[2] * scale.z, r.m[5] * scale.z, r.m[8] * scale.z, 0.0f,
		translation.x, translation.y, translation.z, 1.0f
	);
}


Math::Quaternion Math::Quaternion::FromAxisAngle(const Vector3D& axis, float angleDeg)
{
	float rad = MyToRadians(angleDeg);
	float half = rad * 0.5f;

	float sinHalf = std::sin(half);
	float cosHalf = std::cos(half);

	Vector3D n = axis.Normalized();

	return Quaternion(n.x * sinHalf, n.y * sinHalf, n.z * sinHalf, cosHalf);
}

float Math::Quaternion::Magnitude() const
{
	return Vector4D(x, y, z, w).Magnitude();
}

void Math::Quaternion::Normalize()
{
	float norm = Magnitude();
	if (IsEqualZero(norm))
		return;

	float invNorm = 1.0f / norm;
	x *= invNorm;
	y *= invNorm;
	z *= invNorm;
	w *= invNorm;
}

Math::Quaternion Math::Quaternion::Normalized()
{
	return Vector4D(x, y, z, w).Normalized();
}

float Math::Quaternion::Dot(const Quaternion& q) const
{
	return x * q.x + y * q.y + z * q.z + w * q.w;
}

Math::Quaternion Math::Quaternion::FromEuler(const Vector3D& eulerDeg)
{
	float xRad = MyToRadians(eulerDeg.x) * 0.5f;
	float yRad = MyToRadians(eulerDeg.y) * 0.5f;
	float zRad = MyToRadians(eulerDeg.z) * 0.5f;

	float cx = cosf(xRad);
	float sx = sinf(xRad);
	float cy = cosf(yRad);
	float sy = sinf(yRad);
	float cz = cosf(zRad);
	float sz = sinf(zRad);

	return Quaternion(
		sx * cy * cz - cx * sy * sz,
		cx * sy * cz + sx * cy * sz,
		cx * cy * sz - sx * sy * cz,
		cx * cy * cz + sx * sy * sz
	);
}

Math::Vector3D Math::Quaternion::ToEuler()
{
	Vector3D euler;

	float sinr_cosp = 2.0f * (w * x + y * z);
	float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
	euler.x = std::atan2(sinr_cosp, cosr_cosp);

	float sinp = 2.0f * (w * y - z * x);
	if (std::abs(sinp) >= 1)
		euler.y = std::copysign(PI / 2.0f, sinp);
	else
		euler.y = std::asin(sinp);

	float siny_cosp = 2.0f * (w * z + x * y);
	float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
	euler.z = std::atan2(siny_cosp, cosy_cosp);

	euler.x = MyToDegrees(euler.x);
	euler.y = MyToDegrees(euler.y);
	euler.z = MyToDegrees(euler.z);

	return euler;
}

Math::Vector3D Math::Quaternion::Forward() const
{
	return {
		-2.f * (x * z + w * y),
		-2.f * (y * z - w * x),
		-1.f + 2.f * (x * x + y * y)
	};
}

Math::Vector3D Math::Quaternion::Right() const
{
	return {
		1.f - 2.f * (y * y + z * z),
		2.f * (x * y + w * z),
		2.f * (x * z - w * y)
	};
}

Math::Vector3D Math::Quaternion::Up() const
{
	return {
		2.f * (x * y - w * z),
		1.f - 2.f * (x * x + z * z),
		2.f * (y * z + w * x)
	};
}

float Math::Quaternion::Angle(Quaternion q)
{
	float dot = Dot(q.Normalized());
	return acosf(std::min(std::max(dot, -1.0f), 1.0f)) * 2.0f;
}

void Math::Quaternion::Print()
{
	std::cout << x << " ; " << y << " ; " << z << " ; " << w << std::endl;
}

Math::Quaternion Math::Quaternion::Inverse() const
{
	float normSq = x * x + y * y + z * z + w * w;
	if (IsEqualZero(normSq))
		return Math::Quaternion::Identity;

	float inv = 1.0f / normSq;
	return Quaternion(-x * inv, -y * inv, -z * inv, w * inv);
}

Math::Quaternion Math::Quaternion::Slerp(Quaternion q1, Quaternion q2, float t)
{
	float dot = q1.Dot(q2);

	dot = std::min(dot, 1.0f);
	dot = std::max(dot, -1.0f);

	if (dot < 0.0f)
	{
		q2 = q2.Opposite();
		dot = -dot;
	}

	if (dot > 0.9995f) {
		Quaternion result = Nlerp(q1, q2, t);
		return result;
	}

	float omega = acos(dot);
	float sinOmega = sin(omega);
	float a = sin((1.0f - t) * omega) / sinOmega;
	float b = sin(t * omega) / sinOmega;

	Quaternion result = q1 * a + q2 * b;
	return result * result.Magnitude();
}

Math::Quaternion Math::Quaternion::Nlerp(Quaternion q1, Quaternion q2, float t)
{
	return (q1 * (1-t) + q2 * t);
}

#pragma region Operators

Math::Quaternion Math::Quaternion::operator+(const Quaternion& q) const
{
	return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
}

Math::Quaternion Math::Quaternion::operator-(const Quaternion& q) const
{
	return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w);
}

Math::Quaternion Math::Quaternion::operator*(float f) const
{
	return Quaternion(x * f, y * f, z * f, w * f);
}

Math::Quaternion Math::Quaternion::operator*(const Quaternion& q) const
{
	return
	{
		w * q.x + q.w * x + (y * q.z - z * q.y),
		w * q.y + q.w * y + (z * q.x - x * q.z),
		w * q.z + q.w * z + (x * q.y - y * q.x),
		w * q.w - (x * q.x + y * q.y + z * q.z)
	};
}

Math::Vector3D Math::Quaternion::operator*(const Vector3D& v) const
{
	Vector3D cross = Vector3D(x, y, z).CrossProduct(v);
	Vector3D t = 2.f * cross;

	return v + w * t + cross;
}

Math::Quaternion Math::Quaternion::operator-()
{
	return Opposite();
}

#pragma endregion