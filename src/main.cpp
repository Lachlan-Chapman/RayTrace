#include <iostream>
#include "PPM.hpp"

int main(int argc, char** argv) {
	PPM ppm(10, 10);
	ppm.WriteTestImage();
	return 0;
}