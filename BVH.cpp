#include "BVH.h"

#include <utility>

BVHAccel::BVHAccel(std::vector<std::shared_ptr<Object>>& p) {
	std::cout << "build BVH" << std::endl;
	root = recurisve_build(p);
}

std::shared_ptr<BVHNode> BVHAccel::recurisve_build(std::vector<std::shared_ptr<Object>> objects) {
	auto node = std::make_shared<BVHNode>(BVHNode{});

	AABB aabb;
	for (auto object : objects)
	{
		aabb = AABB::merge(aabb, object->get_aabb());
	}

	if (objects.size() == 1)
	{
		node->box = objects[0]->get_aabb();
		node->object = objects[0];
		node->left = nullptr;
		node->right = nullptr;
		//node->num = 1;
	}
	else if (objects.size() == 2)
	{
		node->left = recurisve_build(std::vector{ objects[0] });
		node->right = recurisve_build(std::vector{ objects[1] });
		node->box = AABB::merge(node->left->box, node->right->box);
		//node->num = 2;
	}
	else
	{
		//node->num = objects.size();
		const int max_extent = aabb.get_max_extent();
		if (max_extent == 0)
		{
			std::sort(objects.begin(), objects.end(), [](auto obj_1, auto obj_2) {return obj_1->get_aabb().get_middle(0) < obj_2->get_aabb().get_middle(0); });
		} else if(max_extent == 1)
		{
			std::sort(objects.begin(), objects.end(), [](auto obj_1, auto obj_2) {return obj_1->get_aabb().get_middle(1) < obj_2->get_aabb().get_middle(1); });
		} else if (max_extent == 2)
		{
			std::sort(objects.begin(), objects.end(), [](auto obj_1, auto obj_2) {return obj_1->get_aabb().get_middle(2) < obj_2->get_aabb().get_middle(2); });
		} else
		{
			throw std::runtime_error("only three");
		}

		auto begin = objects.begin();
		auto end = objects.end();
		auto mid = begin + objects.size() / 2;

		const auto left_objects = std::vector(begin, mid);
		const auto right_objects = std::vector(mid, end);

		node->left = recurisve_build(left_objects);
		node->right = recurisve_build(right_objects);
		node->box = aabb;
		
	}
	return node;
}

Intersection BVHAccel::intersect(Ray& ray) {
	if(root == nullptr)
	{
		throw std::runtime_error("root is nullptr");
	}
	return intersect(root, ray);
}

Intersection BVHAccel::intersect(std::shared_ptr<BVHNode> node, Ray& ray) {
	Intersection return_val{};
	//node is nullptr
	if (node==nullptr)
	{
		return return_val;
	}

	if (!node->box.intersect(ray))
	{
		return return_val;
	}

	if(node->left == nullptr && node->right == nullptr)
	{
		node->object->intersect(ray, return_val);
		/*if (return_val.happened)
		{
			std::cout << "hit " << return_val.p_m->name << std::endl;
		}*/
		//std::cout << "p_min: " << node->box.p_min_ << std::endl;
		//std::cout << "p_max: " << node->box.p_max_ << std::endl;
		//std::cout << "aabb_min: " << node->object->get_aabb().p_min_ << std::endl;
		//std::cout << "aabb_max: " << node->object->get_aabb().p_max_ << std::endl;
		return return_val;
	}

	Intersection left = intersect(node->left, ray);
	Intersection right = intersect(node->right, ray);
	// fix miss light!
	if (left.happened && right.happened)
	{
		if (left.t - right.t > eps) return_val = right;
		else if (right.t - left.t > eps) return_val = left;
		else
		{
			if (left.p_m->is_light()) return_val = left;
			else return_val = right;
		}
	} else if (left.happened)
	{
		return_val = left;
	} else if (right.happened)
	{
		return_val = right;
	}
	
	return return_val;
	
}

int BVHAccel::count_leaves() {
	return root->count();
}

