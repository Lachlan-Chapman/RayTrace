#include "constant.hpp"
#include "ray.hpp"
#include "color.hpp"
#include "hittable.hpp"
#include "world.hpp"


ray::ray() {}
ray::ray(const vec3f &p_origin, const vec3f &p_direction, const interval &p_interval) : m_origin(p_origin), m_direction(p_direction), m_interval(p_interval) {}

vec3f ray::at(double p_time) const {return m_origin + (m_direction * p_time);}

color ray::traceColor(const world& p_world, int p_maxBounce) const {
	ray current_ray = *this;
	color attenuation(1.0); //start off full white (full energy)
	for(int bounce_id = 0; bounce_id <= p_maxBounce; bounce_id++) {
		hitRecord _record; //record for this bounce instance
		if(p_world.intersect(current_ray, interval(0.001, constant::INF), _record)) { //did we hit a shape & which one was it
			ray scattered;
			color material_attenuation;
			//std::clog << "Intersected @ bounce " << bounce_id << std::endl;
			if(_record.m_material->reflect(current_ray, _record, material_attenuation, scattered)) { //true means this material scattered
				attenuation *= material_attenuation; //combine the colors
				current_ray = scattered;
			} else {
				return color(0.5); //the material said scattering is no longer so all light was absorbed
			}
		} else {
			double t = 0.5 * (current_ray.m_direction.unit()[1] + 1.0);
			color sky = (1.0 - t) * color(1.0) + t * color{0.5, 0.7, 1.0};
			return attenuation * sky;
		}
	}
	
	//return color{1.0, 0.0, 1.0}; //debug magenta for dead rays
	return attenuation; //dead bounce limit
}