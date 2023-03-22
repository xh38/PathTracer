#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
Texture::Texture(std::string& path) {
	std::cout << "load texture from: " << path << std::endl;
	data_ = stbi_loadf(path.c_str(), &width, &height, &channel, 0);
	//std::cout << "test get uv at (0.5, 0.5): " << get_color(0.5, 0.5) << std::endl;
}

Vec3 Texture::get_color(float u, float v) {
	int x = (int)(u * width - 0.5f) % width;
	int y = (int)(v * height - 0.5f) % height;
	x = x < 0 ? x + width : x;
	y = y < 0 ? y + height : y;

	Vec3 color = Vec3(data_[3 * width * y + 3 * x],
		data_[3 * width * y + 3 * x + 1],
		data_[3 * width * y + 3 * x + 2]) / 255.0;

	return color;
}
