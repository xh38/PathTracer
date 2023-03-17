#pragma once
#include "Vector.h"
#include "Material.h"
#include <limits>

class Intersection {
public:
	Intersection();
	bool happened;
	double t;
	Vec3 point;
	Vec3 normal;
	std::shared_ptr<Material> p_m;
	Vec3 v0, v1, v2;
};

inline Intersection::Intersection() {
	happened = false;
	t = std::numeric_limits<double>::max();
	point = normal = { 0, 0, 0 };
	p_m = nullptr;
	v0 = v1 = v2 = { 0, 0, 0 };
}
