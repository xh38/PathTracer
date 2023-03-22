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

bool Triangle::intersect(Ray& ray, std::shared_ptr<Intersection>& inter) {
	const Vec3 p_vec = cross(ray.direction, e2);
	const float det = dot(e1, p_vec);

	// ray parallel to triangle
	if (float_equal(det, 0.0f))
	{
		return false;
	}

	// compute u
	const Vec3 t_vec = ray.origin - v0;
	float u = dot(t_vec, p_vec);
	if (u < 0.0f || u > det)
	{
		return false;
	}

	// compute v
	const Vec3 q_vec = cross(t_vec, e1);
	float v = dot(ray.direction, q_vec);
	if (v < 0.0f || u + v > det)
	{
		return false;
	}

	const float inv_det = 1.0f / det;
	const float t = dot(e2, q_vec) * inv_det;
	u *= inv_det;
	v *= inv_det;
	if (t < eps)
	{
		return false;
	}

	inter = std::make_shared<Intersection>();
	inter->t = t;
	inter->p_m = p_m;
	inter->normal = normal;
	inter->happened = true;
	inter->point = ray(t);
	inter->v0 = this->v0;
	inter->v1 = this->v1;
	inter->v2 = this->v2;
	inter->u = u;
	inter->v = v;
	inter->t0 = this->t0;
	inter->t1 = this->t1;
	inter->t2 = this->t2;
	return true;
}

AABB Triangle::get_aabb() {
	AABB temp(v0, v1);
	return AABB::merge(temp, v2);
}

void Triangle::sample_point(Intersection& sample, float& pdf) {
	float x = std::sqrt(get_rand_float()), y = get_rand_float();
	sample.point = v0 * (1 - x) + v1 * (1 - y) * x + v2 * x * y;
	sample.normal = this->normal;
	sample.p_m = this->p_m;
	pdf = 1 / area ;
}
