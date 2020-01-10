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
		const int CH_5 = 3;
	}

	const int RC_MIN = 984;
	const int RC_MAX = 2003;
	const int NORMALIZED_RC_MIN = 1000;
	const int NORMALIZED_RC_MAX = 2000;

	void initialize();

	float get_axis(int axis_pin);
	float get_axis_percent(int pin);

	bool rc_connected();
}