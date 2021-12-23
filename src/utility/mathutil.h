#pragma once
#include <cmath>
#include <float.h>


const double PI = 3.1415926535897932384626433832795;
const double E = 2.7182818284590452353602874713527;
const double EPSILON = 0.001;
const double DEG2RAD = PI / 180.0;
const double RAD2DEG = 180.0 / PI;


inline double clamp(double mini, double maxi, double val) {
	if (val < mini)
		return mini;
	if (val > maxi)
		return maxi;
	return val;
}


/**
 * @brief 2D Vector. Components of type double.
 * 
 */
struct Vec2 {
	double x, y;

	Vec2() { ; }
	Vec2(double x, double y) { this->x = x; this->y = y; }

	Vec2 operator * (double k) const { return Vec2(k * x, k * y); }
	Vec2 operator / (double k) const { return Vec2(x / k, y / k); }
	void operator *= (double k) { x *= k; y *= k; }
	void operator /= (double k) { double invk = 1.0 / k; x *= invk; y *= invk; }

	Vec2 operator + (const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
	Vec2 operator - (const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
	void operator += (const Vec2& v) { x += v.x; y += v.y; }
	void operator -= (const Vec2& v) { x -= v.x; y -= v.y; }

	Vec2 operator - () const { return Vec2(-x, -y); }

	double len_sqr() const { return x * x + y * y; }
	double len() const { return std::sqrt(len_sqr()); }
	Vec2 normalized() const { double l = len(); return ((l > EPSILON) ? (*this / l) : Vec2()); }
	void normalize() { *this = normalized(); }
};


inline Vec2 operator * (double k, const Vec2& v) { return v * k; }
inline double dist_sqr(const Vec2& a, const Vec2& b) { return (a - b).len_sqr(); }
inline double dist(const Vec2& a, const Vec2& b) { return (a - b).len(); }
inline double dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }
inline Vec2 cross(const Vec2& v, double k) { return Vec2(v.y * k, v.x * -k); }
inline Vec2 cross(double k, const Vec2& v) { return Vec2(v.y * -k, v.x * k); }
inline double cross(const Vec2& a, const Vec2& b) { return a.x * b.y - a.y * b.x; } // z component of Vec3(a) x Vec3(b)


/**
 * @brief 2x2 matrix. Components are of type double.
 * 
 */
struct Mat2x2 {
	double m00, m01, m10, m11;

	Mat2x2() { ; }
	Mat2x2(double x0, double x1, double x2, double x3) { m00=x0; m01=x1; m10=x2; m11=x3; }
	Mat2x2(double radians) { 
		double c = std::cos(radians); 
		double s = std::sin(radians);
		m00 = c; m01 = -s; m10 = s; m11 = c;
	}

	Mat2x2 operator + (const Mat2x2& other) const {
		return Mat2x2(m00 + other.m00, m01 + other.m01, m10 + other.m10, m11 + other.m11);
	}
	Mat2x2 operator * (const Mat2x2& other) const {
		return Mat2x2(
			m00 * other.m00 + m01 * other.m10, 
			m00 * other.m01 + m01 * other.m11, 
			m10 * other.m00 + m11 * other.m10, 
			m10 * other.m01 + m11 * other.m11
		);
	}

	Vec2 operator * (const Vec2& v) const { return Vec2(m00 * v.x + m01 * v.y, m10 * v.x + m11 * v.y); }
	
	double determinant() const { return m00 * m11 - m01 * m10; }
	Mat2x2 transpose() const { return Mat2x2(m00, m10, m01, m11);	}
	Mat2x2 inverse() const{ double d = determinant(); return Mat2x2(m11 / d, -m10 / d, -m01 / d, m00 / d); }
};