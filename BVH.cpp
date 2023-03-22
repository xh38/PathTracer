#include "BVH.h"

#include <queue>

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

bool BVHAccel::intersect(Ray& ray, std::shared_ptr<Intersection>& inter) {
	if(root == nullptr)
	{
		throw std::runtime_error("root is nullptr");
	}
	return intersect(root, ray, inter);
}

bool BVHAccel::intersect(const std::shared_ptr<BVHNode>& node, Ray& ray, std::shared_ptr<Intersection>& inter) {
	//node is nullptr
	bool flag = false;
	if (node==nullptr || !node->box.intersect(ray))
	{
		return flag;
	}

	if(node->is_leaf())
	{
		return node->object->intersect(ray, inter);
	}

	std::shared_ptr<Intersection> left;
	std::shared_ptr<Intersection> right;
	bool l = intersect(node->left, ray, left);
	bool r = intersect(node->right, ray, right);
	if (l && r)
	{
		if (float_equal(left->t, right->t)) {
			if (left->p_m->is_light())
			{
				inter = left;
			} else
			{
				inter = right;
			}
		}
		else
		{
			inter = left->t < right->t ? left : right;
		}
		flag = true;
	}
	else if (l){
		inter = left;
		flag = true;
	} else if (r)
	{
		inter = right;
		flag = true;
	}
	return flag;

}

int BVHAccel::count_leaves() {
	//if (root->left != nullptr)
	//{
	//	std::cout << "left: " << root->left->count() << std::endl;
	//}
	//if (root->right != nullptr)
	//{
	//	std::cout << "right: " << root->right->count() << std::endl;
	//}
	return root->count();
}

int BVHAccel::max_depth(const std::shared_ptr<BVHNode>& node) {
	if (node == nullptr)
	{
		return 0;
	}
	int max_l = max_depth(node->left);
	int max_r = max_depth(node->right);
	return std::max(max_l, max_r) + 1;
}

int BVHAccel::depth() {
	return max_depth(root);
}

