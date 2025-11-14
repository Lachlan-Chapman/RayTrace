#include <iostream>
#include <fstream>

#include "PPM.hpp"
#include "vec.hpp"
#include "color.hpp"
#include "camera.hpp"

#include "sphere.hpp"

#include "lambertian.hpp"

#define IMAGE_WIDTH 1000
#define IMAGE_HEIGHT 1000


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
		3,
		2
	);
	cam.m_world.append(new sphere(
		{0.0, 0.5, -1.0},
		0.5,
		new lambertian( //white test
			vec3{1.0, 0.5, 0.3},
			0.7
		)
	));
	cam.m_world.append(new sphere(
		{0.0, -0.5, -1.0},
		0.5,
		new lambertian( //white test
			vec3{0.3, 0.5, 1.0},
			0.7
		)
	));
	cam.render();
}

int main(int argc, char** argv) {
	testCamera();
	return 0;
}

