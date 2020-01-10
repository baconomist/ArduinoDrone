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

	void initialize();

	void set_throttle_percent(float percent);
	void set_roll_percent(float percent);
	void set_pitch_percent(float percent);
	void set_yaw_percent(float percent);

	void disarm();
	void arm();

	void _update_motors();
	void _calibrate_escs();
}