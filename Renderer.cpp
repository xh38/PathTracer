#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "Renderer.h"
#include <chrono>
#include <thread>
#include <omp.h>


MonteCarloPathTracer::MonteCarloPathTracer(double p, int spp) {
	p_ = p;
	spp_ = spp;
	frame_buffer = std::vector<Vec3>();
}

void MonteCarloPathTracer::write_render_result(std::string name, int width, int height) {
	float gamma = 2.5;
	int len = width * height * 3;
	uint8_t* data = static_cast<uint8_t*>(std::malloc(len * sizeof(uint8_t)));
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				frame_buffer[i * width + j][k] = frame_buffer[i * width + j][k] > 1 ? 1.0 : frame_buffer[i * width + j][k];
				frame_buffer[i * width + j][k] = frame_buffer[i * width + j][k] < 0 ? 0.0 : frame_buffer[i * width + j][k];
				double correction = std::pow(frame_buffer[i * width + j][k], 1.0 / gamma);
				data[(i * width + j) * 3 + k] =static_cast<uint8_t>(correction*255);
				//std::cout << "x: " << j << " y: " << i << " c: " << k << "\nvalue: " << data[(i * width + j) * 3 + k] << "\n";
				//data[(i * width + j) * 3 + k] = static_cast<uint8_t>(frame_buffer[i * width + j][k] * 255);
			}
		}
	}
	std::string output_name = name + "_result.png";
	stbi_write_png(output_name.c_str(), width, height, 3, data, width * 3);
}

void MonteCarloPathTracer::resize_frame_buffer(int x, int y) {
	frame_buffer.resize(x * y);
}

Vec3 MonteCarloPathTracer::cal_direct(Ray& ray, Scene& scene, const Intersection& ray_inter) {
	Vec3 direct{ 0, 0, 0 };
	double pdf_light;
	Intersection light_sample;

	//sample a point on light sources
	scene.sample_light(light_sample, pdf_light);

	const Vec3 shadow_ray_vec = light_sample.point - ray_inter.point;
	const Vec3 light_dir = normalize(shadow_ray_vec);
	const double dis = shadow_ray_vec.length();

	Ray shadow_ray{ ray_inter.point, light_dir};
	const Intersection shadow_ray_inter = scene.get_closest_intersection_bvh(shadow_ray);

	if (!shadow_ray_inter.happened || std::abs(shadow_ray_inter.t - dis) < eps)
	{
		Vec3 emit = light_sample.p_m->radiance;
		Vec3 light_noraml = light_sample.normal;
		// diffuse part
		if (ray_inter.p_m->Kd.length() > 0)
		{
			direct += emit * ray_inter.p_m->lambertian_brdf(ray_inter.normal, light_dir) * dot(-light_dir, light_noraml) / pdf_light / dis / dis;
		}
		// specular part
		if (ray_inter.p_m->Ks.length() > 0)
		{
			direct += emit * ray_inter.p_m->specular_brdf(ray_inter.normal, ray.direction, light_dir) * dot(-light_dir, light_noraml) / pdf_light / dis / dis;
		}
	}
	return direct;
}

Vec3 MonteCarloPathTracer::cal_indirect(Ray& ray, Scene& scene, const Intersection& ray_inter) {
	Vec3 indirect{ 0, 0, 0 };
	Vec3 new_ori = ray_inter.point;
	auto material = ray_inter.p_m;
	auto N = ray_inter.normal;
	Vec3 r_d = ray.direction;
	Vec3 new_dir;
	double pdf;

	new_dir = ray_inter.p_m->uniform_sample(r_d, N, pdf);

	//double kd_norm = ray_inter.p_m->Kd.length();
	//double ks_norm = ray_inter.p_m->Ks.length();
	//double p_kd = kd_norm / (kd_norm + ks_norm);
	//// perfect specular
	//if (p_kd == 0)
	//{
	//	// perfect reflection
	//	new_dir = Ray::reflect(ray.direction, ray_inter.normal);
	//	pdf = 1.0;

	//}
	//// for p = 1 - p_kd, we use specular importance sampling
	//else if (p_kd < get_rand_double())
	//{
	//	new_dir = ray_inter.p_m->specular_importance_sample();
	//	
	//}
	//// for p = p_kd, we use diffuse importance sampling
	//else
	//{
	//	new_dir = ray_inter.p_m->cosine_weighted_sample(ray.direction, ray_inter.normal, pdf);
	//	pdf = dot(new_dir, ray_inter.normal);
	//}

	Ray new_ray{ new_ori, new_dir };
	Intersection new_inter = scene.get_closest_intersection_bvh(new_ray);
	if (new_inter.happened && new_inter.p_m->is_light())
	{
		return indirect;
	}
	indirect = trace_ray(new_ray, scene, new_inter) * (material->lambertian_brdf(N, new_dir) + material->specular_brdf(N, r_d, new_dir)) * dot(new_dir, N) / pdf / p_;

	return indirect;
}

Vec3 MonteCarloPathTracer::trace_ray(Ray& ray, Scene& scene, const Intersection& ray_inter) {
	Vec3 color{ 0, 0, 0 };

	if (!ray_inter.happened) 
		return color;
	//hit light
	if (ray_inter.p_m->is_light())
		return ray_inter.p_m->get_radiance();

	//std::cout << "normal abs: " <<color << std::endl;
	//direct
	color += cal_direct(ray, scene, ray_inter);

	//end with russian roulette
	if (russian_roulette(p_)) return color;

	//indirect 
	//color += cal_indirect(ray, scene, ray_inter);

	return color;
}


void MonteCarloPathTracer::render(Scene& scene)
{
	//static omp_lock_t lock;
	int w = scene.p_camera_->width_;
	int h = scene.p_camera_->height_;
	//std::cout << "rendering res:" << w << "x" << h << std::endl;
	resize_frame_buffer(w, h);
	std::fill(frame_buffer.begin(), frame_buffer.end(), Vec3( 0,0,0 ));
	//omp_init_lock(&lock);
	auto start = std::chrono::system_clock::now();
#pragma omp parallel for num_threads(16)
	for(int i = 0; i < h; i++)
	{
		for (int j =0 ; j < w; j++)
		{
			
			for (int n = 0; n < spp_; n++)
			{
				Ray ray = scene.p_camera_->get_ray(j, i);
				//Ray ray = scene.p_camera_->sample_ray(j, i);
				//std::cout << "Ray: [origin: " << ray.origin << ", direction: " << ray.direction << "]\n";
				Intersection inter = scene.get_closest_intersection_bvh(ray);
				frame_buffer[i * w + j] += trace_ray(ray, scene, inter) / (1.0 * spp_);
				//std::cout << "pixel (" << j << ", " << i << "): " << frame_buffer[i * h + j] << std::endl;;
			}
			
		}
		//omp_set_lock(&lock);
		UpdateProgress(h);
		//omp_unset_lock(&lock);
	}
	//omp_destroy_lock(&lock);

	auto end = std::chrono::system_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::cout << "\nspent: " << time.count() * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den << "seconds." << std::endl;
}
