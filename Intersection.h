#pragma once
#include "Vector.h"
#include "Material.h"
#include <limits>

class Intersection {
public:
	Intersection(){
		happened = false;
		t = std::numeric_limits<float>::max();
	}
	bool happened;
	float t;
	Vec3 point;
	Vec3 normal;
	std::shared_ptr<Material> p_m;
	tex_coord t0, t1, t2;
	Vec3 v0, v1, v2;
	float u, v;
};
