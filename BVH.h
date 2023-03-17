#pragma once
#include "Object.h"

struct BVHNode {
	AABB box;
	std::shared_ptr<BVHNode> left;
	std::shared_ptr<BVHNode> right;
	std::shared_ptr<Object> object;
	double area;
	//int num;
	int count() {
		int count = 0;
		if (left == nullptr && right == nullptr)
		{
			return 1;
		}
		if (left != nullptr)
		{
			count += left->count();
		}
		if (right != nullptr)
		{
			count += right->count();
		}
		return count;
	}
};


class BVHAccel {
public:
	std::shared_ptr<BVHNode> root;
	std::vector<std::shared_ptr<Object>> objects;
	BVHAccel() { root = nullptr;}
	BVHAccel(std::vector<std::shared_ptr<Object>>& p);
	std::shared_ptr<BVHNode> recurisve_build(std::vector<std::shared_ptr<Object>> p);

	Intersection intersect(Ray& ray);
	Intersection intersect(std::shared_ptr<BVHNode> node, Ray& ray);

	int count_leaves();

};  