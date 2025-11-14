#include <iostream>
#include <fstream>

#include "PPM.hpp"
#include "vec.hpp"
#include "color.hpp"
#include "camera.hpp"
#include "sphere.hpp"


#define IMAGE_WIDTH 128
#define IMAGE_HEIGHT 128


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
		2
	);
	cam.m_world.append(new sphere(
		{0.0, 0.0, -1.0},
		0.5
	));
	cam.render();
}

int main(int argc, char** argv) {
	testCamera();
	return 0;
}

