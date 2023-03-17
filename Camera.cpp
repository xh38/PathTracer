#include "Camera.h"

Camera::Camera(Vec3 eye, Vec3 lookat, Vec3 up, double fovy, int width, int height) {
	position_ = eye;
	width_ = width;
	height_ = height;

	const double aspect_ratio = 1.0 * width_ / height_;
	const double theta = degrees_to_radians(fovy);
	const double h = tan(theta / 2.0);
	const double viewport_h = 2.0 * h;
	const double viewport_w = aspect_ratio * viewport_h;

	const Vec3 w = normalize(eye - lookat);
	const Vec3 u = normalize(cross(up, w));
	const Vec3 v = cross(w, u);

	horizontal_ = viewport_w * u;
	vertical_ = viewport_h * v;

	lower_left_corner_ = position_ - horizontal_ / 2 - vertical_ / 2 - w;
	upper_left_corner_ = position_ - horizontal_ / 2 + vertical_ / 2 - w;
}

Ray Camera::get_ray(const int x, const int y) {
	const double s = 1.0 * (x + 0.5) / width_;
	const double t = 1.0 * (y + 0.5) / height_;

	return { position_, upper_left_corner_ + s * horizontal_ - t * vertical_ - position_ };
}

Ray Camera::sample_ray(int x, int y) {
	const double s = 1.0 * (x + get_rand_double()) / width_;
	const double t = 1.0 * (y + get_rand_double()) / height_;

	return { position_, upper_left_corner_ + s * horizontal_ - t * vertical_ - position_ };
}