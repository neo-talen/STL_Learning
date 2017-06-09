#ifndef MATH_VEC3_H
#define MATH_VEC3_H

#include<cmath>

namespace htl {

	class Vec3 {
	private:
		float x, y, z;

	public:
		/**
		constructor
		*/
		Vec3();
		Vec3(float a, float b, float c);
		Vec3(const float* ary);
		//constructs a vector that describes the direction between (ps --> pe)
		Vec3(const Vec3 &ps, const Vec3 &pe);

		//copy constructor
		Vec3(const Vec3& copy);

		/**
		destructor
		*/
		~Vec3();

		/**
		static functions
		*/

		//return the angle between the two vector (in radians)
		static float angle(const Vec3 &v1, const Vec3 &v2);

		//dst = v1 + v2;
		static void add(const Vec3 &v1, const Vec3 &v2, Vec3 *dst);

		//dst = clamp v to [min, max]
		static void clamp(const Vec3 &v, const Vec3 &min, const Vec3 &max, Vec3 *dst);

		//dst = v1 cross_product v2
		static void cross(const Vec3 &v1, const Vec3 &v2, Vec3 *dst);

		//return dot(v1, v2)
		static float dot(const Vec3 &v1, const Vec3 &v2);
	};



}

#endif