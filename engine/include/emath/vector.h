#pragma once

#include <vector>
#include "../utilities/macro.h"

namespace Math
{
	class Quaternion;

	class ENIGMA_API Vector2D
	{
	public:
		float x;
		float y;

		Vector2D();
		Vector2D(float _x, float _y);
		Vector2D(float n);
		Vector2D(const Vector2D& p1, const Vector2D& p2);
		~Vector2D() = default;

		float Magnitude() const;
		float SquaredMagnitude() const;
		Vector2D ClampMagnitude(float maxLength) const;

		void Normalize();
		Vector2D Normalized() const;

		float DotProduct(const Vector2D& v) const;

		float Distance(const Vector2D& v) const;
		float Angle(const Vector2D& v) const;
		Vector2D Opposite() const;
		static Vector2D Lerp(Vector2D start, Vector2D end, float t);
		Vector2D MidPoint(const Vector2D& v) const;

		Vector2D Translate(const Vector2D& t) const;
		Vector2D Translate(float dx, float dy) const;
		Vector2D Rotate(float angle) const;
		Vector2D RotateAround(float angle, const Vector2D& p) const;
		Vector2D Scale(float s) const;
		Vector2D Scale(const Vector2D& s) const;
		Vector2D ScaleAround(float s, const Vector2D& p) const;
		Vector2D ScaleAround(const Vector2D& s, const Vector2D& p) const;

		void Print() const;

		#pragma region Vector2D operators

		Vector2D operator+(const Vector2D& v) const;
		Vector2D operator-(const Vector2D& v) const;
		Vector2D operator*(const Vector2D& v) const;
		Vector2D operator/(const Vector2D& v) const;

		void operator+=(const Vector2D& v);
		void operator-=(const Vector2D& v);
		void operator*=(const Vector2D& v);
		void operator/=(const Vector2D& v);

		Vector2D operator+(const float f) const;
		Vector2D operator-(const float f) const;
		Vector2D operator*(const float f) const;
		Vector2D operator/(const float f) const;

		void operator+=(const float f);
		void operator-=(const float f);
		void operator*=(const float f);
		void operator/=(const float f);

		Vector2D operator-() const;

		float& operator[](int index);
		const float& operator[](int index) const;

		bool operator==(const Vector2D& v) const;

		#pragma endregion

		static const Vector2D Zero;
		static const Vector2D One;
		static const Vector2D Up;
		static const Vector2D Down;
		static const Vector2D Left;
		static const Vector2D Right;
	};
	inline Vector2D operator*(float s, const Vector2D& v) { return v * s; };

	class Vector4D;

	class ENIGMA_API Vector3D
	{
	public:
		float x;
		float y;
		float z;

		Vector3D();
		Vector3D(float _x, float _y, float _z);
		Vector3D(float n);
		Vector3D(const Vector3D& p1, const Vector3D& p2);
		Vector3D(const Vector4D& v);
		~Vector3D() = default;

		float Magnitude() const;
		float SquaredMagnitude() const;
		Vector3D ClampMagnitude(float max) const;

		Vector3D Normalized() const;
		void Normalize();

		float DotProduct(const Vector3D& v) const;
		Vector3D CrossProduct(const Vector3D& v) const;

		float Distance(const Vector3D& v) const;
		float Angle(const Vector3D& v) const;
		Vector3D Opposite() const;
		static Vector3D Lerp(const Vector3D& start, const Vector3D& end, float t);
		Vector3D MidPoint(const Vector3D& v) const;
		static Vector3D GetSafeUpVector(Vector3D dir);

		Vector3D Translate(const Vector3D& t) const;
		Vector3D Translate(float dx, float dy, float dz) const;
		Vector3D Rotate(float angle, const Vector3D& axe) const;
		Vector3D Rotate(const Quaternion& q) const;
		Vector3D RotateAround(float angle, const Vector3D& axe, const Vector3D& p) const;
		Vector3D RotateAround(const Quaternion& q, const Vector3D& p) const;
		Vector3D Scale(float s) const;
		Vector3D Scale(const Vector3D& s) const;
		Vector3D ScaleAround(float s, const Vector3D& p) const;
		Vector3D ScaleAround(const Vector3D& s, const Vector3D& p) const;

		void Print() const;

		#pragma region Vector3D operators

		Vector3D operator+(const Vector3D& v)const;
		Vector3D operator-(const Vector3D& v)const;
		Vector3D operator*(const Vector3D& v)const;
		Vector3D operator/(const Vector3D& v)const;

		void operator+=(const Vector3D& v);
		void operator-=(const Vector3D& v);
		void operator*=(const Vector3D& v);
		void operator/=(const Vector3D& v);

		Vector3D operator+(const float f) const;
		Vector3D operator-(const float f) const;
		Vector3D operator*(const float f) const;
		Vector3D operator/(const float f) const;

		void operator+=(const float f);
		void operator-=(const float f);
		void operator*=(const float f);
		void operator/=(const float f);

		Vector3D operator-() const;

		float& operator[](int index);
		const float& operator[](int index) const;
		bool operator==(const Vector3D& v) const;

		#pragma endregion

		static const Vector3D Zero;
		static const Vector3D One;
		static const Vector3D Up;
		static const Vector3D Down;
		static const Vector3D Left;
		static const Vector3D Right;
		static const Vector3D Forward;
		static const Vector3D Backward;
	};
	inline Vector3D operator*(float s, const Vector3D& v) { return v * s; };

	class ENIGMA_API Vector4D
	{
	public:
		float x = 0.f;
		float y = 0.f;
		float z = 0.f;
		float w = 0.f;

		Vector4D();
		Vector4D(float _x, float _y, float _z, float _w);
		Vector4D(const Vector3D& v, float _w);
		~Vector4D() = default;

		void Print() const;
		void Normalize();
		Vector4D Normalized() const;
		float Distance(const Vector4D& v) const;
		float SquaredMagnitude() const;
		float Magnitude() const;
		float DotProduct(const Vector4D& v) const;

		Vector4D Opposite() const;
		Vector4D MidPoint(const Vector4D& v) const;

		Vector4D Translate(const Vector4D& t) const;
		Vector4D Translate(float dx, float dy, float dz, float dw) const;
		Vector4D Scale(float s) const;
		Vector4D Scale(const Vector4D& s) const;

		static Vector4D Lerp(const Vector4D& start, const Vector4D& end, float t);

		#pragma region Vector4D operators

		Vector4D operator+(const Vector4D& v) const;
		Vector4D operator-(const Vector4D& v) const;
		Vector4D operator*(const Vector4D& v) const;
		Vector4D operator/(const Vector4D& v) const;

		void operator+=(const Vector4D& v);
		void operator-=(const Vector4D& v);
		void operator*=(const Vector4D& v);
		void operator/=(const Vector4D& v);

		Vector4D operator+(const float f) const;
		Vector4D operator-(const float f) const;
		Vector4D operator*(const float f) const;
		Vector4D operator/(const float f) const;

		void operator+=(const float f);
		void operator-=(const float f);
		void operator*=(const float f);
		void operator/=(const float f);

		Vector4D operator-();

		float& operator[](int index);
		const float& operator[](int index) const;
		bool operator==(const Vector4D& v) const;

		#pragma endregion

		static const Vector4D Zero;
		static const Vector4D One;
	};
	inline Vector4D operator*(float s, const Vector4D& v) { return v * s; };
}