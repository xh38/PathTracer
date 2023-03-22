#pragma once
#include <string>

#include "Vector.h"

class Texture {
public:
	Texture() {}
	Texture(std::string& path);
	Vec3 get_color(float u, float v);

	int width, height, channel;
	float* data_;
};
