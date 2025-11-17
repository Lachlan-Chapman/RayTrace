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
	camera(int);

	void setCameraMeta(const vec2i&, double, double, double);
	void setPosition(const vec3&, const vec3&);
	void setRayTraceMeta(int, int);
	
	
	void render();
	
	world m_world;
	interval m_interval; //default range for a ray from this camera
	int m_sample_count, m_bounce_limit;
	
private:
	vec3 toGamma(vec3, double) const;
	vec3 toSRGB(vec3) const;
	vec3 randomDefocus() const;

	vec2i m_image_dimension; //image res
	vec2 m_viewport_dimension; //physical view representation in world
	vec3 m_viewport_u, m_viewport_v;

	vec3 m_viewport_origin, m_pixel_delta_u, m_pixel_delta_v, m_pixel_origin; //origins converting for viewport to screen res
	
	vec3 m_position, m_target;
	vec3 m_u, m_v, m_w; //camera basis vectors
	vec3 m_up_vector;
	double m_focal_length, m_aspect_ratio, m_sample_scale;
	double m_fov;

	double m_defocus_angle, m_focus_distance;
	vec3 m_defocus_u, m_defocus_v;

};