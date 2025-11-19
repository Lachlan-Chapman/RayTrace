#pragma once
#include <random>

#include "vec.hpp"
namespace rng {
	inline double decimal() {
		static std::uniform_real_distribution<double> distribution(0.0, 1.0);
		static std::mt19937 generator; //seeded with 1 by default so if the program needs randomness, seed with current time or something manually
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

	inline vec3 insideUnitCircle() {
		vec3 sample;
		while(sample.square_length() > 1.0) {
			sample = vec3{
				rng::decimal(),
				rng::decimal(),
				0.0
			};
		}
		return sample;
	}
}