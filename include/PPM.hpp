#pragma once
#include <fstream>
class PPM {
public:
	PPM();
	PPM(int, int, const char*);
	~PPM();
	void WriteTestImage();

private:
	int m_width, m_height;
	std::fstream m_fstream;
};