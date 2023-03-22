#pragma once
#include "Camera.h"
#include "Intersection.h"
#include "Mesh.h"

class Scene {
public:
	Scene(const std::string& scene_name);

	std::unique_ptr<Camera> p_camera_;
	std::vector<std::shared_ptr<Triangle>> lights_;
	float light_area_sum;
    std::unique_ptr<TriangleMesh> mesh_;
	std::vector<std::shared_ptr<Material>> materials_;
	BVHAccel* BVH;
	int width_, height_;
	//render func
	bool get_closest_intersection(Ray& ray, std::shared_ptr<Intersection>& inter);
	bool get_closest_intersection_bvh(Ray& ray, std::shared_ptr<Intersection>& inter);
	//bool intersect(Ray& ray, float t_near, float t_far);
	void sample_light(Intersection& light_sample, float& pdf);
};




