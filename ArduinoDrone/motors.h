#pragma once

// MIN = 1040  MAX = 2000

namespace Motors 
{
	namespace Pins 
	{
		const int ESC_1 = 11;
		const int ESC_2 = 10;
		const int ESC_3 = 9;
		const int ESC_4 = 6;
	}

	const int ESC_CALIB_TIME = 8000;
	const int THR_MIN = 1040;
	const int THR_MAX = 2000;

	float current_throttle = 0;
	float current_roll = 0;
	float current_pitch = 0;
	float current_yaw = 0;

	void initialize();

	void update();

	void set_throttle_percent(float percent);
	void set_roll_percent(float percent);
	void set_pitch_percent(float percent);
	void set_yaw_percent(float percent);
}