#include "Mesh.h"

TriangleMesh::TriangleMesh(std::vector<std::shared_ptr<Object>>& triangles) {
	triangles_ = triangles;
}

bool TriangleMesh::intersect(Ray& ray, std::shared_ptr<Intersection>& inter) {
	bool flag = false;
	for (auto& triangle : triangles_)
	{
		std::shared_ptr<Intersection> temp = std::make_shared<Intersection>();
		triangle->intersect(ray, temp);
		if (temp->happened)
		{
			if (inter->t - temp->t > eps)
			{
				inter = temp;
			} else if (float_equal(inter->t, temp->t))
			{
				if (temp->p_m->is_light())
				{
					inter = temp;
				}
			}
			flag |= true;
		}
	}
	return flag;
}

AABB TriangleMesh::get_aabb() {
	throw std::runtime_error("please do not ask for aabb in mesh");
}
