#include "Triangle.h"

Triangle::Triangle(Vec3 v0, Vec3 v1, Vec3 v2, std::shared_ptr<Material> m) : v0(v0), v1(v1), v2(v2), p_m(m) {
	e1 = v1 - v0;
	e2 = v2 - v0;
	area = 0.5 * cross(e1, e2).length();
	normal = normalize(cross(e1, e2));
}

bool Triangle::is_light() {
	return p_m->is_light();
}

bool Triangle::intersect(Ray& ray, Intersection& inter) {
	const Vec3 p_vec = cross(ray.direction, e2);
	const double det = dot(e1, p_vec);

	// ray parallel to triangle
	if (double_equal(det, 0.0))
	{
		inter.happened = false;
		return false;
	}

	// compute u
	const Vec3 t_vec = ray.origin - v0;
	double u = dot(t_vec, p_vec);
	if (u < 0 || u > det)
	{
		inter.happened = false;
		return false;
	}

	// compute v
	const Vec3 q_vec = cross(t_vec, e1);
	double v = dot(ray.direction, q_vec);
	if (v < 0 || u + v > det)
	{
		inter.happened = false;
		return false;
	}

	const double inv_det = 1.0 / det;
	const double t = dot(e2, q_vec) * inv_det;
	//u *= inv_det;
	//v *= inv_det;
	if (t < eps)
	{
		inter.happened = false;
		return false;
	}

	inter.t = t;
	inter.p_m = p_m;
	inter.normal = normal;
	inter.happened = true;
	inter.point = ray(t);
	inter.v0 = this->v0;
	inter.v1 = this->v1;
	inter.v2 = this->v2;
	return true;
}

AABB Triangle::get_aabb() {
	AABB temp(v0, v1);
	return AABB::merge(temp, v2);
}

void Triangle::sample_point(Intersection& sample, double& pdf) {
	double x = std::sqrt(get_rand_double()), y = get_rand_double();
	sample.point = v0 * (1 - x) + v1 * (1 - y) * x + v2 * x * y;
	sample.normal = this->normal;
	sample.p_m = this->p_m;
	pdf = 1 / area ;
}
