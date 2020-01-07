#include "controller.h"


namespace Controller 
{
	void initialize()
	{
		pinMode(Pins::ROLL, INPUT);
		pinMode(Pins::PITCH, INPUT);
		pinMode(Pins::THROTTLE, INPUT);
		pinMode(Pins::YAW, INPUT);
	}

	// @warning may take a long time if there is no RC connected to the drone
	float get_axis(int axis_pin)
	{
		return map(pulseIn(axis_pin, HIGH), RC_MIN, RC_MAX, NORMALIZED_RC_MIN, NORMALIZED_RC_MAX);
	}

	// @warning may take a long time if there is no RC connected to the drone
	float get_axis_percent(int axis_pin) 
	{
		if(axis_pin == Pins::THROTTLE)
			return map(get_axis(axis_pin), NORMALIZED_RC_MIN, NORMALIZED_RC_MAX, 0, 100);
		else
			return map(get_axis(axis_pin), NORMALIZED_RC_MIN, NORMALIZED_RC_MAX, -100, 100);
	}

}
