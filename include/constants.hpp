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

}