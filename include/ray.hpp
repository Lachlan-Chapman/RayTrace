#pragma once
#include "vec.hpp"
#include "color.hpp"
#include "interval.hpp"

class world; //forwad decleration as world needs hittable needs ray needs world

class ray {
public:
	ray();
	ray(const vec3&, const vec3&, const interval&);

	const vec3& origin() const;
	const vec3& direction() const;
	color pixelColor(const world&) const;

	vec3 at(double) const;
private:
	vec3 m_origin, m_direction;
	interval m_interval;
};