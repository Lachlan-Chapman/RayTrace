#pragma once
#include <random>

#include "vec.hpp"
namespace rng {
	inline float decimal() {
		static std::uniform_real_distribution<float> distribution(0.0, 1.0);
		static std::mt19937 generator(935);
		return distribution(generator);
	}

	inline float decimal(double p_min, double p_max) {
		return p_min + (p_max - p_min) * decimal();
	}


	inline vec3f vector() {
		return vec3f{
			rng::decimal(),
			rng::decimal(),
			rng::decimal()
		}.unit();
	}

	inline vec3f insideUnitCircle() {
		vec3f sample;
		while(sample.square_length() > 1.0) {
			sample = vec3f{
				rng::decimal(),
				rng::decimal(),
				0.0
			};
		}
		return sample;
	}
}