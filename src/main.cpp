#include <iostream>
#include <fstream>
#include <string>

#include "PPM.hpp"
#include "vec.hpp"
#include "color.hpp"

void testPPM() {
	PPM ppm; //default
	//PPM ppm(10, 10, "test.ppm")
	ppm.WriteTestImage();
}

void testVec() {
	vec<3, int> veci; //default
	vec<3, double> vecd = {0.1, 0.2, 0.3}; //init list creation

	std::cout << "int vector " << veci << '\n' << "double vec " << vecd << std::endl;
}

void testColor() {
	color rainbow = {0.5, 0.5, 0.5};
	std::cout << "Test Color " << static_cast<vec<3, double>>(rainbow) << " becomes " << rainbow << std::endl;
}

int main(int argc, char** argv) {

	testColor();
	return 0;
}

