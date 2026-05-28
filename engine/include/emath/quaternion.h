#pragma once

#include "vector.h"
#include "matrix.h"
#include "../utilities/macro.h"

namespace Math
{
	class ENIGMA_API Quaternion
	{
	public:
		float w;
		float x;
		float y;
		float z;

		Quaternion();
		Quaternion(float _x, float _y, float _z, float _w);
		Quaternion(const Vector4D& v);

		void Normalize();
		float Magnitude() const;
		float Dot(const Quaternion& q2) const;
		float Angle(Quaternion q);
		void Print();
		Quaternion Opposite();
		Quaternion Conjugate();
		Quaternion Normalized();
		Quaternion Inverse() const;

		Quaternion Add(Quaternion q);
		Vector3D RotateVector(const Vector3D& v) const;
		Matrix3x3 ToRotMat3();
		Matrix4x4 ToRotMat4();
		Vector3D ToEuler();
		Vector3D Forward() const;
		Vector3D Right() const;
		Vector3D Up() const;


		static Quaternion FromRotationMatrix(const Matrix3x3& m);
		static Matrix4x4 TRS(Vector3D translation, Quaternion q, Vector3D scale);
		static Quaternion FromAxisAngle(const Vector3D& axis, float angleDeg);
		static Quaternion FromEuler(const Vector3D& eulerDeg);
		static Quaternion Slerp(Quaternion q1, Quaternion q2, float t);
		static Quaternion Nlerp(Quaternion q1, Quaternion q2, float t);

		Quaternion operator+(const Quaternion& q) const;
		Quaternion operator-(const Quaternion& q) const;
		Quaternion operator*(float f) const;
		Quaternion operator*(const Quaternion& q) const;
		Vector3D operator*(const Vector3D& v) const;
		Quaternion operator-();

		static const Quaternion Identity;
	};
}