#include <iostream>
#include <fstream>

#include "PPM.hpp"
#include "vec.hpp"
#include "color.hpp"
#include "camera.hpp"

#include "sphere.hpp"

#include "material.hpp"

#define IMAGE_WIDTH 200
#define IMAGE_HEIGHT 100


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
	camera cam(
		{IMAGE_WIDTH, IMAGE_HEIGHT},
		1.0,
		1,
		10
	);

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
	// cam.m_world.append(new sphere( //left inner
	// 	{-1.0, 0.0, -1.0},
	// 	0.25,
	// 	new dielectric(
	// 		color(1.0),
	// 		1.0,
	// 		1.0 / 1.5
	// 	)
	// ));
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
	cam.render();
}

int main(int argc, char** argv) {
	testCamera();
	return 0;
}

