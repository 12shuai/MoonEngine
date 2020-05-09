#pragma once
#include <cmath>
#include <iostream>
#include <iomanip>

#include "MathUtils.h"
#include "Vector3.h"

namespace MOON {
	class Matrix3x3 {
	public:
		float x[3][3] = { {1,0,0}, {0,1,0}, {0,0,1} };

		Matrix3x3() {}
		Matrix3x3(float i) {
			x[0][0] = i; x[0][1] = 0; x[0][2] = 0;
			x[1][0] = 0; x[1][1] = i; x[1][2] = 0;
			x[2][0] = 0; x[2][1] = 0; x[2][2] = i;
		}
		Matrix3x3(float a, float b, float c, float d, float e, float f, float g, float h, float i) {
			x[0][0] = a; x[0][1] = b; x[0][2] = c;
			x[1][0] = d; x[1][1] = e; x[1][2] = f;
			x[2][0] = g; x[2][1] = h; x[2][2] = i;
		}
		Matrix3x3(const Matrix3x3 &m) {
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
					x[i][j] = m[i][j];
		}

		static Matrix3x3 identity() { return Matrix3x3(); }

		const float* operator[](uint8_t i) const { return x[i]; }
		float* operator[](uint8_t i) { return x[i]; }

		// Multiply the current matrix with another matrix (rhs)
		Matrix3x3 operator*(const Matrix3x3& v) const {
			Matrix3x3 tmp;
			multiply(*this, v, tmp);

			return tmp;
		}

		Vector3 operator*(const Vector3 &v) const {
			Vector3 result;

			result[0] = v[0] * x[0][0] + v[1] * x[1][0] + v[2] * x[2][0];
			result[1] = v[0] * x[0][1] + v[1] * x[1][1] + v[2] * x[2][1];
			result[2] = v[0] * x[0][2] + v[1] * x[1][2] + v[2] * x[2][2];

			return result;
		}

		static void multiply(const Matrix3x3 &a, const Matrix3x3& b, Matrix3x3 &c) {
#if 0
			for (uint8_t i = 0; i < 3; ++i) {
				for (uint8_t j = 0; j < 3; ++j) {
					c[j][i] = a[0][i] * b[j][0] + a[1][i] * b[j][1] + a[2][i] * b[j][2];
				}
			}
#else
			const float * __restrict ap = &a.x[0][0];
			const float * __restrict bp = &b.x[0][0];
			float * __restrict cp = &c.x[0][0];

			float a0, a1, a2;

			a0 = ap[0];
			a1 = ap[3];
			a2 = ap[6];

			cp[0] = a0 * bp[0] + a1 * bp[1] + a2 * bp[2];
			cp[3] = a0 * bp[3] + a1 * bp[4] + a2 * bp[5];
			cp[6] = a0 * bp[6] + a1 * bp[7] + a2 * bp[8];

			a0 = ap[1];
			a1 = ap[4];
			a2 = ap[7];

			cp[1] = a0 * bp[0] + a1 * bp[1] + a2 * bp[2];
			cp[4] = a0 * bp[3] + a1 * bp[4] + a2 * bp[5];
			cp[7] = a0 * bp[6] + a1 * bp[7] + a2 * bp[8];

			a0 = ap[2];
			a1 = ap[5];
			a2 = ap[8];

			cp[2] = a0 * bp[0] + a1 * bp[1] + a2 * bp[2];
			cp[5] = a0 * bp[3] + a1 * bp[4] + a2 * bp[5];
			cp[8] = a0 * bp[6] + a1 * bp[7] + a2 * bp[8];
#endif
		}

		// return a transposed copy of the current matrix as a new matrix
		Matrix3x3 transposed() const {
#if 0 
			Matrix3x3 t;
			for (uint8_t i = 0; i < 3; ++i) {
				for (uint8_t j = 0; j < 3; ++j) {
					t[i][j] = x[j][i];
				}
			}
			return t;
#else 
			return Matrix3x3(x[0][0], x[1][0], x[2][0],
							 x[0][1], x[1][1], x[2][1],
							 x[0][2], x[1][2], x[2][2]);
#endif 
		}

		// transpose itself
		Matrix3x3& transpose() {
			Matrix3x3 tmp(x[0][0], x[1][0], x[2][0],
						  x[0][1], x[1][1], x[2][1],
						  x[0][2], x[1][2], x[2][2]);
			*this = tmp;

			return *this;
		}

		/*
		calculate |A|, |A^-1| = 1/|A|
		mat: matrix A
		n: rank of matrix
		*/
		template <class T>
		float GetA(const T &mat, int n) const {
			if (n == 1) return mat[0][0];

			float ans = 0; T temp;
			for (int i = 0;i < n;i++) {
				for (int j = 0;j < n - 1;j++)
					for (int k = 0;k < n - 1;k++)
						temp[j][k] = mat[j + 1][(k >= i) ? k + 1 : k];

				float t = GetA(temp, n - 1);
				if (i % 2 == 0) ans += mat[0][i] * t;
				else ans -= mat[0][i] * t;
			}
			return ans;
		}

