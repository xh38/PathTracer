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

double AABB::get_middle(int axis) {
	return 0.5 * (p_min_[axis] + p_max_[axis]);
}

bool AABB::intersect(Ray& ray) {
	Vec3 inv_dir{ 1.0 / ray.direction.x(), 1.0 / ray.direction.y(), 1.0 / ray.direction.z() };

	//double t_x_1 = (p_min_.x() - ray.origin.x()) * inv_dir.x();
	//double t_x_2 = (p_max_.x() - ray.origin.x()) * inv_dir.x();

	//double t_y_1 = (p_min_.y() - ray.origin.y()) * inv_dir.y();
	//double t_y_2 = (p_max_.y() - ray.origin.y()) * inv_dir.y();

	//double t_z_1 = (p_min_.z() - ray.origin.z()) * inv_dir.z();
	//double t_z_2 = (p_max_.z() - ray.origin.z()) * inv_dir.z();

	Vec3 t1 = (p_min_ - ray.origin) * inv_dir;
	Vec3 t2 = (p_max_ - ray.origin) * inv_dir;

	double t_enter = std::max(std::max(std::min(t1.x(), t2.x()), std::min(t1.y(), t2.y())), std::min(t1.z(), t2.z()));
	double t_exit = std::min(std::min(std::max(t1.x(), t2.x()), std::max(t1.y(), t2.y())), std::max(t1.z(), t2.z()));

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
