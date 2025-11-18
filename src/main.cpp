#include <iostream>
#include <fstream>
#include <chrono>

#include "constant.hpp"

#include "hittable.hpp"
#include "material.hpp"
#include "world.hpp"

#include "PPM.hpp"
#include "camera.hpp"
#include "renderer.hpp"

#define IMAGE_WIDTH 100
#define IMAGE_HEIGHT 100
#define FOV 90




int main(int argc, char** argv) {
	world _world(32);
	_world.append(new sphere(
			vec3(0.0),
			0.5,
			new lambertian(
				color(1.0),
				1.0
			)
		)
	);

	PPM _image("image.ppm", vec2i{IMAGE_WIDTH, IMAGE_HEIGHT});

	cameraConfig _config;
	_config.d_position = vec3{-2.0, 2.0, 1.0};
	_config.d_target = vec3{0.0, 0.0, -1.0};
	_config.d_upVector = vec3{0.0, 1.0, 0.0};
	_config.d_focusDistance = 3.4;
	_config.d_defocusAngle = constant::PI / 18;
	_config.d_fov = constant::PI * 0.5;
	renderer _renderer(&_image, &_world, _config);

	_renderer.renderImage(1, 1);

	return 0;
}

