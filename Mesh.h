#pragma once
#include "Triangle.h"
#include "BVH.h"

#include <vector>

class TriangleMesh : Object {
public:
	TriangleMesh(std::vector<std::shared_ptr<Object>>& triangles);
	std::vector<std::shared_ptr<Object>> triangles_;
	

	bool intersect(Ray& ray, Intersection& inter) override;
	AABB get_aabb() override;
};
