#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Quaternion.h"

namespace MOON {
	Vector3::Vector3(const Vector2 &v) {
		x = v.x; y = v.y; z = 1;
	}

	Vector3::Vector3(const Vector4 &v) {
		x = v.x; y = v.y; z = v.z;
	}

	bool operator!=(const Vector3 &v1, const Vector3 &v2) {
		return !(v1 == v2);
	}

	bool operator==(const Vector3 &v1, const Vector3 &v2) {
		if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z) return true;
		else return false;
	}

	std::istream& operator>>(std::istream &is, Vector3 &t) {
		is >> t.x >> t.y >> t.z;
		return is;
	}

	std::ostream& operator<<(std::ostream &os, const Vector3 &t) {
		os << "(" << t.x << ", " << t.y << ", " << t.z << ")";
		return os;
	}

	Vector3 operator+(const Vector3 &v1, const Vector3 &v2) {
		return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
	}

	Vector3 operator-(const Vector3 &v1, const Vector3 &v2) {
		return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
	}

	Vector3 operator*(const Vector3 &v1, const Vector3 &v2) {
		return Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
	}

	Vector3 operator/(const Vector3 &v1, const Vector3 &v2) {
		return Vector3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
	}

	Vector3 operator*(const float &t, const Vector3 &v) {
		return Vector3(t * v.x, t * v.y, t * v.z);
	}

	Vector3 operator/(const float &t, const Vector3 &v) {
		return Vector3(t / v.x, t / v.y, t / v.z);
	}

	Vector3 operator*(const Vector3 &v, const float &t) {
		return Vector3(t * v.x, t * v.y, t * v.z);
	}

	Vector3 operator/(const Vector3 &v, const float &t) {
		return Vector3(v.x / t, v.y / t, v.z / t);
	}

	Vector3& Vector3::operator+=(const Vector3 &v) {
		x += v.x; y += v.y; z += v.z;
		return *this;
	}

	Vector3& Vector3::operator-=(const Vector3 &v) {
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}

	Vector3& Vector3::operator*=(const Vector3 &v) {
		x *= v.x; y *= v.y; z *= v.z;
		return *this;
	}

	Vector3& Vector3::operator/=(const Vector3 &v) {
		x /= v.x; y /= v.y; z /= v.z;
		return *this;
	}

	Vector3& Vector3::operator*=(const float &t) {
		x *= t; y *= t; z *= t;
		return *this;
	}

	Vector3& Vector3::operator/=(const float &t) {
		x /= t; y /= t; z /= t;
		return *this;
	}

	Vector3 Vector3::Normalize(const Vector3 &v) {
		float len = 1.0f / v.magnitude();

		return Vector3(v.x, v.y, v.z) * len;
	}

	float Vector3::Dot(const Vector3 &v1, const Vector3 &v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	Vector3 Vector3::Cross(const Vector3 &v1, const Vector3 &v2) {
		return Vector3(
			v1.y * v2.z - v1.z * v2.y,
			v1.z * v2.x - v1.x * v2.z,
			v1.x * v2.y - v1.y * v2.x
		);
	}

	// return angle in radians
	float Vector3::Angle(const Vector3 &a, const Vector3 &b) {
		//float angle = a.dot(b);
		//angle /= (a.magnitude() * b.magnitude());
		//return angle = acosf(angle);
		return acosf(Vector3::Normalize(a).dot(Vector3::Normalize(b)));
	}

	Vector3 Vector3::Projection(const Vector3 &a, const Vector3 &b) {
		Vector3 bn = b / b.magnitude();
		return bn * a.dot(bn);
	}

	Vector3 Vector3::ONE() {
		return Vector3(1.0, 1.0, 1.0);
	}

	Vector3 Vector3::ZERO() {
		return Vector3(0.0, 0.0, 0.0);
	}

	Vector3 Vector3::Decomposition(
		const Vector3& vector,
		Vector3 xAxis, Vector3 yAxis, Vector3 zAxis,
		Quaternion coordRotation = Quaternion::identity()) {
		if (coordRotation != Quaternion::identity()) {
			xAxis = coordRotation * xAxis;
			yAxis = coordRotation * yAxis;
			zAxis = coordRotation * zAxis;
		}
		auto xProj = Vector3::ProjectionMag(vector, xAxis);
		auto yProj = Vector3::ProjectionMag(vector, yAxis);
		auto zProj = Vector3::ProjectionMag(vector, zAxis);

		return Vector3(xProj, yProj, zProj);
	}
}