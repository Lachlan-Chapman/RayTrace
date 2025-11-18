#pragma once
#include <fstream>
#include "color.hpp"
class PPM {
public:
	PPM();
	PPM(const char *p_ostream, const vec2i &p_resolution);
	~PPM();
	color pixel(const vec2i &p_pixel) const;
	void draw(const vec2i &p_pixel, const color &p_color);
	void writeTestImage();
	void writeImage();
	vec2i m_resolution;
private:
	color *m_image = nullptr; //contigous image data like gpu buffer use case
	std::fstream m_fstream;
};