#pragma once

#include <random>

#include "vec.hpp"
#include "ray.hpp"
#include "PPM.hpp"
#include "world.hpp"
#include "interval.hpp"

struct cameraConfig {
	vec3 d_position, d_target, d_upVector;
	double d_focusDistance, d_defocusAngle, d_fov;
};

class camera {
public:
	camera();
	camera(const world* const p_world, const vec3 &p_position, double p_defocusAngle, double p_fov);



	void init(const vec2i &p_imageResolution, const vec3 &p_target, const vec3 &p_upVector, double p_focusDistance);
	
	color renderPixel(const vec2i &p_pixelCoordinate, int p_sampleCount, int p_bounceLimit) const;
	
private:
	vec3 randomDefocus() const;
	
	vec3 m_position;

	vec3 m_pixelOrigin; 
	
	vec3 m_pixelDeltaU, m_pixelDeltaV; //origins converting for viewport to screen res
	
	double m_aspectRatio, m_sampleScale;
	double m_fov;

	double m_defocusAngle;
	vec3 m_defocusU, m_defocusV;

	const world* const m_world; //fixed ptr and and fixed contents

};