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

void generateWorld(world &p_world) {
	for(int i = 0; i < 122; i++) {
		material *_mat;
		double mat_selection = rng::decimal();
		
		color _col = (rng::vector() + color(0.2)).unit(); //brighten it slightly
		vec3 _pos = vec3{rng::decimal(-25.0, 14.0), 0.0, rng::decimal(-13, 7)} * rng::vector();
		
		double _rad = 0.2;
		_pos[1] = _rad;

		if(mat_selection < 0.5) {
			_mat = new metal(
				_col,
				1.0,
				0.0
			);
		} else if(mat_selection < 0.75) {
			_mat = new dielectric(
				_col,
				1.0,
				1.5
			);
		} else {
			_mat = new metal(
				_col,
				1.0,
				rng::decimal(0.7, 1.0)
			);
		}
		
		p_world.append(new sphere(
			_pos,
			_rad,
			_mat
		));
	}
}
 
int main(int argc, char** argv) {
	// return 0;

	world _world(128);
	//generateWorld(_world);
	_world.append(new sphere( //ground
			vec3{0.0, -1000.0, 0.0},
			1000,
			new lambertian(
				color(0.5),
				1.0
			)
		)
	);
	_world.append(new sphere( //simple diffuse
			vec3{-4.0, 1.0, 0.0},
			1.0,
			new lambertian(
				color{0.4, 0.5, 0.9},
				1.0
			)
		)
	);
	_world.append(new sphere(
			vec3{0.0, 1.0, 0.0},
			1.0,
			new dielectric(
				color(1.0),
				1.0,
				1.5
			)
		)
	);
	_world.append(new sphere(
			vec3{0.0, 1.0, 0.0},
			0.5,
			new dielectric(
				color(1.0),
				1.0,
				1.5
			)
		)
	);
	_world.append(new sphere(
			vec3{4.0, 1.0, 0.0},
			1.0,
			new metal(
				color{0.9, 0.6, 0.5},
				1.0,
				0.0
			)
		)
	);

	PPM _image("image.ppm", vec2i{IMAGE_WIDTH, IMAGE_HEIGHT});

	cameraConfig _config;
	//_config.d_position = vec3{0.0, 1.0, 3.0};
	_config.d_position = vec3{15.0, 2.0, 4.0};
	_config.d_target = vec3{0.0, 1.0, 0.0};
	_config.d_upVector = vec3{0.0, 1.0, 0.0};
	_config.d_focusDistance = 10.0;
	_config.d_defocusAngle = constant::PI / 18;
	_config.d_fov = FOV * 0.0174532925199;
	renderer _renderer(&_image, &_world, _config);

	int sample = 50;
	int bounce = 10;
			
	unsigned long long ray_count = IMAGE_WIDTH * IMAGE_HEIGHT * sample * bounce;
	double est_time = ray_count * 9.32032e-05;
	std::clog << "Est Time (ms) " << est_time << " @ " << ray_count/1000000.0 << "_m rays" << std::endl;
	double milliseconds;
	{
		scopeTimer mt_timer("MT Render", std::clog);
		_renderer.renderImageMT(sample, bounce, vec2i{4, 1}, 0); //scan lines appear to have an edge perhaps with cache locality
		milliseconds = mt_timer.milliseconds();
	}
	std::clog << "Rendered " << ray_count/1000000.0 << "_m Rays @ " << milliseconds / ray_count << "ms/ray\n" << std::endl;
	return 0;
}