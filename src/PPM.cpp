#include <iostream>
#include <fstream>
#include "PPM.hpp"
#include "color.hpp"

PPM::PPM() : PPM({2, 2}, "image.ppm") {} //delegate constructor
PPM::PPM(const vec2i &p_dimension, const char *p_ostream) : m_dimension(p_dimension) {
	m_fstream.open(p_ostream, std::ios::out | std::ios::trunc);
	if(!m_fstream.is_open()) {
		std::cerr << "Failed to open " << p_ostream << std::endl;
	}
	m_image = new color[p_dimension[0] * p_dimension[1]];
}

PPM::~PPM() {
	if(m_fstream.is_open()) {
		m_fstream.close();
	}
	if(m_image) {
		delete m_image;
		m_image = nullptr;
	}
}

color PPM::pixel(const vec2i &p_pixel) const {
	return m_image[p_pixel[0] * m_dimension[0] + p_pixel[1]]; //returns the current color of the pixel
}

void PPM::draw(const vec2i &p_pixel, const color &p_color) {
	m_image[p_pixel[1] * m_dimension[0] + p_pixel[0]] = p_color; //writes the incoming color to that pixel data
}

void PPM::writeTestImage() {
	m_fstream << "P3\n" << m_dimension[0] << ' ' << m_dimension[1] << "\n255\n";

	int image_dim = m_dimension[0] * m_dimension[1];
	double steps_prog = 100.0 / image_dim; //percentage per pixel
	double progress = 0.0; //total progress
	for(int j = 0; j < m_dimension[1]; j++) {
		for(int i = 0; i < m_dimension[0]; i++) {
			double r = double(i) / (m_dimension[0]-1);
			double g = double(j) / (m_dimension[1]-1);
			color pixel = {r, g, 0};
			m_fstream << pixel;

			progress += steps_prog;
			std::clog << "Progress " << progress << '%' << std::endl;
		}
	}
}

void PPM::writeImage() {
	m_fstream << "P3\n" << m_dimension[0] << ' ' << m_dimension[1] << "\n255\n";
	int image_dim = m_dimension[0] * m_dimension[1];
	double steps_prog = 100.0 / image_dim; //percentage per pixel
	double progress = 0.0; //total progress
	for(int row = 0; row < m_dimension[1]; row++) {
		for(int col = 0; col < m_dimension[0]; col++) {
			m_fstream << pixel({row, col});
			progress += steps_prog;
			//std::clog << "Progress " << progress << '%' << std::endl;
		}
	}
	std::clog << "Done Rendering " << m_dimension[0] << 'x' << m_dimension[1] << " Pixel(s)" << std::endl;
}