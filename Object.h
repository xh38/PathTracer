#pragma once
#include "Ray.h"
#include "Vector.h"
#include "Intersection.h"
#include "AABB.h"

class Object {
public:
	Object() {}
	virtual bool intersect(Ray& r, Intersection& inter) = 0;
	virtual AABB get_aabb() = 0;
};
