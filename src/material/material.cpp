#include "vec.hpp"
#include "constant.hpp"
#include "random.hpp"
#include "material.hpp"

material::material() : material(color(1.0), 1.0) {}
material::material(const color &p_albedo, double p_reflectance) : m_albedo(p_albedo), m_reflectance(p_reflectance) {}

//so if we have a normalised local dir and we then want to put it into the space of another dir, we pass the world space normal and it will rotate to match
//use case could be, i have a diffuse vec and then i want to get it applied to the surface normal, i put the diffuse dir into the normals space then i can go from the hit point by this new dir
vec3f material::transformByNormal(const vec3f &p_localDirection, const vec3f &p_normal) {
	vec3f w = p_normal;
	vec3f a = std::abs(w.z) < 0.999 ? vec3f(0, 0, 1) : vec3f(1, 0, 0); //0.999 allows for float error | this tells us the forward dir to use
	vec3f u = a.cross(w).unit(); //tangent //our new x | remember that cross produces a new vec normal to both input vectors
	vec3f v = w.cross(u).unit(); //bitangent //our new y
	//z will always be +
	return 
		(u * p_localDirection.x) +
		(v * p_localDirection.y) +
		(w * p_localDirection.z);
}

vec3f material::diffuse() {
	double rand_1 = rng::decimal();
	double rand_2 = rng::decimal();
	double phi = 2 * constant::PI * rand_1;

	double z = std::sqrt(1 - std::sqrt(rand_2)); //z will always be + //double sqrt pushes the z to +1 than +0

	double x = std::cos(phi) * std::sqrt(rand_2);
	double y = std::sin(phi) * std::sqrt(rand_2);

	return vec3f{x, y, z};
}

vec3f material::diffuseFast(const vec3f &p_normal) {
	vec3f in_sphere = rng::vector();
	return in_sphere.dot(p_normal) < 0 ? -in_sphere : in_sphere;
}

vec3f material::mirror(const vec3f &p_ray, const vec3f &p_normal) {
	return p_ray - (2 * p_ray.dot(p_normal) * p_normal);
}