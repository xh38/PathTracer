#pragma once
#include "Camera.h"
#include "Intersection.h"
#include "Mesh.h"

//TODO : build a BVH to acc

class Scene {
public:
	Scene(const std::string& scene_name);

	std::unique_ptr<Camera> p_camera_;
	std::vector<std::shared_ptr<Triangle>> lights_;
	double light_area_sum;
    std::unique_ptr<TriangleMesh> mesh_;
	std::vector<std::shared_ptr<Material>> materials_;
	BVHAccel* BVH;
	int width_, height_;
	//render func
	Intersection get_closest_intersection(Ray& ray);
	Intersection get_closest_intersection_bvh(Ray& ray);
	//bool intersect(Ray& ray, double t_near, double t_far);
	void sample_light(Intersection& light_sample, double& pdf);


	
};



