#pragma once

#include <Arduino.h>

namespace Orientation 
{
	namespace Pins 
	{
		const int SCL = A5;
		const int SDA = A4;
		const int INT = 2;
	}

	namespace Axis 
	{
		const int YAW = 0;
	  	const int PITCH = 2;
		const int ROLL = 1;
	}

	void initialize();

	void update();

	void calibrate();

	float get_axis_degrees(int axis);
}