// ----------------------------------------------------
// Roman P.
// Copyright (c) 2025.
// ----------------------------------------------------
#pragma once
#include "core.h"

#ifndef FI
	//#define FINL __forceinline
	#define INL inline
#endif

namespace my {

	const double PI = 3.14159265359;

	INL float radians(float degree) { return degree * static_cast<float>(PI / 180.0); }
	INL float degrees(float radians) { return radians * static_cast<float>(180.0 / PI); }

	// --------------------------------------------------------
	struct vec2 {
		float x = 0.0f, y = 0.0f;
		vec2() {}
		vec2(float _x, float _y) : x(_x), y(_y) {}
		vec2 operator+(const vec2& v) { return vec2(x + v.x, y + v.y); }
		vec2 operator-(const vec2& v) { return vec2(x - v.x, y - v.y); }
		vec2 operator*(const float& scalar) { return vec2(scalar * x, scalar * y); }

		float length() { return sqrt(x * x + y * y); }
		void normalize() { float l = sqrt(x * x + y * y); x /= l; y /= l; }
	};

	INL float dotProd(const vec2& v1, const vec2& v2) { return (v1.x * v2.x + v1.y * v2.y); }

	// --------------------------------------------------------
	struct vec3 {
		float x, y, z;

		vec3() : x(.0f), y(.0f), z(.0f) {}
		vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
		vec3 operator+(const vec3& v) { return vec3(x + v.x, y + v.y, z + v.z); }
		vec3 operator-(const vec3& v) { return vec3(x - v.x, y - v.y, z - v.z); }
		vec3 operator*(const float& scalar) { return vec3(scalar * x, scalar * y, scalar * z); }

		float length() { return sqrt(x * x + y * y + z * z); }
		vec3 normalize() { float l = length(); if (l <= 0) return vec3(0, 0, 0); x /= l; y /= l; z /= l; return *this; }
		void rotate(float angle, const vec3& q);
	};

	INL vec3 operator+(const vec3& v1, const vec3& v2) { return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }
	INL vec3 operator-(const vec3& v1, const vec3& v2) { return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }

	INL vec3 operator*(const vec3& v, const float s) { return vec3(v.x * s, v.y * s, v.z * s); }
	INL vec3 operator*(const float s, const vec3& v) { return vec3(v.x * s, v.y * s, v.z * s); }
	//FI vec3 operator/(const quat& v, const float d) { return vec3(v.x / d, v.y / d, v.z / d); }

