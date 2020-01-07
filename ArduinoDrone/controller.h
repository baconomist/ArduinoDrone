#pragma once

#include <Arduino.h>

// A0 -> Roll
// A1 -> Pitch
// A2 -> Throttle
// A3 -> Yaw

namespace Controller
{
	namespace Pins
	{
		const int ROLL = A0;
		const int PITCH = A1;
		const int THROTTLE = A2;
		const int YAW = A3;
	}

	void initialize();

	float get_axis(int axis_pin);
}