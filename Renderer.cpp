#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "Renderer.h"
#include <chrono>
#include <thread>
#include <omp.h>


MonteCarloPathTracer::MonteCarloPathTracer(float p, int spp) {
	p_ = p;
	spp_ = spp;
	frame_buffer = std::vector<Vec3>();
}

void MonteCarloPathTracer::write_render_result(std::string name, int width, int height) {
	float gamma = 2.2f;
	int len = width * height * 3;
	uint8_t* data = static_cast<uint8_t*>(std::malloc(len * sizeof(uint8_t)));
	uint8_t* raw = static_cast<uint8_t*>(std::malloc(len * sizeof(uint8_t)));

	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				frame_buffer[i * width + j][k] = frame_buffer[i * width + j][k] > 1 ? 1.0f : frame_buffer[i * width + j][k];
				frame_buffer[i * width + j][k] = frame_buffer[i * width + j][k] < 0 ? 0.0f : frame_buffer[i * width + j][k];
				float correction = std::pow(frame_buffer[i * width + j][k], 1.0 / gamma);
				data[(i * width + j) * 3 + k] =static_cast<uint8_t>(correction*255);
				raw[(i * width + j) * 3 + k] = static_cast<uint8_t>(frame_buffer[i * width + j][k] * 255);
				//std::cout << "x: " << j << " y: " << i << " c: " << k << "\nvalue: " << data[(i * width + j) * 3 + k] << "\n";
				//data[(i * width + j) * 3 + k] = static_cast<uint8_t>(frame_buffer[i * width + j][k] * 255);
			}
		}
	}
	std::string output_name = name + "_result.png";
	std::string raw_name = name + "_result_raw.png";
	stbi_write_png(output_name.c_str(), width, height, 3, data, width * 3);
	stbi_write_png(raw_name.c_str(), width, height, 3, raw, width * 3);
}

void MonteCarloPathTracer::resize_frame_buffer(int x, int y) {
	frame_buffer.resize(x * y);
}

float compute(Vec3 brdf) {
	return 0.2126f * brdf[0] + 0.7152f * brdf[1] + 0.0722f * brdf[2];
}

Vec3 MonteCarloPathTracer::cal_direct(Ray& ray, Scene& scene, std::shared_ptr<Intersection>& ray_inter) {
	Vec3 direct{ 0, 0, 0 };
	float pdf_light;
	Intersection light_sample;

	//sample a point on light sources
	scene.sample_light(light_sample, pdf_light);

	const Vec3 shadow_ray_vec = light_sample.point - ray_inter->point;
	const Vec3 light_dir = normalize(shadow_ray_vec);
	const float dis = shadow_ray_vec.length();

	Ray shadow_ray{ ray_inter->point, light_dir};
	std::shared_ptr<Intersection> shadow_ray_inter;
	scene.get_closest_intersection_bvh(shadow_ray, shadow_ray_inter);

	auto material = ray_inter->p_m;
	//if (compute(material->lambertian_brdf(ray_inter->normal, light_dir) + material->specular_brdf(ray_inter->normal, ray.direction, light_dir)) < eps)
	//{
	//	return direct;
	//}

	if (shadow_ray_inter == nullptr || std::abs(shadow_ray_inter->t - dis) < eps)
	{
		Vec3 emit = light_sample.p_m->radiance;
		Vec3 light_noraml = light_sample.normal;
		auto normal = ray_inter->normal;
		// diffuse part
		if (material->Kd.length() > 0)
		{
			direct += emit * material->lambertian_brdf(normal, light_dir) * dot(-light_dir, light_noraml) / pdf_light / dis / dis;
		}
		// specular part
		if (material->Ks.length() > 0)
		{
			direct += emit * material->specular_brdf(normal, ray.direction, light_dir) * dot(-light_dir, light_noraml) / pdf_light / dis / dis;
		}
	}
	return direct;
}

//Vec3 MonteCarloPathTracer::sample_reflection_ray(Ray& ray, std::shared_ptr<Intersection>& ray_inter,  float& pdf) {
//	Vec3 new_dir{};
//	float kd_norm = ray_inter->p_m->Kd.length();
//	float ks_norm = ray_inter->p_m->Ks.length();
//	float p_kd = kd_norm / (kd_norm + ks_norm);
//	// perfect specular
//	if (float_equal(p_kd, 0))
//	{
//		// perfect reflection
//		new_dir = Ray::reflect(ray.direction, ray_inter->normal);
//		pdf = 1.0;
//	} else if (float_equal(p_kd, 1.0))
//	{
//		new_dir = ray_inter->p_m->cosine_weighted_sample(ray.direction, ray_inter->normal, pdf);
//	}
//	// for p = 1 - p_kd, we use specular importance sampling
//	else if (p_kd < get_rand_float())
//	{
//		new_dir = ray_inter.p_m->specular_importance_sample(ray.direction, ray_inter->normal, pdf);
//	}
//	// for p = p_kd, we use diffuse importance sampling
//	else
//	{
//		new_dir = ray_inter->p_m->cosine_weighted_sample(ray.direction, ray_inter->normal, pdf);
//	}
//	return new_dir;
//}

