#pragma once
#include "Vector.h"
#include "Utils.h"
#include "Texture.h"


class Material {
public:
	std::string name;
	Vec3 Kd;
	Vec3 Ks;
	Vec3 Tr;
	float Ns, Ni;
	bool hasEmit;
	Vec3 radiance;
	bool hasTexture;
	Texture texture;

	Vec3 reflect(const Vec3& in, const Vec3& normal) {
		return in - 2 * dot(in, normal) * normal;
	}

	bool is_light() const { return hasEmit; }

	Vec3 get_radiance() const {
		if (!hasEmit)
			throw std::runtime_error("should not acquire radiance on a surface that is not light");
		return radiance;
	}

	Vec3 uniform_sample(Vec3 in_dir, Vec3 normal, float& pdf);
	Vec3 cosine_weighted_sample(Vec3 in_dir, Vec3 normal, float& pdf);
	Vec3 specular_importance_sample(Vec3 in_dir, Vec3 normal, float& pdf);

	Vec3 specular_brdf(Vec3 normal, Vec3 in, Vec3 out);
	Vec3 lambertian_brdf(Vec3 normal, Vec3 out);

	float specular_pdf(Vec3 sampled, Vec3 reflected);
	float cosine_weighted_pdf(Vec3 sampled, Vec3 normal);
	float schlick_fresnel(Vec3 in, Vec3 normal, float n1, float n2);

	bool refract(Vec3 in, Vec3 normal, float n1, float n2, Vec3& refract_dir);

private:
	Vec3 to_world(const Vec3& local, const Vec3& normal);
};


