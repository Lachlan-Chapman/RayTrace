#pragma once

class interval {
public:
	float m_min, m_max;
	interval();
	interval(float p_min, float p_max);
	interval(const interval &p_other);
	float length() const;
	bool contains(float p_time) const;
	bool surrounds(float p_time) const;
	static const interval empty, universe, forward;
};