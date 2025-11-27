#include "constant.hpp"
#include "ray.hpp"
#include "color.hpp"
#include "hittable.hpp"
#include "world.hpp"


ray::ray() {}
ray::ray(const ray &p_other) :
	m_origin(p_other.m_origin),
	m_direction(p_other.m_direction),
	m_interval(p_other.m_interval),
	m_inverseDirection(p_other.m_inverseDirection),
	m_directionSigns(p_other.m_directionSigns)
{}

ray::ray(const vec3f &p_origin, const vec3f &p_direction, const interval &p_interval) :
	m_origin(p_origin),
	m_direction(p_direction),
	m_interval(p_interval)
{
	for(int dim = 0; dim < 3; dim++) {
		m_inverseDirection[dim] = 
			(m_direction[dim] == 0.0f) ? 
			std::numeric_limits<float>::infinity() :
			(1.0f / m_direction[dim]);
		
		m_directionSigns[dim] = 
			(m_direction[dim] < 0.0f) ? 
			1 :
			0;
	}
}

vec3f ray::at(float p_time) const { return m_origin + (m_direction * p_time); }

color ray::traceColor(const world& p_world, int p_maxBounce) const {
	ray current_ray = *this;
	color attenuation(1.0); //start off full white (full energy)
	for(int bounce_id = 0; bounce_id <= p_maxBounce; bounce_id++) { //<= allows that 0 bounce allows the initial ray, 1 is 1 reflection so it matches
		hitRecord _record; //record for this bounce instance
		if(p_world.intersect(current_ray, interval(0.001, constant::INF), _record)) { //did we hit a shape & which one was it
			//std::clog << "Object hit" << std::endl;
			ray scattered;
			color material_attenuation;
			if(_record.m_material->reflect(current_ray, _record, material_attenuation, scattered)) { //true means this material scattered
				attenuation *= material_attenuation; //combine the colors
				current_ray = scattered;
			} else {
				return color(0.0); //the material said scattering is no longer so all light was absorbed
			}
		} else {
			float t = 0.5 * (current_ray.m_direction[1] + 1.0);
			color sky = (1.0 - t) * color(1.0) + t * color{0.5, 0.7, 1.0};
			//color sky = (1.0 - t) * color(1, 0, 0) + t * color(0, 1, 0); //debug green to red sky
			//std::clog << "Sky Hit @ " << current_ray.m_origin << std::endl;
			return attenuation * sky;
		}
	}
	
	//return color{1.0, 0.0, 1.0}; //debug magenta for dead rays
	return attenuation; //dead bounce limit
}