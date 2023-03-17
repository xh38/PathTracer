#pragma once
#include "Ray.h"
#include "Utils.h"

class Camera {
public:
	Camera(Vec3 eye, Vec3 lookat, Vec3 up, double fovy, int width, int height);
	
	Ray get_ray(int x, int y);
	Ray sample_ray(int x, int y);

	Vec3 position_;
	Vec3 lower_left_corner_, upper_left_corner_;
	Vec3 horizontal_, vertical_;
	int width_, height_;
};

