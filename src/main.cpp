#include <iostream>
#include <fstream>
#include <chrono>

#include "constant.hpp"
#include "random.hpp"

#include "hittable.hpp"
#include "sceneObject.hpp"
#include "material.hpp"
#include "world.hpp"

#include "PPM.hpp"
#include "camera.hpp"
#include "renderer.hpp"

#include "tile.hpp"

#include "profiler.hpp"

#include "BVHTechniques.hpp"

#define IMAGE_WIDTH 100
#define IMAGE_HEIGHT 100
#define FOV 20

#ifndef GIT_HASH
#define GIT_HASH "default"
#endif

//tracer validation
void testRender(const char* p_testName, const world &p_world) {
	cameraConfig _config;
	_config.d_position = vec3f{6.0, 0.0, 4.0};
	_config.d_target = vec3f{0.0, 0.0, 0.0};
	_config.d_upVector = vec3f{0.0, 1.0, 0.0};
	_config.d_focusDistance = 10.0;
	_config.d_defocusAngle = constant::PI / 18;
	_config.d_fov = 90.0 * 0.0174532925199; //const from deg to radians

	PPM _image(p_testName, vec2i{150, 150});
	renderer _renderer(&_image, &p_world, _config);
	int sample = 1;
	int bounce = 10;
	_renderer.renderImageMT(sample, bounce, vec2i{4, 1}); //scan lines appear to have an edge perhaps with cache locality
	_renderer.saveImage();
}

void testBVH() {
	world _world(2, BVHTechnique::median, 2);
	_world.append(new rectangle(
		position(0, 2, 0),
		size(1.0),
		new lambertian(
			color(0.7),
			1.0
		)
	));

	_world.append(new rectangle(
		position(0, 0, 0),
		size(1.0),
		new lambertian(
			color(0.7),
			1.0
		)
	));

	_world.buildBVH();
	// hitRecord _record;
	// _world.intersect(
	// 	ray(
	// 		position(0, 0, 4),
	// 		vec3f(0, 0, -1),
	// 		interval::forward
	// 	),
	// 	interval::forward,
	// 	_record
	// );

	testRender("BVH.ppm", _world);
}

void testSphere() {
	world _world(4, BVHTechnique::median, 2);
	_world.append(new sphere(
		position(0, -5, 0),
		size(1.0),
		new lambertian(
			color(0.9, 0.8, 0.7),
			1.0
		)
	));

	_world.append(new sphere(
		position(0, -2, 0),
		size(1.0),
		new metal(
			color(0.9, 0.8, 0.7),
			1.0,
			1.0
		)
	));

	_world.append(new sphere(
		position(0, 1, 0),
		size(1.0),
		new dielectric(
			color(0.9, 0.8, 0.7),
			1.0,
			1.5
		)
	));

	_world.append(new sphere(
		position(0, 4, 0),
		size(1.0),
		new passthrough()
	));
	_world.buildBVH();
	testRender("testSphere.ppm", _world);
}

void testRectangle() {
	world _world(4, BVHTechnique::median, 2);
	_world.append(new rectangle(
		position(0, -5, 0),
		size(1.0),
		new lambertian(
			color(0.9, 0.8, 0.7),
			1.0
		)
	));

	_world.append(new rectangle(
		position(0, -2, 0),
		size(1.0),
		new metal(
			color(0.9, 0.8, 0.7),
			1.0,
			1.0
		)
	));

	_world.append(new rectangle(
		position(0, 1, 0),
		size(1.0),
		new dielectric(
			color(0.9, 0.8, 0.7),
			1.0,
			1.5
		)
	));

	_world.append(new rectangle(
		position(0, 4, 0),
		size(1.0),
		new passthrough()
	));
	_world.buildBVH();
	testRender("testRectangle.ppm", _world);
}

