#include <iostream>
#include "PPM.hpp"

PPM::PPM() : PPM(100, 100) {} //delegate constructor
PPM::PPM(int p_width, int p_height) : m_width(p_width), m_height(p_height) {}

void PPM::WriteTestImage() {
	std::cout << "P3\n" << m_width << ' ' << m_height << "\n255\n";

	int image_dim = m_width * m_height;
	double steps_prog = 1 / image_dim; //percentage per pixel
	double progress; //total progress
	for(int j = 0; j < m_height; j++) {
		for(int i = 0; i < m_width; i++) {
			double r = double(i) / (m_width-1);
			double g = double(j) / (m_height-1);
			double b = 0;

			int ir = int(255.999 * r);
			int ig = int(255.999 * g);
			int ib = int(255.999 * b);

			std::cout << ir << ' ' << ig << ' ' << ib << '\n';

			progress += steps_prog;
			std::clog << "Progress " << progress << '%' << std::endl;
		}
	}
}