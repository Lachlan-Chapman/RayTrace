#include "interval.hpp"
#include "constant.hpp"
interval::interval() : m_min(constant::INF), m_max(-constant::INF) {} //default ray length would be always invalid
interval::interval(double p_min, double p_max) : m_min(p_min), m_max(p_max) {}

double interval::size() const {return m_max - m_min;} //interval magnitude
bool interval::contains(double p_time) const {return m_min <= p_time && p_time <= m_max;} //closed [] type range check //using time here as the param to carry on that the interval will be used for evaluating ray lengths but this is a general interval class
bool interval::surrounds(double p_time) const {return m_min < p_time && p_time < m_max;} //open () type range check
const interval interval::empty = interval(constant::INF, -constant::INF); //all contains/sorrounds will fail
const interval interval::universe = interval(-constant::INF, constant::INF); //all contains/sorrounds will pass
const interval interval::forward = interval(0.001, constant::INF); //wont look in the oppisite dir for roots