//benchmark
void generateWorld(world &p_world) {
	for(int i = 0; i < world::MAX_OBJECTS-5; i++) {
		material *_mat;
		double mat_selection = rng::decimal();
		
		color _col = (rng::vector() + color(0.2)).unit(); //brighten it slightly
		vec3f _pos = vec3f{rng::decimal(-25.0, 14.0), 0.0, rng::decimal(-13, 7)} * rng::vector();
		
		double _rad = 0.2;
		_pos[1] = _rad;

		if(mat_selection < 0.5) {
			_mat = new metal( //mistake, should be lambertian but the benchmarking settings are now set so. also metal uses the lambertian material so speed ups to it would affect metals also
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
	
	
	p_world.append(new sphere( //ground
			position{0.0, -1000.0, 0.0},
			size(1000.0),
			new lambertian(
				color(0.5),
				reflectance(1.0)
			)
		)
	);
	p_world.append(new sphere( //simple diffuse
			position{-4.0, 1.0, 0.0},
			size(1.0),
			new lambertian(
				color{0.4, 0.5, 0.9},
				reflectance(1.0)
			)
		)
	);
	p_world.append(new sphere(
			position{0.0, 1.0, 0.0},
			size(1.0),
			new dielectric(
				color(1.0),
				reflectance(1.0),
				ior(1.5)
			)
		)
	);
	p_world.append(new sphere(
			position{0.0, 1.0, 0.0},
			size(0.5),
			new dielectric(
				color(1.0),
				reflectance(1.0),
				ior(1.5)
			)
		)
	);
	p_world.append(new sphere(
			position{4.0, 1.0, 0.0},
			size(1.0),
			new metal(
				color{0.9, 0.6, 0.5},
				reflectance(1.0),
				roughness(0.0)
			)
		)
	);
	p_world.buildBVH();
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

void generateBenchmarkWorld(world &p_world) {
	for(int i = 0; i < world::MAX_OBJECTS-5; i++) {
		material *_mat;
		double mat_selection = rng::decimal();
		
		color _col = (rng::vector() + color(0.2)).unit(); //brighten it slightly
		vec3f _pos = vec3f{rng::decimal(-25.0, 14.0), 0.0, rng::decimal(-13, 7)} * rng::vector();
		
		double _rad = 0.2;
		_pos[1] = _rad;

		if(mat_selection < 0.5) {
			_mat = new metal( //mistake, should be lambertian but the benchmarking settings are now set so. also metal uses the lambertian material so speed ups to it would affect metals also
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
	
	
	p_world.append(new sphere( //ground
			position{0.0, -1000.0, 0.0},
			size(1000.0),
			new lambertian(
				color(0.5),
				reflectance(1.0)
			)
		)
	);
	p_world.append(new sphere( //simple diffuse
			position{-4.0, 1.0, 0.0},
			size(1.0),
			new lambertian(
				color{0.4, 0.5, 0.9},
				reflectance(1.0)
			)
		)
	);
	p_world.append(new sphere(
			position{0.0, 1.0, 0.0},
			size(1.0),
			new dielectric(
				color(1.0),
				reflectance(1.0),
				ior(1.5)
			)
		)
	);
	p_world.append(new sphere(
			position{0.0, 1.0, 0.0},
			size(0.5),
			new dielectric(
				color(1.0),
				reflectance(1.0),
				ior(1.5)
			)
		)
	);
	p_world.append(new sphere(
			position{4.0, 1.0, 0.0},
			size(1.0),
			new metal(
				color{0.9, 0.6, 0.5},
				reflectance(1.0),
				roughness(0.0)
			)
		)
	);
	p_world.buildBVH();
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
		//_renderer.renderImage(sample, bounce, vec2i{4, 1}); //scan lines appear to have an edge perhaps with cache locality		
		mt_time = mt_timer.milliseconds();
	}
	std::clog << GIT_HASH << " " << ray_count << " Rays @ " << mt_time << "(" << (mt_time/ray_count) << " ms/ray" << ")" << std::endl;
	_renderer.saveImage();
}

void compareIntersectionCode(const world &p_world) {
	vec3f origin(15, 2, 4); //copying a ray from the camera center directly at the target
	vec3f direction((vec3f(0, 1, 0) - origin).unit());
	ray common_ray = ray(
		origin,
		direction,
		interval::forward
	);

	hitRecord temp_record;
	int run_count = 100000; //remove timer noise and os unpredictability
	volatile float sink;

	steadyTimer timer;
	{
		timer.start();
		for(int run = 0; run < run_count; run++) {
			ray _ray(common_ray);
			interval _interval(common_ray.m_interval);
			p_world.intersect(_ray, _interval, temp_record);
			sink += temp_record.m_time;
		}
		timer.stop();
	}

	std::clog << "BVH Time (ms): " << timer.milliseconds() << std::endl;
	timer.reset();
	
	{
		timer.start();
		for(int run = 0; run < run_count; run++) {
			ray _ray(common_ray);
			interval _interval(common_ray.m_interval);
			p_world.intersectAll(_ray, _interval, temp_record);
			sink += temp_record.m_time;
		}
		timer.stop();
	}
	std::clog << "Loop Time (ms): " << timer.milliseconds() << std::endl;
	std::clog << "ignore" << sink << std::endl;
}

void fullRender(const world &p_world) {
	cameraConfig _config;
	_config.d_position = vec3f{15.0, 2.0, 4.0};
	_config.d_target = vec3f{0.0, 1.0, 0.0};
	_config.d_upVector = vec3f{0.0, 1.0, 0.0};
	_config.d_focusDistance = 10.0;
	_config.d_defocusAngle = constant::PI / 18;
	_config.d_fov = FOV * 0.0174532925199;
	
	vec2i resolution(3840, 2160);
	int samples = 40;
	int bounces = 12;

	PPM _image("4K_Render.ppm", resolution);
	renderer _renderer(&_image, &p_world, _config);
	int ray_count = resolution.x * resolution.y * samples * bounces;

	std::clog << "Rendering In 4K" << std::endl;

	steadyTimer timer;
	timer.start();
	{
		scopeTimer timer("4K Render", std::clog);
		_renderer.renderImageMT(samples, bounces, vec2i{4, 1}, 0);
	}
	timer.stop();
	float mt_time = timer.milliseconds();
	std::clog << GIT_HASH << " " << ray_count << " Rays @ " << mt_time << "(" << (mt_time/ray_count) << " ms/ray" << ")" << std::endl;
	_renderer.saveImage();
}

int main(int argc, char** argv) {
	//testBVH();
	//testSphere();
	//testRectangle();
	//return 1;

	
	//compareIntersectionCode(_world);
	//return 2;
	
	world benchy_world(128, BVHTechnique::median, 2);
	generateBenchmarkWorld(benchy_world);
	for(int test_id = 0; test_id < 5; test_id++) {
		std::clog << "Test " << test_id << std::endl;
		benchmarkVector();
		benchmarkRender(benchy_world);
	}
	
	//world _world(world::MAX_OBJECTS, BVHTechnique::median, 2);
	//generateWorld(_world);
	//fullRender(_world);


	return 0;
}