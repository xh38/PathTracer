#include "Material.h"
#include "Ray.h"
Vec3 Material::uniform_sample(Vec3 in_dir, Vec3 normal, float& pdf) {
	float x_1 = get_rand_float();
	float x_2 = get_rand_float();

	float z = std::fabs(1.0 - 2.0 * x_1);
	float r = std::sqrt(1.0 - z * z);
	float phi = 2 * pi * x_2;
	Vec3 local_ray_dir{ r * std::cos(phi), r * std::sin(phi), z };
	pdf = 0.5 / pi;
	return to_world(local_ray_dir, normal);
}

Vec3 Material::cosine_weighted_sample(Vec3 in_dir, Vec3 normal, float& pdf) {
	float x1 = get_rand_float();
	float x2 = get_rand_float();
	float phi = 2 * pi * x2;
	float z = std::sqrt(x1);
	float sin_theta = std::sqrt(1.0 - z * z);
	Vec3 local_ray_dir{ sin_theta * std::cos(phi), sin_theta * std::sin(phi), z };
	pdf = z / pi;
	return to_world(local_ray_dir, normal);
}

Vec3 Material::specular_importance_sample(Vec3 in_dir, Vec3 normal, float& pdf) {
	Vec3 perfect_reflection = Ray::reflect(in_dir, normal);
	float x1 = get_rand_float();
	float x2 = get_rand_float();
	float phi = 2 * pi * x2;
	float z = std::pow(x1, 1/(Ns + 1));
	float sin_theta = std::sqrt(1.0 - z * z);
	Vec3 local_ray_dir{ sin_theta * std::cos(phi), sin_theta * std::sin(phi), z };
	pdf = (Ns + 1) * std::pow(z, Ns) * 0.5 / pi;
	return to_world(local_ray_dir, perfect_reflection);
}

Vec3 Material::specular_brdf(Vec3 normal, Vec3 in, Vec3 out) {
	Vec3 view_dir = -in;
	Vec3 half_way_vector = normalize(out + view_dir);
	float cos_alpha = dot(half_way_vector, normal);
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

float Material::specular_pdf(Vec3 sampled, Vec3 reflected) {
	return (Ns + 1) * std::pow(dot(sampled, reflected), Ns) * 0.5 / pi;
}

float Material::cosine_weighted_pdf(Vec3 sampled, Vec3 normal) {
	return dot(sampled, normal) / pi;
}

float Material::schlick_fresnel(Vec3 in, Vec3 normal, float n1, float n2) {
	float r0 = pow((n1 - n2) / (n1 + n2), 2);
	float costheta = dot(-in, normal);
	float reflection = r0 + (1.0f - r0) * pow((1.0f - costheta), 5);
	return reflection;
}

//vec3 refract(const vec3& uv, const vec3& n, float etai_over_etat) {
//	auto cos_theta = fmin(dot(-uv, n), 1.0);
//	vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
//	vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
//	return r_out_perp + r_out_parallel;
//}

bool Material::refract(Vec3 in, Vec3 normal, float n1, float n2, Vec3& refract_dir) {
	float cos_1 = dot(in, normal);
	float sin_1 = std::sqrt(1.0 - cos_1 * cos_1);
	float sin_2 = n2 / n1 * sin_1;
	float cos_2 = std::sqrt(1.0 - sin_2 * sin_2);

	//if (cosphi2 >= 0.0f)
	//{
	//	refract_dir = (n1 / n2) * in - (n1 / n2) * cos_1 * normal - normal * sqrt(cosphi2);
	//	return true;
	//}
	//else 
	return false;
}

Vec3 Material::to_world(const Vec3& local, const Vec3& normal) {

	Vec3 local_y;
	if (std::fabs(normal.x()) > std::fabs(normal.y()))
	{
		const float inv_length = 1.0 / std::sqrt(normal.x() * normal.x() + normal.z() * normal.z());
		local_y = Vec3{ normal.z() * inv_length, 0.0, -normal.x() * inv_length };
	}
	else
	{
		const float inv_length = 1.0 / std::sqrt(normal.y() * normal.y() + normal.z() * normal.z());
		local_y = Vec3{ 0.0, normal.z() * inv_length,  -normal.y() * inv_length };
	}
	const Vec3 local_x = cross(local_y, normal);
	return local.x() * local_x + local.y() * local_y + local.z() * normal;
}
