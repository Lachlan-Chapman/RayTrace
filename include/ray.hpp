#pragma once
#include "vec.hpp"
#include "color.hpp"
#include "ray.hpp"

class ray {
public:
	ray();
	ray(const vec3&, const vec3&);

	const vec3& origin() const;
	const vec3& direction() const;
	color pixelColor() const;

	vec3 at(double) const;
private:
	vec3 m_origin, m_direction;
};