#include <iostream>
#include <fstream>
#include <chrono>

#include "constant.hpp"
#include "random.hpp"

#include "hittable.hpp"
#include "material.hpp"
#include "world.hpp"

#include "PPM.hpp"
#include "camera.hpp"
#include "renderer.hpp"

#include "tile.hpp"

#include "profiler.hpp"

#define IMAGE_WIDTH 100
#define IMAGE_HEIGHT 100
#define FOV 20
#define OBJ_COUNT 128

#ifndef GIT_HASH
#define GIT_HASH "default"
#endif

void generateWorld(world &p_world) {
	for(int i = 0; i < OBJ_COUNT-5; i++) {
		material *_mat;
		double mat_selection = rng::decimal();
		
		color _col = (rng::vector() + color(0.2)).unit(); //brighten it slightly
		vec3f _pos = vec3f{rng::decimal(-25.0, 14.0), 0.0, rng::decimal(-13, 7)} * rng::vector();
		
		double _rad = 0.2;
		_pos[1] = _rad;

		if(mat_selection < 0.5) {
			_mat = new metal(
				_col,
				reflectance(1.0),
				roughness(0.0)
			);
		} else if(mat_selection < 0.75) {
			_mat = new dielectric(
				_col,
				reflectance(1.0),
				ior(1.5)
			);
		} else {
			_mat = new metal(
				_col,
				reflectance(1.0),
				roughness(rng::decimal(0.7, 1.0))
			);
		}
		
		p_world.append(new sphere(
			_pos,
			_rad,
			_mat
		));
	}
}

void benchmarkVector() {
	vec3f v(1, 2, 3);
	std::clog << GIT_HASH;
	{
		scopeTimer _timer(" [10'000'000] * vec3f.unit()", std::clog);
		for(int i = 0; i < 10'000'000; i++) {
			v.unit();
		}
	}
}

void benchmarkRender(const world &p_world) {
	PPM _image("image.ppm", vec2i{IMAGE_WIDTH, IMAGE_HEIGHT});

	cameraConfig _config;
	_config.d_position = vec3f{15.0, 2.0, 4.0};
	_config.d_target = vec3f{0.0, 1.0, 0.0};
	_config.d_upVector = vec3f{0.0, 1.0, 0.0};
	_config.d_focusDistance = 10.0;
	_config.d_defocusAngle = constant::PI / 18;
	_config.d_fov = FOV * 0.0174532925199;
	
	renderer _renderer(&_image, &p_world, _config);
	int sample = 1;
	int bounce = 1;
	int ray_count = IMAGE_WIDTH * IMAGE_HEIGHT * sample * bounce;
	double st_time;
	{
		steadyTimer st_timer;
		st_timer.start();
		_renderer.renderImage(sample, bounce, vec2i{4, 1}); //scan lines appear to have an edge perhaps with cache locality
		st_time = st_timer.milliseconds();
	}
	std::clog << GIT_HASH << " " << ray_count << " Rays @ " << st_time << "(" << (st_time/ray_count) << " ms/ray" << ")" << std::endl;
	
	
	sample = 50;
	bounce = 10;
	ray_count = IMAGE_WIDTH * IMAGE_HEIGHT * sample * bounce;
	double mt_time;
	{
		steadyTimer mt_timer;
		mt_timer.start();
		_renderer.renderImageMT(sample, bounce, vec2i{4, 1}, 0); //scan lines appear to have an edge perhaps with cache locality
		mt_time = mt_timer.milliseconds();
	}
	std::clog << GIT_HASH << " " << ray_count << " Rays @ " << mt_time << "(" << (mt_time/ray_count) << " ms/ray" << ")" << std::endl;
	_renderer.saveImage();
}
 
int main(int argc, char** argv) {	
	world _world(OBJ_COUNT);
	generateWorld(_world);
	_world.append(new sphere( //ground
			position{0.0, -1000.0, 0.0},
			radius(1000),
			new lambertian(
				color(0.5),
				reflectance(1.0)
			)
		)
	);
	_world.append(new sphere( //simple diffuse
			position{-4.0, 1.0, 0.0},
			radius(1.0),
			new lambertian(
				color{0.4, 0.5, 0.9},
				reflectance(1.0)
			)
		)
	);
	_world.append(new sphere(
			position{0.0, 1.0, 0.0},
			radius(1.0),
			new dielectric(
				color(1.0),
				reflectance(1.0),
				ior(1.5)
			)
		)
	);
	_world.append(new sphere(
			position{0.0, 1.0, 0.0},
			radius(0.5),
			new dielectric(
				color(1.0),
				reflectance(1.0),
				ior(1.5)
			)
		)
	);
	_world.append(new sphere(
			position{4.0, 1.0, 0.0},
			radius(1.0),
			new metal(
				color{0.9, 0.6, 0.5},
				reflectance(1.0),
				roughness(0.0)
			)
		)
	);

	for(int test_id = 0; test_id < 5; test_id++) {
		std::clog << "Test " << test_id << std::endl;
		benchmarkVector();
		benchmarkRender(_world);
	}


	return 0;
}