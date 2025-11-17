#include <iostream>
#include <fstream>
#include <chrono>

#include "PPM.hpp"
#include "vec.hpp"
#include "color.hpp"
#include "camera.hpp"

#include "sphere.hpp"

#include "material.hpp"

#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 360
#define FOV 20.0


void testPPM() {
	PPM ppm; //default
	//PPM ppm(10, 10, "test.ppm")
	ppm.writeTestImage();
}

void testVec() {
	vec<3, int> veci; //default
	vec<3, double> vecd = {0.1, 0.2, 0.3}; //init list creation

	std::cout << "int vector: " << veci << '\n' << "double vec: " << vecd << std::endl;
}

void testColor() {
	color rainbow = {0.5, 0.5, 0.5};
	std::cout << "Test Color: " << static_cast<vec<3, double>>(rainbow) << " becomes: " << rainbow << std::endl;
}

void testCamera() {
	camera cam(32);
	cam.setCameraMeta(vec2{IMAGE_WIDTH, IMAGE_HEIGHT}, FOV, 0.8, 3.4);
	cam.setPosition(vec3{
		-2.0, 2.0, 1.0
	}, vec3{
		0.0, 0.0, -1.0
	});
	cam.setRayTraceMeta(5, 4);

	cam.m_world.append(new sphere( //"earth"
		{0.0, -100.5, -1.0},
		100,
		new lambertian(
			color{0.8, 0.8, 0.0},
			1.0
		)
	));
	cam.m_world.append(new sphere( //left
		{-1.0, 0.0, -1.0},
		0.5,
		new dielectric(
			color(1.0),
			1.0,
			1.5
		)
	));
	cam.m_world.append(new sphere( //left inner
		{-1.0, 0.0, -1.0},
		0.25,
		new dielectric(
			color(1.0),
			1.0,
			1.0 / 1.5
		)
	));
	cam.m_world.append(new sphere( //center
		{0.0, 0.0, -1.0},
		0.5,
		new lambertian(
			color{0.1, 0.2, 0.5},
			1.0
		)
	));
	cam.m_world.append(new sphere( //right
		{1.0, 0.0, -1.0},
		0.5,
		new metal(
			color{0.8, 0.6, 0.2},
			1.0,
			1.0
		)
	));
	std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
	cam.render();
	std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
	std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	std::cout << "Render completed in " << duration.count() << " ms (" << static_cast<double>(duration.count()) / 1000.0 << " seconds)" << std::endl;
}

int main(int argc, char** argv) {
	testCamera();
	return 0;
}

