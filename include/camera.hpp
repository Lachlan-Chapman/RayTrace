#pragma once

#include <random>

#include "vec.hpp"
#include "ray.hpp"
#include "PPM.hpp"
#include "world.hpp"
#include "interval.hpp"

struct cameraConfig {
	vec3d d_position, d_target, d_upVector;
	double d_focusDistance, d_defocusAngle, d_fov;
};

class camera {
public:
	camera();
	camera(const world* const p_world, const vec3d &p_position, double p_defocusAngle, double p_fov);

	void init(const vec2i &p_imageResolution, const vec3d &p_target, const vec3d &p_upVector, double p_focusDistance);
	
	color renderPixel(const vec2i &p_pixelCoordinate, int p_sampleCount, int p_bounceLimit) const;
	
private:
	vec3f randomDefocus() const;
	
	vec3d m_position;
	vec3d m_pixelOrigin, m_pixelDeltaU, m_pixelDeltaV; //origins converting for viewport to screen res
	
	vec3f m_fPosition, m_fPixelDeltaU, m_fPixelDeltaV, m_fPixelOrigin; //float versions for render pipeline

	vec3d m_defocusU, m_defocusV;
	vec3f m_fDefocusU, m_fDefocusV;
	
	double m_aspectRatio, m_sampleScale, m_fov, m_defocusAngle;
	
	const world* const m_world; //fixed ptr and and fixed contents

};