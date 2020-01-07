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
		return map(pulseIn(axis_pin, HIGH), 984, 2003, 1000, 2000);
	}
}
