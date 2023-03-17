#pragma once
#include <string>

#include "Vector.h"

class Texture {
public:
	Texture() {}
	Texture(std::string& path);
	Vec3 get_color(double u, double v);

	int width, height, channel;
	float* data_;
};
