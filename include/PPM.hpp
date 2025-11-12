#pragma once
#include <fstream>
#include "color.hpp"
class PPM {
public:
	PPM();
	PPM(vec2i, const char*);
	~PPM();
	color pixel(vec2i) const;
	void draw(vec2i, color);
	void writeTestImage();
	void writeImage();

private:
	vec2i m_dimension;
	color *m_image = nullptr; //contigous image data like gpu buffer use case
	std::fstream m_fstream;
};