#pragma once
#include "Vector.h"
#include "Ray.h"
#include "Intersection.h";
class AABB {
public:
	Vec3 p_min_, p_max_;
	AABB(){};
	AABB(Vec3 p_0, Vec3 p_1) {
		p_min_ = { std::min(p_0.x(), p_1.x()),std::min(p_0.y(), p_1.y()) ,std::min(p_0.z(), p_1.z()) };
		p_max_ = { std::max(p_0.x(), p_1.x()),std::max(p_0.y(), p_1.y()) ,std::max(p_0.z(), p_1.z()) };
	}
	static AABB merge(AABB box1, AABB box2);
	static AABB merge(AABB box, Vec3 p);
	float get_middle(int axis);
	bool intersect(Ray& ray);
	int get_max_extent();
};