Vec3 MonteCarloPathTracer::sample_reflection_ray_pdf(Ray& ray, std::shared_ptr<Intersection>& ray_inter, float& pdf) {
	Vec3 new_dir;
	float kd_norm = ray_inter->p_m->Kd.length();
	float ks_norm = ray_inter->p_m->Ks.length();
	float p_kd = kd_norm / (kd_norm + ks_norm);
	float diffuse_pdf, specular_pdf;
	auto material = ray_inter->p_m;
	auto normal = ray_inter->normal;
	// perfect specular
	if (float_equal(kd_norm, 0.0f))
	{
		// perfect reflection
		new_dir = Ray::reflect(ray.direction, normal);
		specular_pdf = 1.0;
		diffuse_pdf = dot(new_dir, normal);
	} else if(float_equal(ks_norm, 0.0f))
	{
		new_dir = material->cosine_weighted_sample(ray.direction, normal, diffuse_pdf);
		Vec3 reflection_dir = Ray::reflect(ray.direction, normal);
		specular_pdf = material->specular_pdf(new_dir, reflection_dir);
	}
	// for p = 1 - p_kd, we use specular importance sampling
	else if (p_kd < get_rand_float())
	{
		new_dir = material->specular_importance_sample(ray.direction, normal, specular_pdf);
		diffuse_pdf = material->cosine_weighted_pdf(new_dir, normal) ;
	}
	// for p = p_kd, we use diffuse importance sampling
	else
	{
		new_dir = material->cosine_weighted_sample(ray.direction, normal, diffuse_pdf);
		Vec3 reflection_dir = Ray::reflect(ray.direction, normal);
		specular_pdf = material->specular_pdf(new_dir, reflection_dir);
	}
	pdf = p_kd * diffuse_pdf + (1 - p_kd) * specular_pdf;
	return new_dir;
}

Vec3 MonteCarloPathTracer::cal_indirect(Ray& ray, Scene& scene, std::shared_ptr<Intersection>& ray_inter) {
	Vec3 indirect{ 0, 0, 0 };
	Vec3 new_ori = ray_inter->point;
	auto material = ray_inter->p_m;
	auto N = ray_inter->normal;
	Vec3 r_d = ray.direction;

	// has refraction
	if (material->Ni > 1)
	{
		bool outside = dot(r_d, N) < 0;
		const float n1 = outside ? 1.0f : material->Ni;
		const float n2 = outside ? material->Ni : 1.0f;
		Vec3 out_normal = outside ? N : -N;

		const float p_reflection = material->schlick_fresnel(r_d, out_normal, n1, n2);
		Vec3 refract_dir;
		const bool can_refract = material->refract(r_d, N, n1, n2, refract_dir);
		if (can_refract && p_reflection < get_rand_float())
		{
			// refraction
			Ray new_ray{ new_ori, refract_dir };
			std::shared_ptr<Intersection> new_inter;
			scene.get_closest_intersection_bvh(new_ray, new_inter);
			if (new_inter->happened && new_inter->p_m->is_light())
			{
					return indirect;
			}
			indirect = trace_ray(new_ray, scene, new_inter) * material->Tr * dot(refract_dir, N) / p_;
		} else
		{
			indirect = trace_reflection(ray, scene, ray_inter);
		}
	} else
	{
		indirect = trace_reflection(ray, scene, ray_inter);
	}
	return indirect;
}

Vec3 MonteCarloPathTracer::trace_reflection(Ray& ray, Scene& scene, std::shared_ptr<Intersection>& ray_inter) {
	float pdf;
	Vec3 new_ori = ray_inter->point;
	auto material = ray_inter->p_m;
	auto N = ray_inter->normal;
	Vec3 r_d = ray.direction;
	Vec3 new_dir = sample_reflection_ray_pdf(ray, ray_inter, pdf);
	Ray new_ray{ new_ori, new_dir };
	std::shared_ptr<Intersection> new_inter;
	scene.get_closest_intersection_bvh(new_ray, new_inter);
	if (new_inter == nullptr || new_inter->p_m->is_light())
	{
		return Vec3{ 0,0,0 };
	}
	return trace_ray(new_ray, scene, new_inter) * (material->lambertian_brdf(N, new_dir) + material->specular_brdf(N, r_d, new_dir)) * dot(new_dir, N) / pdf / p_;
}

Vec3 MonteCarloPathTracer::trace_ray(Ray& ray, Scene& scene, std::shared_ptr<Intersection>& ray_inter) {
	Vec3 color{ 0, 0, 0 };

	if (ray_inter == nullptr || !ray_inter->happened)
		return color;

	//hit light
	if (ray_inter->p_m->is_light())
		return ray_inter->p_m->get_radiance();

	//std::cout << "normal abs: " <<color << std::endl;
	//auto n = ray_inter->normal;
	//color = { std::abs(n[0]), std::abs(n[1]), std::abs(n[2]) };
	//return color;
	//direct
	color += cal_direct(ray, scene, ray_inter);

	//end with russian roulette
	if (russian_roulette(p_)) return color;

	//indirect 
	color += cal_indirect(ray, scene, ray_inter);

	return color;
}


void MonteCarloPathTracer::render(Scene& scene)
{
	//static omp_lock_t lock;
	int w = scene.p_camera_->width_;
	int h = scene.p_camera_->height_;
	std::cout << "rendering res:" << w << "x" << h << " spp: " << spp_ << std::endl;
	resize_frame_buffer(w, h);
	std::fill(frame_buffer.begin(), frame_buffer.end(), Vec3( 0,0,0 ));
	//omp_init_lock(&lock);
	auto start = std::chrono::high_resolution_clock::now();
#pragma omp parallel for 
	for(int i = 0; i < h; i++)
	{
		for (int j =0 ; j < w; j++)
		{
			for (int n = 0; n < spp_; n++)
			{
				Ray ray = scene.p_camera_->sample_ray(j, i);
				std::shared_ptr<Intersection> inter;
				scene.get_closest_intersection_bvh(ray, inter);
				frame_buffer[i * w + j] += trace_ray(ray,  scene, inter) / (1.0f * spp_);
				//frame_buffer[i * w + j] += scene.trace_ray(ray, inter) / (1.0 * spp_);
			}
		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::cout << "spent: " << time.count() / 1000 << "ms." << std::endl;
}
