#include <iostream>
#include <fstream>
#include "PPM.hpp"

PPM::PPM() : PPM(2, 2, "image.ppm") {} //delegate constructor
PPM::PPM(int p_width, int p_height, const char *p_ostream) : m_width(p_width), m_height(p_height) {
	m_fstream.open(p_ostream);
	if(!m_fstream.is_open()) {
		std::cerr << "Failed to open " << p_ostream << std::endl;
	}
}

PPM::~PPM() {
	if(m_fstream.is_open()) {
		m_fstream.close();
	}
}

void PPM::WriteTestImage() {
	m_fstream << "P3\n" << m_width << ' ' << m_height << "\n255\n";

	int image_dim = m_width * m_height;
	double steps_prog = 100.0 / image_dim; //percentage per pixel
	double progress = 0.0; //total progress
	for(int j = 0; j < m_height; j++) {
		for(int i = 0; i < m_width; i++) {
			double r = double(i) / (m_width-1);
			double g = double(j) / (m_height-1);
			double b = 0;

			int ir = int(255.999 * r);
			int ig = int(255.999 * g);
			int ib = int(255.999 * b);

			m_fstream << ir << ' ' << ig << ' ' << ib << '\n';

			progress += steps_prog;
			std::clog << "Progress " << progress << '%' << std::endl;
		}
	}
}