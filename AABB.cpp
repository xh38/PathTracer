#include "AABB.h"

AABB AABB::merge(AABB box1, AABB box2) {
	Vec3 new_min{ std::min(box1.p_min_[0], box2.p_min_[0]),
				  std::min(box1.p_min_[1], box2.p_min_[1]),
				  std::min(box1.p_min_[2], box2.p_min_[2]) };
	Vec3 new_max{ std::max(box1.p_max_[0], box2.p_max_[0]),
				  std::max(box1.p_max_[1], box2.p_max_[1]),
				  std::max(box1.p_max_[2], box2.p_max_[2]), };

	return {new_min, new_max};
}

AABB AABB::merge(AABB box, Vec3 p) {
	Vec3 new_min{ std::min(box.p_min_[0], p[0]),
				  std::min(box.p_min_[1], p[1]),
				  std::min(box.p_min_[2], p[2]) };
	Vec3 new_max{ std::max(box.p_max_[0], p[0]),
				  std::max(box.p_max_[1], p[1]),
				  std::max(box.p_max_[2], p[2]), };
	return { new_min, new_max };
}

float AABB::get_middle(int axis) {
	return 0.5 * (p_min_[axis] + p_max_[axis]);
}

bool AABB::intersect(Ray& ray) {
	Vec3 inv_dir{ 1.0f / ray.direction.x(), 1.0f / ray.direction.y(), 1.0f / ray.direction.z() };

	//float t_x_1 = (p_min_.x() - ray.origin.x()) * inv_dir.x();
	//float t_x_2 = (p_max_.x() - ray.origin.x()) * inv_dir.x();

	//float t_y_1 = (p_min_.y() - ray.origin.y()) * inv_dir.y();
	//float t_y_2 = (p_max_.y() - ray.origin.y()) * inv_dir.y();

	//float t_z_1 = (p_min_.z() - ray.origin.z()) * inv_dir.z();
	//float t_z_2 = (p_max_.z() - ray.origin.z()) * inv_dir.z();
	//float t_max = std::numeric_limits<float>::max();
	//float t_min = 0.0;
	//for (int i = 0; i < 3; i++)
	//{
	//	float t_enter = (p_min_[i] - ray.origin[i]) * inv_dir[i];
	//	float t_exit = (p_max_[i] - ray.origin[i]) * inv_dir[i];
	//	if (inv_dir[i] < 0)
	//	{
	//		std::swap(t_enter, t_exit);
	//	}
	//	t_max = std::min(t_exit, t_max);
	//	t_min = std::max(t_enter, t_min);
	//	if (t_max < t_min)
	//	{
	//		return false;
	//	}
	//}
	//return t_max > eps;

	Vec3 t1 = (p_min_ - ray.origin) * inv_dir;
	Vec3 t2 = (p_max_ - ray.origin) * inv_dir;

	float t_enter = std::max(std::max(std::min(t1.x(), t2.x()), std::min(t1.y(), t2.y())), std::min(t1.z(), t2.z()));
	float t_exit = std::min(std::min(std::max(t1.x(), t2.x()), std::max(t1.y(), t2.y())), std::max(t1.z(), t2.z()));

	if (t_enter <= t_exit && t_exit > eps)
	{
		return true;
	}

	return false;
}

int AABB::get_max_extent() {
	Vec3 diagonal = p_max_ - p_min_;
	if (diagonal.x() > diagonal.y() && diagonal.x() > diagonal.z())
		return 0;
	if (diagonal.y() > diagonal.z())
		return 1;
	return 2;
}
