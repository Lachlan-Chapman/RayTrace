#pragma once

#include <random>

#include "vec.hpp"
#include "ray.hpp"
#include "PPM.hpp"
#include "world.hpp"
#include "interval.hpp"

class camera {
public:
	camera();
	camera(vec2i, double, int, int);

	vec3 viewportU() const;
	vec3 viewportV() const;

	void render();

	world m_world;
	interval m_interval; //default range for a ray from this camera
	int m_sample_count, m_max_bounce;
	
private:
	vec3 getRay(vec2i);

	vec2i m_image_dimension;
	vec2 m_viewport_dimension;
	vec3 m_position = {0.0, 0.0, 0.0}; //default at world origin
	vec3 m_viewport_origin, m_pixel_delta_u, m_pixel_delta_v, m_pixel_origin;
	double m_focal_length, m_aspect_ratio, m_sample_scale;

};