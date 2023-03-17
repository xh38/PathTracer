#include "Material.h"
#include "Ray.h"
Vec3 Material::uniform_sample(Vec3 in_dir, Vec3 normal, double& pdf) {
	double x_1 = get_rand_double();
	double x_2 = get_rand_double();

	double z = std::fabs(1.0 - 2.0 * x_1);
	double r = std::sqrt(1.0 - z * z);
	double phi = 2 * pi * x_2;
	Vec3 local_ray_dir{ r * std::cos(phi), r * std::sin(phi), z };
	pdf = 0.5 / pi;
	return to_world(local_ray_dir, normal);
}

Vec3 Material::cosine_weighted_sample(Vec3 in_dir, Vec3 normal, double& pdf) {
	double x1 = get_rand_double();
	double x2 = get_rand_double();
	double phi = 2 * pi * x2;
	double z = std::sqrt(x1);
	double sin_theta = std::sqrt(1.0 - z * z);
	Vec3 local_ray_dir{ sin_theta * std::cos(phi), sin_theta * std::sin(phi), z };
	pdf = z / pi;
	return to_world(local_ray_dir, normal);
}

Vec3 Material::specular_importance_sample(Vec3 in_dir, Vec3 normal, double& pdf) {
	Vec3 perfect_reflection = Ray::reflect(in_dir, normal);
	double x1 = get_rand_double();
	double x2 = get_rand_double();
	double phi = 2 * pi * x2;
	double z = 1 / std::pow(x1, Ns + 1);
	throw std::runtime_error("specular_importance_sample not impled");
}

Vec3 Material::specular_brdf(Vec3 normal, Vec3 in, Vec3 out) {
	Vec3 view_dir = -in;
	Vec3 half_way_vector = normalize(out + view_dir);
	double cos_alpha = dot(half_way_vector, normal);
	//return Ks *  std::pow(cos_alpha, Ns);
	return Ks * (Ns + 2.0) * std::pow(cos_alpha, Ns) / 2.0 / pi;
}

Vec3 Material::lambertian_brdf(Vec3 normal, Vec3 out) {
	if (dot(out, normal) > 0)
	{
		return Kd / pi * dot(out, normal);
	}
	return { 0., 0.,0. };
}

Vec3 Material::to_world(const Vec3& local, const Vec3& normal) {

	Vec3 local_y;
	if (std::fabs(normal.x()) > std::fabs(normal.y()))
	{
		const double inv_length = 1.0 / std::sqrt(normal.x() * normal.x() + normal.z() * normal.z());
		local_y = Vec3{ normal.z() * inv_length, 0.0, -normal.x() * inv_length };
	}
	else
	{
		const double inv_length = 1.0 / std::sqrt(normal.y() * normal.y() + normal.z() * normal.z());
		local_y = Vec3{ 0.0, normal.z() * inv_length,  -normal.y() * inv_length };
	}
	const Vec3 local_x = cross(local_y, normal);
	return local.x() * local_x + local.y() * local_y + local.z() * normal;

}