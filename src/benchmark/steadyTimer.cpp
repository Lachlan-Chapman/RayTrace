#include "profiler.hpp"

steadyTimer::steadyTimer() : m_running(false) {}
void steadyTimer::start() {
	m_running = true;
	m_start = std::chrono::steady_clock::now();
}

void steadyTimer::stop() {
	m_running = false;
	m_stop = std::chrono::steady_clock::now();
}

void steadyTimer::reset() {
	m_running = false;
	m_start = {};
	m_stop = {};
}

bool steadyTimer::running() const {return m_running;}

std::chrono::steady_clock::time_point steadyTimer::end() const {return m_running ? std::chrono::steady_clock::now() : m_stop;} //return stop point either the stopped time or the time rn to allow multiple checkpoints throughout a test

double steadyTimer::seconds() const {
	return std::chrono::duration<double>(end() - m_start).count();
}

double steadyTimer::milliseconds() const {
	return std::chrono::duration<double, std::milli>(end() - m_start).count();
}

double steadyTimer::microseconds() const {
	return std::chrono::duration<double, std::micro>(end() - m_start).count();
}