	INL vec3 normalize(const vec3& v) { float l = sqrt(v.x * v.x + v.y * v.y + v.z * v.z); if (l <= 0) return vec3(0, 0, 0); return vec3(v.x / l, v.y / l, v.z / l); }
	INL float dotProd(const vec3& v1, const vec3& v2) { return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z); }
	INL vec3 crossProd(const vec3& v1, const vec3& v2) { return vec3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x); }

	INL vec3 lerp(const vec3& v1, const vec3& v2, float a) {
		assert(a >= 0.0f && a <= 1.0f);
		return v1 * (1.0f - a) + (v2 * a);
	}
	INL vec3 lerp_imp(const vec3& v1, const vec3& v2, float a) {
		assert(a >= 0.0f && a <= 1.0f);
		return v1 + a * (v2 - v1); // Imprecise method
	}

	// Вспомогательные функции
	static INL void cross(const float a[3], const float b[3], float out[3]) {
		out[0] = a[1] * b[2] - a[2] * b[1];
		out[1] = a[2] * b[0] - a[0] * b[2];
		out[2] = a[0] * b[1] - a[1] * b[0];
	}

	static INL float dot(const float a[3], const float b[3]) {
		return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	}

	// compute cross product of two __m128 vectors (x,y,z,?)
	static INL __m128 sse_cross(const __m128& a, const __m128& b) {
		// tmp1 = (a.y, a.z, a.x), tmp2 = (b.z, b.x, b.y) then a*tmp2 - b*tmp1 pattern
		__m128 a_yzx = _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1)); // (a.z,a.x,a.y,a.w) careful ordering
		__m128 a_zxy = _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 1, 0, 2)); // (a.y,a.z,a.x,?)
		__m128 b_yzx = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1));
		__m128 b_zxy = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2));
		return _mm_sub_ps(_mm_mul_ps(a_yzx, b_zxy), _mm_mul_ps(a_zxy, b_yzx));
	}

	// horizontal dot for first 3 components of vectors a and b
	static INL float sse_dot3(const __m128& a, const __m128& b) {
		__m128 mul = _mm_mul_ps(a, b);
		// sum x+y+z
		float tmp[4];
		_mm_storeu_ps(tmp, mul);
		return tmp[0] + tmp[1] + tmp[2];
	}

	// --------------------------------------------------------
	struct vec4 {
		float x, y, z, w;
		vec4() : x(.0f), y(.0f), z(.0f), w(.0f) {}
		vec4(float _x = .0f, float _y = .0f, float _z = .0f, float _w = .0f) : x(_x), y(_y), z(_z), w(_w) {}
	};

	INL vec4 operator*(const vec4& v1, const vec4& v2) {
		return vec4(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
	}
	INL vec4 operator-(const vec4& v1, const vec4& v2) {
		return vec4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
	}
	INL vec4 operator+(const vec4& v1, const vec4& v2) {
		return vec4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
	}

	// --------------------------------------------------------
	// матрицы 2x2 возможно надо транспонировать 
	// --------------------------------------------------------
	struct mat2 {
		float _m[2][2] = {};

		mat2(int i = 0) { if (i == 1) { _m[0][0] = 1.0f; _m[1][1] = 1.0f; } }
		void identity() { _m[0][0] = 1.0f; _m[1][1] = 1.0f; }
	};

	INL mat2 operator+(const mat2& m1, const mat2& m2);
	INL mat2 operator*(const mat2& m1, const mat2& m2);

	INL vec2 operator*(const mat2& m, const vec2& v) { return vec2(v.x * m._m[0][0] + v.y * m._m[0][1], v.x * m._m[1][0] + v.y * m._m[1][1]); }

	// --------------------------------------------------------
	// matrix 3x3 возможно надо транспонировать 
	// --------------------------------------------------------
	struct mat3 {
		float m[3][3] = {};
		mat3(int i = 0) { if (i == 1) { m[0][0] = 1.0f; m[1][1] = 1.0f; m[2][2] = 1.0f; } }
		void identity() { m[0][0] = 1.0f; m[1][1] = 1.0f; m[2][2] = 1.0f; }
	};

	INL vec3 operator*(const mat3& m, const vec3& v) {
		return vec3(v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2],
			v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2],
			v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2]);
	}

	INL mat3 operator*(const mat3& m1, const mat3& m2);

	// --------------------------------------------------------
	// matrix 4x4
	// --------------------------------------------------------
	struct
		//alignas(16)
		mat4 {
		float m[4][4] = {};

		mat4(int i = 0) { if (i == 1) { m[0][0] = 1.0f; m[1][1] = 1.0f; m[2][2] = 1.0f; m[3][3] = 1.0f; } }
		mat4(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33);
		mat4(const vec4& v0, const vec4& v1, const vec4& v2, const vec4& v3);

		void identity() { memset(m, 0, 16 * sizeof(float)); m[0][0] = 1.0f; m[1][1] = 1.0f; m[2][2] = 1.0f; m[3][3] = 1.0f; }

		const float* data() const { return &m[0][0]; }
		float* operator[](int i) { return this->m[i]; }

		mat4 operator*(float s) {
			m[0][0] *= s; m[0][1] *= s; m[0][2] *= s; m[0][3] *= s;
			m[1][0] *= s; m[1][1] *= s; m[1][2] *= s; m[1][3] *= s;
			m[2][0] *= s; m[2][1] *= s; m[2][2] *= s; m[2][3] *= s;
			m[3][0] *= s; m[3][1] *= s; m[3][2] *= s; m[3][3] *= s;
			return *this;
		}

		void transpose() {
			mat4 ml;
			ml.m[0][0] = this->m[0][0]; ml.m[0][1] = this->m[1][0]; ml.m[0][2] = this->m[2][0]; ml.m[0][3] = this->m[3][0];
			ml.m[1][0] = this->m[0][1]; ml.m[1][1] = this->m[1][1]; ml.m[1][2] = this->m[2][1]; ml.m[1][3] = this->m[3][1];
			ml.m[2][0] = this->m[0][2]; ml.m[2][1] = this->m[1][2]; ml.m[2][2] = this->m[2][2]; ml.m[2][3] = this->m[3][2];
			ml.m[3][0] = this->m[0][3]; ml.m[3][1] = this->m[1][3]; ml.m[3][2] = this->m[2][3]; ml.m[3][3] = this->m[3][3];
			*this = ml;
		}
	};

	INL vec3 operator*(const mat4& m, const vec3& v) {
		return vec3(
			v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
			v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
			v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]);
	}

	INL vec4 operator*(const mat4& m, const vec4& v) {
		return vec4(
			v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0],
			v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1],
			v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2],
			v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3]);
	}

	// (2nd way only)
	INL mat4 operator*(const mat4& m1, const mat4& m2);

	// вроде ок (не знаю точной формулы)
	INL mat4 translate(const mat4& m, const vec3& v);

	// вроде ок (не знаю точной формулы)
	INL mat4 scale(const mat4& m, const vec3& v);

	// error when all axis is zero
	INL mat4 rotate(const mat4& m, float angle, const vec3& axis);

	INL mat4 rotateX(float angle);
	INL mat4 rotateY(float angle);
	INL mat4 rotateZ(float angle);

	// универсальная версия (полная версия)
	// Если матрица содержит перспективу - использовать полную версию
	/*INL mat4 inverse(const mat4& a) {
		mat4 inv;
		float invOut[16];
		const float* m = &a.m[0][0];

		invOut[0] = m[5] * m[10] * m[15] -
			m[5] * m[11] * m[14] -
			m[9] * m[6] * m[15] +
			m[9] * m[7] * m[14] +
			m[13] * m[6] * m[11] -
			m[13] * m[7] * m[10];

		invOut[4] = -m[4] * m[10] * m[15] +
			m[4] * m[11] * m[14] +
			m[8] * m[6] * m[15] -
			m[8] * m[7] * m[14] -
			m[12] * m[6] * m[11] +
			m[12] * m[7] * m[10];

		invOut[8] = m[4] * m[9] * m[15] -
			m[4] * m[11] * m[13] -
			m[8] * m[5] * m[15] +
			m[8] * m[7] * m[13] +
			m[12] * m[5] * m[11] -
			m[12] * m[7] * m[9];

		invOut[12] = -m[4] * m[9] * m[14] +
			m[4] * m[10] * m[13] +
			m[8] * m[5] * m[14] -
			m[8] * m[6] * m[13] -
			m[12] * m[5] * m[10] +
			m[12] * m[6] * m[9];

		invOut[1] = -m[1] * m[10] * m[15] +
			m[1] * m[11] * m[14] +
			m[9] * m[2] * m[15] -
			m[9] * m[3] * m[14] -
			m[13] * m[2] * m[11] +
			m[13] * m[3] * m[10];

		invOut[5] = m[0] * m[10] * m[15] -
			m[0] * m[11] * m[14] -
			m[8] * m[2] * m[15] +
			m[8] * m[3] * m[14] +
			m[12] * m[2] * m[11] -
			m[12] * m[3] * m[10];

		invOut[9] = -m[0] * m[9] * m[15] +
			m[0] * m[11] * m[13] +
			m[8] * m[1] * m[15] -
			m[8] * m[3] * m[13] -
			m[12] * m[1] * m[11] +
			m[12] * m[3] * m[9];

		invOut[13] = m[0] * m[9] * m[14] -
			m[0] * m[10] * m[13] -
			m[8] * m[1] * m[14] +
			m[8] * m[2] * m[13] +
			m[12] * m[1] * m[10] -
			m[12] * m[2] * m[9];

		invOut[2] = m[1] * m[6] * m[15] -
			m[1] * m[7] * m[14] -
			m[5] * m[2] * m[15] +
			m[5] * m[3] * m[14] +
			m[13] * m[2] * m[7] -
			m[13] * m[3] * m[6];

		invOut[6] = -m[0] * m[6] * m[15] +
			m[0] * m[7] * m[14] +
			m[4] * m[2] * m[15] -
			m[4] * m[3] * m[14] -
			m[12] * m[2] * m[7] +
			m[12] * m[3] * m[6];

		invOut[10] = m[0] * m[5] * m[15] -
			m[0] * m[7] * m[13] -
			m[4] * m[1] * m[15] +
			m[4] * m[3] * m[13] +
			m[12] * m[1] * m[7] -
			m[12] * m[3] * m[5];

		invOut[14] = -m[0] * m[5] * m[14] +
			m[0] * m[6] * m[13] +
			m[4] * m[1] * m[14] -
			m[4] * m[2] * m[13] -
			m[12] * m[1] * m[6] +
			m[12] * m[2] * m[5];

		invOut[3] = -m[1] * m[6] * m[11] +
			m[1] * m[7] * m[10] +
			m[5] * m[2] * m[11] -
			m[5] * m[3] * m[10] -
			m[9] * m[2] * m[7] +
			m[9] * m[3] * m[6];

		invOut[7] = m[0] * m[6] * m[11] -
			m[0] * m[7] * m[10] -
			m[4] * m[2] * m[11] +
			m[4] * m[3] * m[10] +
			m[8] * m[2] * m[7] -
			m[8] * m[3] * m[6];

		invOut[11] = -m[0] * m[5] * m[11] +
			m[0] * m[7] * m[9] +
			m[4] * m[1] * m[11] -
			m[4] * m[3] * m[9] -
			m[8] * m[1] * m[7] +
			m[8] * m[3] * m[5];

		invOut[15] = m[0] * m[5] * m[10] -
			m[0] * m[6] * m[9] -
			m[4] * m[1] * m[10] +
			m[4] * m[2] * m[9] +
			m[8] * m[1] * m[6] -
			m[8] * m[2] * m[5];

		float det = m[0] * invOut[0] + m[1] * invOut[4] + m[2] * invOut[8] + m[3] * invOut[12];
		if (fabs(det) < 1e-8f)
			assert(0);

		det = 1.0f / det;
		for (int i = 0; i < 16; i++) invOut[i] *= det;

		// copy back into mat4
		for (int c = 0; c < 4; ++c)
			for (int r = 0; r < 4; ++r)
				inv.m[c][r] = invOut[c * 4 + r];

		return inv;
	}*/

	// Быстрая инверсия для аффинных матриц (без перспективы)
	INL mat4 inverseAffine(const mat4& a) {
		mat4 inv;

		// Извлекаем 3x3 часть (вращение/масштаб)
		float c0[3] = { a.m[0][0], a.m[0][1], a.m[0][2] };
		float c1[3] = { a.m[1][0], a.m[1][1], a.m[1][2] };
		float c2[3] = { a.m[2][0], a.m[2][1], a.m[2][2] };

		// Вычисляем обратную к 3x3 (через аджугат/детерминант)
		float r0[3], r1[3], r2[3];
		cross(c1, c2, r0); // row 0 of adjugate
		cross(c2, c0, r1); // row 1
		cross(c0, c1, r2); // row 2

		float det = dot(c0, r0);
		if (fabs(det) < 1e-8f)
			assert(0);

		float invDet = 1.0f / det;

		for (int row = 0; row < 3; ++row) {
			inv.m[0][row] = (row == 0 ? r0[0] : (row == 1 ? r1[0] : r2[0])) * invDet;
			inv.m[1][row] = (row == 0 ? r0[1] : (row == 1 ? r1[1] : r2[1])) * invDet;
			inv.m[2][row] = (row == 0 ? r0[2] : (row == 1 ? r1[2] : r2[2])) * invDet;
		}

		// Обратный перенос: -R^{-1} * T
		// Перенос T (столбец 3)
		float T[3] = { a.m[3][0], a.m[3][1], a.m[3][2] };

		float invT0 = -(inv.m[0][0] * T[0] + inv.m[1][0] * T[1] + inv.m[2][0] * T[2]);
		float invT1 = -(inv.m[0][1] * T[0] + inv.m[1][1] * T[1] + inv.m[2][1] * T[2]);
		float invT2 = -(inv.m[0][2] * T[0] + inv.m[1][2] * T[1] + inv.m[2][2] * T[2]);

		inv.m[3][0] = invT0;
		inv.m[3][1] = invT1;
		inv.m[3][2] = invT2;

		// Последняя строка/столбец для 4x4 в column-major
		inv.m[0][3] = 0.0f;
		inv.m[1][3] = 0.0f;
		inv.m[2][3] = 0.0f;
		inv.m[3][3] = 1.0f;

		return inv;
	}

	// SIMD - оптимизированная версия(SSE)
	// (для аффинных матриц)
	// Работает только если матрица аффинная(то есть m[3][3] == 1, m[0][3] = m[1][3] = m[2][3] = 0).
	// Поддерживает вращение + масштаб + перенос.
	INL mat4 inverseAffineSSE(const mat4& M) {
		mat4 inv;

		// Загрузка 3 столбцов вращения/масштаба
		// Загрузим столбцы (x,y,z,w) — w может быть 0/1, не важно
		__m128 c0 = _mm_loadu_ps(&M.m[0][0]); // column 0
		__m128 c1 = _mm_loadu_ps(&M.m[1][0]); // column 1
		__m128 c2 = _mm_loadu_ps(&M.m[2][0]); // column 2
		__m128 c3 = _mm_loadu_ps(&M.m[3][0]); // translation column (Tx,Ty,Tz,1)

		// Вычисляем векторные произведения (они будут строками adjugate)
		__m128 r0_v = sse_cross(c1, c2); // r0 = cross(c1,c2)
		__m128 r1_v = sse_cross(c2, c0); // r1 = cross(c2,c0)
		__m128 r2_v = sse_cross(c0, c1); // r2 = cross(c0,c1)

		// детерминант = dot(c0, r0)
		float det = sse_dot3(c0, r0_v);
		if (std::fabs(det) < 1e-8f) throw std::runtime_error("Matrix not invertible (affine SSE)");

		float invDet = 1.0f / det;

		// Сохраним r0,r1,r2 в массивы float для удобной записи в column-major
		float r0[4], r1[4], r2[4], tcol[4];
		_mm_storeu_ps(r0, r0_v);
		_mm_storeu_ps(r1, r1_v);
		_mm_storeu_ps(r2, r2_v);
		_mm_storeu_ps(tcol, c3);

		// invR (rows = r0,r1,r2) ; записываем в inv.m[col][row] = invR[row][col]
		for (int row = 0; row < 3; ++row) {
			inv.m[0][row] = (row == 0 ? r0[0] : (row == 1 ? r1[0] : r2[0])) * invDet;
			inv.m[1][row] = (row == 0 ? r0[1] : (row == 1 ? r1[1] : r2[1])) * invDet;
			inv.m[2][row] = (row == 0 ? r0[2] : (row == 1 ? r1[2] : r2[2])) * invDet;
		}

		// invT = -invR * T
		float T0 = tcol[0], T1 = tcol[1], T2 = tcol[2];
		inv.m[3][0] = -(inv.m[0][0] * T0 + inv.m[1][0] * T1 + inv.m[2][0] * T2);
		inv.m[3][1] = -(inv.m[0][1] * T0 + inv.m[1][1] * T1 + inv.m[2][1] * T2);
		inv.m[3][2] = -(inv.m[0][2] * T0 + inv.m[1][2] * T1 + inv.m[2][2] * T2);

		inv.m[0][3] = 0.0f;
		inv.m[1][3] = 0.0f;
		inv.m[2][3] = 0.0f;
		inv.m[3][3] = 1.0f;

		return inv;
	}

	// аналитическая формула (для аффинных матриц) 
	INL mat4 invertAffine2(const mat4& src) {
		// Верхний левый 3x3 — транспонируем и инвертируем
		float det = src.m[0][0] * (src.m[1][1] * src.m[2][2] - src.m[2][1] * src.m[1][2]) -
			src.m[1][0] * (src.m[0][1] * src.m[2][2] - src.m[2][1] * src.m[0][2]) +
			src.m[2][0] * (src.m[0][1] * src.m[1][2] - src.m[1][1] * src.m[0][2]);
		if (std::abs(det) < 1e-10f) assert(0);
		float inv_det = 1.0f / det;

		mat4 dst;
		dst.m[0][0] = inv_det * (src.m[1][1] * src.m[2][2] - src.m[1][2] * src.m[2][1]);
		dst.m[0][1] = inv_det * (src.m[2][1] * src.m[0][2] - src.m[0][1] * src.m[2][2]);
		dst.m[0][2] = inv_det * (src.m[0][1] * src.m[1][2] - src.m[1][1] * src.m[0][2]);

		dst.m[1][0] = inv_det * (src.m[1][2] * src.m[2][0] - src.m[1][0] * src.m[2][2]);
		dst.m[1][1] = inv_det * (src.m[2][2] * src.m[0][0] - src.m[2][0] * src.m[0][2]);
		dst.m[1][2] = inv_det * (src.m[0][2] * src.m[1][0] - src.m[1][2] * src.m[0][0]);

		dst.m[2][0] = inv_det * (src.m[1][0] * src.m[2][1] - src.m[2][0] * src.m[1][1]);
		dst.m[2][1] = inv_det * (src.m[0][1] * src.m[2][0] - src.m[0][0] * src.m[2][1]);
		dst.m[2][2] = inv_det * (src.m[1][1] * src.m[0][0] - src.m[0][1] * src.m[1][0]);

		// Обратный перенос
		dst.m[3][0] = -(src.m[3][0] * dst.m[0][0] + src.m[3][1] * dst.m[1][0] + src.m[3][2] * dst.m[2][0]);
		dst.m[3][1] = -(src.m[3][0] * dst.m[0][1] + src.m[3][1] * dst.m[1][1] + src.m[3][2] * dst.m[2][1]);
		dst.m[3][2] = -(src.m[3][0] * dst.m[0][2] + src.m[3][1] * dst.m[1][2] + src.m[3][2] * dst.m[2][2]);
		dst.m[3][3] = 1.0f;

		// Нижний правый угол и последняя строка
		dst.m[0][3] = dst.m[1][3] = dst.m[2][3] = 0.0f;

		return dst;
	}

	// left-handed
	// negative one to one (z -1 to 1)
	// fovy convert to radians
	// positive +z axis point in to the screen
	// correct left-hand perspective (apect = height / width)
	INL mat4 perspectiveLH_OGLDEV(float fovy, float aspect, float zNear, float zFar);

	// left-handed
	// negative one to one (z -1 to 1)
	// This creates a symmetric frustum with vertical FOV
	// fovy convert to radians
	// positive +z axis point in to the screen
	INL mat4 perspectiveLH(float fovy, float aspect, float zNear, float zFar);

	// left-handed
	// negative zero to one (z 0 to 1)
	// This creates a symmetric frustum with vertical FOV
	// fovy convert to radians
	// positive +z axis point in to the screen
	INL mat4 perspectiveLH_ZO(float fovy, float aspect, float zNear, float zFar);

	// right-handed
	// negative one to one (z -1 to 1)
	// This creates a symmetric frustum with vertical FOV
	// fovy convert to radians
	// negative -z axis point in to the screen
	INL mat4 perspectiveRH(float fovy, float aspect, float zNear, float zFar);

	INL mat4 lookAtLH(const vec3& eye, const vec3& at, const vec3& up);
	INL mat4 lookAtRH(const vec3& eye, const vec3& at, const vec3& up);

	// OGLDEV
	struct Matrix4f {
		float m[4][4] = { 0.0f };

		Matrix4f() {}
		Matrix4f(float a00, float a01, float a02, float a03,
				 float a10, float a11, float a12, float a13,
				 float a20, float a21, float a22, float a23,
				 float a30, float a31, float a32, float a33);

		Matrix4f operator*(const Matrix4f& Right) const;
	};

	// right-handed
	// This creates a symmetric frustum with horizontal FOV
	//FI Matrix4f perspectiveRHHF(float fovx, float aspect, float zNear, float zFar);

	//----------------------------------------------------------------------------
	/* where a - scalar , v - vector
		q(a, v)
		(a = cos(0), v = sin(0) * v)
		where v = 1
		mul : q1*q2 = (a1*a2 - v1 dot v2, a1*v2 + a2*v1 + v1 cros v2)
	// ------------------------------------------ */
	struct quat {
		float w, x, y, z;

		quat() : w(0.f), x(0.f), y(0.f), z(0.f) {}
		quat(float _w, float _x, float _y, float _z) : w(_w), x(_x), y(_y), z(_z) {}
		quat(float angle, const vec3& v) {
			const float a = angle * 0.5f, s = sin(a);
			w = cos(a);
			x = v.x * s; y = v.y * s; z = v.z * s; }

		float length() { return sqrt(w * w + x * x + y * y + z * z); }

		quat normalize();
		quat negate()				const { return quat(-w, -x, -y, -z); }
		quat conjugate()			const { return quat(w, -x, -y, -z); }
	};

	INL quat normalize(const my::quat& q) { float l = sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
		return quat(q.w / l, q.x / l, q.y / l, q.z / l); }

	INL quat operator+(const quat& q1, const quat& q2) { return quat(q1.w + q2.w, q1.x + q2.x, q1.y + q2.y, q1.z + q2.z); }
	INL quat operator-(const quat& q1, const quat& q2) { return quat(q1.w - q2.w, q1.x - q2.x, q1.y - q2.y, q1.z - q2.z); }
	INL quat operator*(const quat& q, const float s) { return quat(q.w * s, q.x * s, q.y * s, q.z * s); }
	INL quat operator*(const float s, const quat& q) { return quat(q.w * s, q.x * s, q.y * s, q.z * s); }
	INL quat operator/(const quat& q, const float d) { return quat(q.w / d, q.x / d, q.y / d, q.z / d); }
	INL quat operator*(const quat& q1, const quat& q2);
	INL quat operator*(const quat& q, const vec3& v);

	INL float dot(const quat& q1, const quat& q2) {
		// VS 17.7.4 generates longer assembly (~20 instructions vs 11 instructions)
//#if !defined(_MSC_VER)
//		return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
//#else
		vec4 v(q1.x * q2.x, q1.y * q2.y, q1.z * q2.z, q1.w * q2.w);
		return (v.x + v.y) + (v.z + v.w);
//#endif
	}

	// преобразовываем кватернион в матрицу поворота
	INL mat4 toMat4(const quat& q) {
		return mat4(
			1 - 2 * (q.y * q.y + q.z * q.z), 2 * (q.x * q.y + q.w * q.z), 2 * (q.x * q.z - q.w * q.y), 0.0f,
			2 * (q.x * q.y - q.w * q.z), 1 - 2 * (q.x * q.x + q.z * q.z), 2 * (q.y * q.z + q.w * q.x), 0.0f,
			2 * (q.x * q.z + q.w * q.y), 2 * (q.y * q.z - q.w * q.x), 1 - 2 * (q.x * q.x + q.y * q.y), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}
	
	INL quat lerp(const quat& q1, const quat& q2, float a) {
		assert(a >= 0.0f && a <= 1.0f);
		return q1 * (1.0f - a) + (q2 * a);
		//return q1 + a * (q2 - q1); // Imprecise method
	}

	INL quat slerp(const quat& q1, const quat& q2, float a) {
		quat d = q2;

		float cosTheta = dot(q1, q2);
		// Avoid going the long way around.
		if (cosTheta < 0.0f) {
			d = q2.negate();
			cosTheta = -cosTheta;
		}

		// Check if they are very close together, to protect against divide−by−zero
		if (cosTheta > 0.9999f) {
		//if (cosTheta > 1.0f - std::numeric_limits<float>::epsilon()) {
			// just use linear interpolation
			return lerp(q1, d, a);
			//return quat(std::lerp(q1.w, d.w, a),
			//	std::lerp(q1.x, d.x, a),
			//	std::lerp(q1.y, d.y, a),
			//	std::lerp(q1.z, d.z, a));
		}

		float angle = acos(cosTheta);
		return (sin((1 - a) * angle) * q1 + sin(a * angle) * d) / sin(angle);
	}

	INL quat slerp2(const quat& q1, const quat& q2, float a) {
		quat d = q2;

		float cosTheta = dot(q1, q2);
		// Avoid going the long way around.
		if (cosTheta < 0.0f) {
			d = q2.negate();
			cosTheta = -cosTheta;
		}

		// Check if they are very close together, to protect against divide−by−zero
		float k0, k1;
		if (cosTheta > 0.9999f) {
			// just use linear interpolation
			k0 = 1.0f - a;
			k1 = a;
		}
		else {
			float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
			float angle = atan2(sinTheta, cosTheta);
			float oneOverSinTheta = 1.0f / sinTheta;

			k0 = sin((1.0f - a) * angle) * oneOverSinTheta;
			k1 = sin(a * angle) * oneOverSinTheta;
		}

		// Interpolate
		return q1 * k0 + d * k1;
	}

	#include "mymath.inl"

} // namespace my

//#undef FI // forceinline