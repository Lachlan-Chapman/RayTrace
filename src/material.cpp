#include "vec.hpp"
#include "constant.hpp"
#include "random.hpp"
#include "material.hpp"

material::material() : material(color(1.0), 1.0) {}
material::material(const color &p_albedo, double p_reflectance) : m_albedo(p_albedo), m_reflectance(p_reflectance) {}
vec3f material::diffuse() {
	double rand_1 = rng::decimal();
	double rand_2 = rng::decimal();
	double z = std::sqrt(1 - rand_2);

	double phi = 2 * constant::PI * rand_1;
	double x = std::cos(phi) * std::sqrt(rand_2);
	double y = std::sin(phi) * std::sqrt(rand_2);

	return vec3f{x, y, z};
}

vec3f material::diffuseFast(const vec3f &p_normal) {
	vec3f in_sphere = rng::vector();
	return dot(in_sphere, p_normal) < 0 ? -in_sphere : in_sphere;
}

vec3f material::mirror(const vec3f &p_ray, const vec3f &p_normal) {
	return p_ray - (2 * dot(p_ray, p_normal) * p_normal);
}