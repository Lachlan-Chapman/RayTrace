#include <iostream>
#include <fstream>
#include "PPM.hpp"
#include "color.hpp"

PPM::PPM() : PPM( "image.ppm", vec2i{2, 2}) {} //delegate constructor
PPM::PPM(const char *p_ostream, const vec2i &p_resolution) : m_resolution(p_resolution) {
	m_fstream.open(p_ostream, std::ios::out | std::ios::trunc);
	if(!m_fstream.is_open()) {
		std::cerr << "Failed to open " << p_ostream << std::endl;
	}
	m_image = new color[p_resolution[0] * p_resolution[1]];
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
	return m_image[p_pixel[0] * m_resolution[0] + p_pixel[1]]; //returns the current color of the pixel
}

void PPM::draw(const vec2i &p_pixel, const color &p_color) {
	m_image[p_pixel[1] * m_resolution[0] + p_pixel[0]] = p_color; //writes the incoming color to that pixel data
}

void PPM::writeTestImage() {
	m_fstream << "P3\n" << m_resolution[0] << ' ' << m_resolution[1] << "\n255\n";

	int image_dim = m_resolution[0] * m_resolution[1];
	double steps_prog = 100.0 / image_dim; //percentage per pixel
	double progress = 0.0; //total progress
	for(int j = 0; j < m_resolution[1]; j++) {
		for(int i = 0; i < m_resolution[0]; i++) {
			double r = double(i) / (m_resolution[0]-1);
			double g = double(j) / (m_resolution[1]-1);
			color pixel = {r, g, 0};
			m_fstream << pixel;

			progress += steps_prog;
			std::clog << "Progress " << progress << '%' << std::endl;
		}
	}
}

vec3 PPM::toGamma(const vec3 &p_color, double p_gamma) const {
	vec3 gamma_converted;
	for(int i = 0; i < 3; i++) {
		gamma_converted[i] = std::pow(p_color[i], 1 / p_gamma);
	}
	return gamma_converted;
}
vec3 PPM::toSRGB(const vec3 &p_color) const {
	vec3 srgb_converted;
	for(int i = 0; i < 3; i++) {
		if(p_color[i] <= 0.0031308) {
			srgb_converted[i] = 12.92 * p_color[i];
		} else {
			srgb_converted[i] = 1.055 * std::pow(p_color[i], 1.0/2.4) - 0.055;
		}
	}
	return srgb_converted;
}

void PPM::writeImage() {
	m_fstream << "P3\n" << m_resolution[0] << ' ' << m_resolution[1] << "\n255\n";
	for(int row = 0; row < m_resolution[1]; row++) {
		for(int col = 0; col < m_resolution[0]; col++) {
			m_fstream << pixel({row, col});
		}
	}
	std::clog << "Done Rendering " << m_resolution[0] << 'x' << m_resolution[1] << " Pixel(s)" << std::endl;
}