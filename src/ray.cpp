#include "ray.hpp"
#include "color.hpp"
#include "hittable.hpp"
#include "world.hpp"


ray::ray() {}
ray::ray(const vec3 &p_origin, const vec3 &p_direction) : m_origin(p_origin), m_direction(p_direction) {}

const vec3& ray::origin() const {return m_origin;}
const vec3& ray::direction() const {return m_direction;}
vec3 ray::at(double p_t) const {return m_origin + (m_direction * p_t);}
color ray::pixelColor(const world& p_world) const {
	hitRecord hit_record;
	for(int i = 0; i < p_world.size(); i++) {
		if(p_world[i]->intersect(*this, {0, inf}, hit_record)) {
			//std::cout << "Normal " << hit_record.m_normal << std::endl;
			return 0.5 * (hit_record.m_normal + color(1.0));
		}
	}
	double pixel_color = 0.5 * (m_direction.unit()[1] + 1);
	return ((1.0 - pixel_color) * color(1.0)) + (pixel_color * color{0.5, 0.7, 1.0});
}