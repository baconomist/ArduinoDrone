#include "motors.h"

#include <Arduino.h>
#include <Servo.h>
#include "utils.h"

/**
(Betaflight defaults)
O 4     O 2
 \  ^  /           ^ +pitch
  \ ^ /	           > +roll
  -----            
  /   \            
 /     \           
O 3     O 1
**/

float current_throttle = 0;
float current_roll = 0;
float current_pitch = 0;
float current_yaw = 0;


namespace Motors
{
	Servo ESC1;
	Servo ESC2;
	Servo ESC3;
	Servo ESC4;

	void initialize()
	{
		Serial.println("Initializing ESCs...");
		ESC1.attach(Pins::ESC_1);
		ESC2.attach(Pins::ESC_2);
		ESC3.attach(Pins::ESC_3);
		ESC4.attach(Pins::ESC_4);

		_calibrate_escs();
	}

	void _update_motors() 
	{
		float k = 0.75f;

		float pitch_forward = clamp(1 / 2 * -current_pitch, -current_throttle * k, current_throttle * k);
		float pitch_back = -pitch_forward;

		float roll_right = clamp(1 / 2 * -current_roll, -current_throttle * k, current_throttle * k);
		float roll_left = -roll_right;

		float yaw_cw = clamp(1 / 2 * current_yaw, -current_throttle * k, current_throttle * k);
		float yaw_ccw = -yaw_cw;

		ESC1.writeMicroseconds(map(current_throttle + pitch_back + roll_right + yaw_cw, 0, 100, THR_MIN, THR_MAX));
		ESC2.writeMicroseconds(map(current_throttle + pitch_forward + roll_right + yaw_ccw, 0, 100, THR_MIN, THR_MAX));
		ESC3.writeMicroseconds(map(current_throttle + pitch_back + roll_left + yaw_ccw, 0, 100, THR_MIN, THR_MAX));
		ESC4.writeMicroseconds(map(current_throttle + pitch_forward + roll_left + yaw_cw, 0, 100, THR_MIN, THR_MAX));
	}

	void set_throttle_percent(float percent) 
	{
		current_throttle = clamp(percent, 0, 100);

		_update_motors();
	}

	void set_roll_percent(float percent) 
	{
		current_roll = clamp(percent, -100, 100);

		_update_motors();
	}

	void set_pitch_percent(float percent) 
	{
		current_pitch = clamp(percent, -100, 100);

		_update_motors();
	}

	void set_yaw_percent(float percent) 
	{
		current_yaw = clamp(percent, -100, 100);

		_update_motors();
	}

	void disarm() 
	{
		ESC1.writeMicroseconds(1000);
		ESC2.writeMicroseconds(1000);
		ESC3.writeMicroseconds(1000);
		ESC4.writeMicroseconds(1000);
	}

	void arm() 
	{
		_calibrate_escs();
	}

	void _calibrate_escs() 
	{
		Serial.println("Running ESC Calibration...");
		int startTime = millis();
		while (millis() < startTime + ESC_CALIB_TIME) {
			ESC1.writeMicroseconds(1000);
			ESC2.writeMicroseconds(1000);
			ESC3.writeMicroseconds(1000);
			ESC4.writeMicroseconds(1000);
		}
		Serial.println("ESC Calibration Done.");
	}
}
