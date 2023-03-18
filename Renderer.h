#pragma once
#include "Scene.h"

class MonteCarloPathTracer {
public:

	std::vector<Vec3> frame_buffer;

	MonteCarloPathTracer(double, int);
	void write_render_result(std::string name, int width, int height);
	
	void render(Scene& scene);
	Vec3 trace_ray(Ray& ray, Scene& scene, const Intersection& ray_inter);
private:
	double p_;
	int spp_;

	void resize_frame_buffer(int x, int y);
	Vec3 cal_direct(Ray& ray, Scene& scene, const Intersection& ray_inter);
	Vec3 sample_reflection_ray(Ray& ray, const Intersection& ray_inter, double& pdf);
	Vec3 cal_indirect(Ray& ray, Scene& scene, const Intersection& ray_inter);
	
};



