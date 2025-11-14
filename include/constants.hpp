#pragma once

#include <limits>
#include <random>

#include "vec.hpp"

namespace constants {
	constexpr double PI = 3.1415926535897932385;
	constexpr double INF = std::numeric_limits<double>::infinity();
}

namespace rng {
	inline double decimal() {
		static std::uniform_real_distribution<double> distribution(0.0, 1.0);
		static std::mt19937 generator;
		return distribution(generator);
	}

	inline double decimal(double p_min, double p_max) {
		return p_min + (p_max - p_min) * decimal();
	}


	inline vec3 vector() {
		return vec3{
			rng::decimal(),
			rng::decimal(),
			rng::decimal()
		}.unit();
	}

	inline vec3 vector(double p_magnitude) {
		return rng::vector() * p_magnitude;
	}

	inline vec3 vectorLambertian() {
		double rand_1 = rng::decimal();
		double rand_2 = rng::decimal();
		double z = std::sqrt(1 - rand_2);

		double phi = 2 * constants::PI * rand_1;
		double x = std::cos(phi) * std::sqrt(rand_2);
		double y = std::sin(phi) * std::sqrt(rand_2);

		return vec3{x, y, z};
	}

	inline vec3 vectorLambertian_fast(const vec3& p_normal) {
		vec3 in_sphere = rng::vector();
		return in_sphere.dot(p_normal) < 0 ? -in_sphere : in_sphere;
	}
}