#pragma once

class interval {
public:
	double m_min, m_max;
	interval();
	interval(double p_min, double p_max);
	double length() const;
	bool contains(double p_time) const;
	bool surrounds(double p_time) const;
	static const interval empty, universe, forward;
};