#pragma once
#include "vec.hpp"
#include "color.hpp"
#include "interval.hpp"

class world; //forwad decleration as world needs hittable needs ray needs world

class ray {
public:
	ray();
	ray(const vec3f &p_origin, const vec3f &p_direction, const interval &p_interval);

	vec3f at(double p_time) const;
	color traceColor(const world &p_world, int) const;

	interval m_interval;
	vec3f m_origin, m_direction;
private:
};