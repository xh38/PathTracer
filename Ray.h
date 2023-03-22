#pragma once
#include "Vector.h"
class Ray {
public:
	Vec3 origin, direction;
	Ray() {}
	Ray(Vec3 origin, Vec3 direction) : origin(origin), direction(normalize(direction)) {}
	static Vec3 reflect(Vec3 in, Vec3 normal);
	static bool refract(Vec3 in, Vec3 normal, float n1, float n2, Vec3& refract_dir);
	Vec3 operator()(float t) const { return origin + direction * t; }
};

inline Vec3 Ray::reflect(Vec3 in, Vec3 normal) {
	return in - 2.0f * dot(in, normal) * normal;
}

inline bool Ray::refract(Vec3 in, Vec3 normal, float n1, float n2, Vec3& refract_dir) {
	float nnt = n1 / n2;
	float costheta = dot(in, normal);
	float cosphi2 = 1.0f - nnt * nnt * (1.0f - costheta * costheta);
	if (cosphi2 >= 0.0f)
	{
		refract_dir = nnt * in - nnt * costheta * normal - normal * sqrt(cosphi2);
		return true;
	}
	else return false;
}
