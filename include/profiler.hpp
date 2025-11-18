#pragma once
#include <chrono>
#include <iostream>
class timer {
public:
	virtual ~timer() = default;
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual void reset() = 0;
	virtual bool running() const = 0;
	virtual double seconds() const = 0;
	virtual double milliseconds() const = 0;
	virtual double microseconds() const = 0;
};

class steadyTimer : public timer {
public:
	steadyTimer();

	void start() override;
	void stop() override;
	void reset() override;

	bool running() const override;

	double seconds() const override;
	double milliseconds() const override;
	double microseconds() const override;
protected:
	std::chrono::steady_clock::time_point end() const;

	std::chrono::steady_clock::time_point m_start, m_stop;
	bool m_running;
};

class scopeTimer : public steadyTimer {
public:
	explicit scopeTimer();
	explicit scopeTimer(const char* p_label, std::ostream &p_ostream = std::clog);
	~scopeTimer() override;

private:
	const char* m_label;
	std::ostream& m_ostream;
};