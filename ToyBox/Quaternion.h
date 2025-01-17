#pragma once
#include "Vector3.h"
#include "MathUtils.h"

#include <cmath>

namespace MOON {
	extern class Matrix4x4;
	class Quaternion {
	public:
		float x;
		float y;
		float z;
		float w;
		Vector3 eulerAngles;

		Quaternion() : x(0), y(0), z(0), w(1) {}
		Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {
			normalize();
			UpdateEulerAngle();
		}
		Quaternion(float yaw, float pitch, float roll) {
			this->SetEulerAngle(yaw, pitch, roll);
		}
		Quaternion(Vector3 eulerAngles) {
			this->SetEulerAngle(eulerAngles.x, eulerAngles.y, eulerAngles.z);
		}
		Quaternion(const Matrix4x4 &mat);
		Quaternion(const Quaternion& q) : x(q.x), y(q.y), z(q.z), w(q.w), eulerAngles(q.eulerAngles) {}
		~Quaternion() {}

		static Quaternion Pow(const Quaternion& q, const float& t);
		static Quaternion identity() { return Quaternion(0, 0, 0, 1); }
		static float Dot(const Quaternion &lhs, const Quaternion &rhs);
		static Quaternion Lerp(const Quaternion &a, const Quaternion &b, float t);
		static Quaternion Slerp(const Quaternion &a, const Quaternion &b, float t);
		static float Angle(const Quaternion &lhs, const Quaternion &rhs);
		static Quaternion Normalize(Quaternion &q);

		void SetEulerAngle(float yaw, float pitch, float roll);
		void SetValue(float _x, float _y, float _z, float _w);
		void SetValue(const Quaternion &q);


		float MagSquared() const;
		float magnitude() const;
		Quaternion Conjugate() const;
		Quaternion Inverse() const;
		Vector3 EulerAngle() const;


		Quaternion& operator^=(const float &pow);
		Quaternion& operator+=(const Quaternion &q);
		Quaternion& operator-=(const Quaternion &q);
		Quaternion& operator*=(const Quaternion &q);
		Quaternion& operator*=(float s);
		Quaternion& operator/=(float s);

		friend bool operator!=(const Quaternion &q1, const Quaternion &q2);
		friend bool operator==(const Quaternion &q1, const Quaternion &q2);
		friend Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs);
		friend Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs);
		friend Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs);
		friend Quaternion operator/(const Quaternion& lhs, const Quaternion& rhs);

		friend Vector3 operator*(const Quaternion& q, const Vector3& point);

		friend Quaternion operator^(const Quaternion& q, const float& pow);
		friend Quaternion operator*(const Quaternion& q, const float& s);
		friend Quaternion operator/(const Quaternion& q, const float& s);
		friend Quaternion operator*(const float& s, const Quaternion& q);
		friend Quaternion operator/(const float& s, const Quaternion& q);

		void normalize();
		void UpdateEulerAngle();

		// angle is in radians
		inline static Quaternion Rotate(const Vector3 &axis, float angle) {
			angle /= 2.0f;
			float sint = sinf(angle);
			return Quaternion(axis.x * sint, axis.y * sint, axis.z * sint, cosf(angle));
		}

		// * When the angle between from and to is 180 degrees,
		// the rotate axis is uncertain, so you need to determine the axis180 on your own
		inline static Quaternion Rotate(const Vector3 &from, const Vector3 &to, const Vector3& axis180 = Vector3::WORLD(UP)) {
			auto angle = Vector3::Angle(from, to);
			if (!angle) return Quaternion::identity();
			else if (from == -to) {
				return Quaternion::Rotate(axis180, Deg2Rad * 180);
			} else {
				auto axis = from.cross(to);
				axis.normalize();
				return Quaternion::Rotate(axis, angle);
			}
		}
	};
}