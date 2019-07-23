#pragma once

namespace oyl {

class Timestep {
public:
	Timestep(float time = 0.0f)
		: m_time(time) {}

	float getMillis() const { return m_time; }
	float getSeconds() const { return m_time / 1000.0f; }

	// Returns the timestep in milliseconds
	operator float() const { return m_time; }

	// Assumes that the timestep is in milliseconds
	friend static float operator *(Timestep ts, float f);
	friend static float operator *(float f, Timestep ts);
private:
	float m_time;
};

static float operator *(Timestep ts, float f) {
	return f * ts.getMillis();
}
static float operator *(float f, Timestep ts) {
	return f * ts.getMillis();
}
}