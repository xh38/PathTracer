#include "Mesh.h"

TriangleMesh::TriangleMesh(std::vector<std::shared_ptr<Object>>& triangles) {
	triangles_ = triangles;
}

bool TriangleMesh::intersect(Ray& ray, Intersection& inter) {
	for (auto& triangle : triangles_)
	{
		Intersection temp{};
		triangle->intersect(ray, temp);
		if (temp.happened)
		{
			if (inter.t - temp.t > eps)
			{
				inter = temp;
			} else if (double_equal(inter.t, temp.t))
			{
				if (temp.p_m->is_light())
				{
					inter = temp;
				}
			}
		}
	}
	return true;
}

AABB TriangleMesh::get_aabb() {
	throw std::runtime_error("please do not ask for aabb in mesh");
}
