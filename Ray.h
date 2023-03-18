#pragma once
#include "Vector.h"
class Ray {
public:
	Vec3 origin, direction;
	Ray() {}
	Ray(Vec3 origin, Vec3 direction) : origin(origin), direction(normalize(direction)) {}
	static Vec3 reflect(Vec3 in, Vec3 normal);
	static Vec3 refract(Vec3 in, Vec3 normal);
	Vec3 operator()(double t) const { return origin + direction * t; }
};

inline Vec3 Ray::reflect(Vec3 in, Vec3 normal) {
	return in - 2.0 * dot(in, normal) * normal;
}
