#include "vec.hpp"
#include "material.hpp"
#include "constant.hpp"
#include "random.hpp"

material::material() : material(color(1.0), 1.0) {}
material::material(const color &p_albedo, double p_reflectance) : m_albedo(p_albedo), m_reflectance(p_reflectance) {}
vec3 material::diffuse() {
	double rand_1 = rng::decimal();
	double rand_2 = rng::decimal();
	double z = std::sqrt(1 - rand_2);

	double phi = 2 * constant::PI * rand_1;
	double x = std::cos(phi) * std::sqrt(rand_2);
	double y = std::sin(phi) * std::sqrt(rand_2);

	return vec3{x, y, z};
}

vec3 material::diffuseFast(const vec3 &p_normal) {
	vec3 in_sphere = rng::vector();
	return in_sphere.dot(p_normal) < 0 ? -in_sphere : in_sphere;
}

vec3 material::mirror(const vec3 &p_ray, const vec3 &p_normal) {
	return p_ray - (2 * p_ray.dot(p_normal) * p_normal);
}