		/*
		calculate A*
		mat: matrix A
		n: rank of matrix
		*/
		template <class T>
		T GetAStart(const T &mat, int n) const {
			T ans; if (n == 1) return ans;
			T temp;
			for (int i = 0;i < n;i++) {
				for (int j = 0;j < n;j++) {
					for (int k = 0;k < n - 1;k++) {
						for (int t = 0;t < n - 1;t++) {
							temp[k][t] = mat[k >= i ? k + 1 : k][t >= j ? t + 1 : t];
						}
					}

					ans[j][i] = GetA(temp, n - 1);
					if ((i + j) % 2 == 1) ans[j][i] = -ans[j][i];
				}
			}

			return ans;
		}

		Matrix3x3 inverse() const {
			float flag = GetA(*this, 3);
			if (flag == 0) return Matrix3x3();

			Matrix3x3 des = GetAStart(*this, 3);
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
					des[i][j] /= flag;

			return des;
		}

		// set current matrix to its inverse
		const Matrix3x3& invert() {
			*this = inverse();
			return *this;
		}

		friend std::ostream& operator << (std::ostream &s, const Matrix3x3 &m) {
			std::ios_base::fmtflags oldFlags = s.flags();
			int width = 12;
			s.precision(5);
			s.setf(std::ios_base::fixed);

			s << "|" << std::setw(width) << m[0][0] <<
				" " << std::setw(width) << m[1][0] <<
				" " << std::setw(width) << m[2][0] << "|\n" <<

				"|" << std::setw(width) << m[0][1] <<
				" " << std::setw(width) << m[1][1] <<
				" " << std::setw(width) << m[2][1] << "|\n" <<

				"|" << std::setw(width) << m[0][2] <<
				" " << std::setw(width) << m[1][2] <<
				" " << std::setw(width) << m[2][2] << "|";

			s.flags(oldFlags);
			return s;
		}

		static Matrix3x3 TranslateMat(const Vector2 &moveVec) {
			Matrix3x3 result(1,			0,		   0,
							 0,			1,		   0,
							 moveVec.x, moveVec.y, 1);
			return result;
		}
		static Matrix3x3 Translate(const Matrix3x3 &model, const Vector2 &moveVec) {
			return TranslateMat(moveVec) * model;
		}

		// TODO: rotate around arbitrary anchor
		/*
		2D Rotate Matrix:
				|cos��  sin��|
		M(��) = |			 | = cos��|1  0| + sin��|0  1|
				|-sin�� cos��|		  |0  1|		|-1 0|
		*/
		// rotate clock-wise if _angle > 0
		static Matrix3x3 RotateMat(const float _angle) {
			Matrix3x3 Rotate;

			Rotate.x[0][0] = cos(_angle);
			Rotate.x[0][1] = -sin(_angle);
			Rotate.x[1][0] = -Rotate.x[1][0];
			Rotate.x[1][1] = Rotate.x[0][0];

			return Rotate;
		}
		static Matrix3x3 Rotate(const Matrix3x3 &model, const float angle) {
			return RotateMat(angle) * model;
		}

		/*
		If scale around a specified point:
				|Sx  0   (1-Sx)X0|
		M(��) = |0   Sy  (1-Sy)Y0|
				|0   0      1	 |
		where (X0, Y0) is the pivot, (Sx, Sy) is the scale value.
		*/
		// Return a 2D scale matrix based on input scale factor
		static Matrix3x3 ScaleMat(const Vector2 &factor) {
			Matrix3x3 result(factor.x,	0,			0,
							 0,			factor.y,	0,
							 0,			0,			1);
			return result;
		}
		static Matrix3x3 ScaleMat(const float &factor) {
			Matrix3x3 result(factor, 0,		 0,
							 0,		 factor, 0,
							 0,		 0,		 1);
			return result;
		}
		static Matrix3x3 Scale(const Matrix3x3 &model, const float &factor) {
			return ScaleMat(factor) * model;
		}
		static Matrix3x3 Scale(const Matrix3x3 &model, const Vector2 &factor) {
			return ScaleMat(factor) * model;
		}

	};
}

/*
A����ĳ�Ҫ������
	1��|A|������0��
	2��r��A��=n��
	3��A���У��У������������޹ء�
	4��A������ֵ��û��0��
	5��A���Էֽ�Ϊ���ɳ��Ⱦ���ĳ˻���

������󷨣�
	����|A|��ȷ��A����
	1�� ���ڰ�����
		��1��������󣺼��������ÿһ��λ�õĴ�������ʽ����ת��
		��2�����Ӧ����ʽ��ֵ��ǰ���Ѿ������
		��3��A^-1 = A* / |A|
	2�� ���ھ���ĳ��ȱ任
		��1�����ó����б任��[A|E]��[E|A^-1]����
	3��LU�ֽⷨ
		������A�ֽ�Ϊһ�������Ǿ�����һ�������Ǿ���ĳ˻���
		��ν�����������һ��Ϊ��ľ���
		L�������Ǿ���(Lower TriangularMatrix)�������Խ������ϵ�Ԫ��ȫ������0�ľ���
		U�������Ǿ���(Upper Triangular Matrix)�������Խ������µ�Ԫ��ȫ������0�ľ���
		LU�ֽ��Ǹ�˹��Ԫ����һ�ֱ�����ʽ����������ڸ�˹��Ԫ����LU�ֽ������ʵ�ֲ��л���
		��������������ַ�������;���(50000*50000)��
*/