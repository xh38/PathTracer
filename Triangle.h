#pragma once

#include "Object.h"
#include "Ray.h"
#include "Material.h"
#include "Utils.h"

class Triangle: public Object {
public:
	Vec3 v0, v1, v2;
    Vec3 e1, e2;
	Vec3 normal;
	double area;
    std::shared_ptr<Material> p_m;

	Triangle(Vec3 v0, Vec3 v1, Vec3 v2, std::shared_ptr<Material> m);
	bool is_light();
	bool intersect(Ray& ray, Intersection& inter) override;
	AABB get_aabb() override;

	void sample_point(Intersection& sample, double& pdf);
};
