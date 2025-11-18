#pragma once
#include <fstream>
#include "color.hpp"
class PPM {
public:
	PPM();
	PPM(const vec2i &p_dimension, const char *p_ostream);
	~PPM();
	color pixel(const vec2i &p_pixel) const;
	void draw(const vec2i &p_pixel, const color &p_color);
	void writeTestImage();
	void writeImage();

private:
	vec2i m_dimension;
	color *m_image = nullptr; //contigous image data like gpu buffer use case
	std::fstream m_fstream;
};