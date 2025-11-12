#pragma once
class PPM {
public:
	PPM();
	PPM(int, int);

	void WriteTestImage();

private:
	int m_width, m_height;
};