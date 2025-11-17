#pragma once

class interval {
public:
	double m_min, m_max;
	interval();
	interval(double, double);
	double size() const;
	bool contains(double) const;
	bool surrounds(double) const;
	static const interval empty, universe, forward;
};