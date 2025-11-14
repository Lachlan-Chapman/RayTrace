#include "constants.hpp"
#include "ray.hpp"
#include "color.hpp"
#include "hittable.hpp"
#include "world.hpp"


ray::ray() {}
ray::ray(const vec3 &p_origin, const vec3 &p_direction, const interval &p_interval) : m_origin(p_origin), m_direction(p_direction), m_interval(p_interval) {}

const vec3& ray::origin() const {return m_origin;}
const vec3& ray::direction() const {return m_direction;}
vec3 ray::at(double p_time) const {return m_origin + (m_direction * p_time);}
color ray::pixelColor(const world& p_world) const {
	hitRecord hit_record; //just used as a formated temp space to store data. like a warehouse object.
	bool has_intersected = false; //track if we hit anything at all (by defualt consider a intersection with the universe)
	
	double closest_distance = m_interval.m_max; 
	//m_interval.m_max (to keep the function as const ill use a temp var to track the closest distance and leave the ray params as is)
	//used to track the object closest to the camera, obv this would be the one to render

	color object_color; //track the current closest objects color (this means it does the material calculation on all objects intersected sooo not great YET)

	//std::cout << m_origin << " | " << m_direction << std::endl;
	for(int i = 0; i < p_world.size(); i++) {
		if(p_world[i]->intersect(*this, interval(m_interval.m_min, closest_distance), hit_record)) {
			has_intersected = true; //alright so now weve hit something
			closest_distance = hit_record.m_time; //how far along the ray did we intersect | now this sets a new limit so if an object returns intersects, it must have been closer than this object
			object_color = 0.5 * (hit_record.m_normal + color(1.0));
		}
	}
	
	if(has_intersected) {
		return object_color;
	} else {
		double pixel_color = 0.5 * (m_direction.unit()[1] + 1);
		return ((1.0 - pixel_color) * color(1.0)) + (pixel_color * color{0.5, 0.7, 1.0}); //universe color as default
	}

}