#include "profiler.hpp"
scopeTimer::scopeTimer() : scopeTimer("Scope Timer", std::clog) {}
scopeTimer::scopeTimer(const char* p_label, std::ostream &p_ostream) : m_label(p_label), m_ostream(p_ostream) {
	start();
}

scopeTimer::~scopeTimer() {
	stop();
	m_ostream << m_label << ": " << milliseconds() << " ms\n";
}