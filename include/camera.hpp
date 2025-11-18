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
	camera(int p_objectCount);

	void setCameraMeta(const vec2i &p_screenResolution, double p_fov, double p_defocusAngle, double p_focusDistance);
	void setPosition(const vec3 &p_position, const vec3 &p_target);
	void setRayTraceMeta(int p_sampleCount, int p_bounceLimit);
	
	
	void render() const;
	
	world m_world;
	
private:
	vec3 toGamma(const vec3 &p_color, double p_gamma = 2.2) const;
	vec3 toSRGB(const vec3 &p_color) const;
	vec3 randomDefocus() const;
	
	int m_sampleCount, m_bounceLimit;
	interval m_interval; //default range for a ray from this camera

	vec2i m_imageDimension; //image res
	vec2 m_viewportDimension; //physical view representation in world
	
	vec3 m_viewportOrigin, m_pixelOrigin; 
	
	vec3 m_viewportU, m_viewportV;
	vec3 m_pixelDeltaU, m_pixelDeltaV; //origins converting for viewport to screen res

	
	vec3 m_position, m_target;
	vec3 m_u, m_v, m_w; //camera basis vectors
	vec3 m_upVector;
	double m_focalLength, m_aspectRatio, m_sampleScale;
	double m_fov;

	double m_defocusAngle, m_focusDistance;
	vec3 m_defocusU, m_defocusV;

};