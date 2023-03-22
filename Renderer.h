#pragma once
#include "Scene.h"
class MonteCarloPathTracer {
public:

	std::vector<Vec3> frame_buffer;

	MonteCarloPathTracer(float, int);
	void write_render_result(std::string name, int width, int height);
	
	void render(Scene& scene);
	Vec3 trace_ray(Ray& ray, Scene& scene, std::shared_ptr<Intersection>& ray_inter);
private:
	float p_;
	int spp_;

	void resize_frame_buffer(int x, int y);
	Vec3 cal_direct(Ray& ray, Scene& scene, std::shared_ptr<Intersection>& ray_inter);
	//Vec3 sample_reflection_ray(Ray& ray, std::shared_ptr<Intersection>& ray_inter, float& pdf);
	Vec3 sample_reflection_ray_pdf(Ray& ray, std::shared_ptr<Intersection>& ray_inter, float& pdf);
	Vec3 cal_indirect(Ray& ray, Scene& scene, std::shared_ptr<Intersection>& ray_inter);

	Vec3 trace_reflection(Ray& ray, Scene& scene, std::shared_ptr<Intersection>& ray_inter);
	
};



