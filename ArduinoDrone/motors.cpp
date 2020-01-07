#include "motors.h"

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

namespace Motors 
{
	Servo ESC1;
	Servo ESC2;
	Servo ESC3;
	Servo ESC4;

	void initialize() 
	{
		ESC1.attach(Pins::ESC_1);
		ESC2.attach(Pins::ESC_2);
		ESC3.attach(Pins::ESC_3);
		ESC4.attach(Pins::ESC_4);

		int startTime = millis();
		while (millis() < startTime + ESC_CALIB_TIME) {
			ESC1.writeMicroseconds(1000);
			ESC2.writeMicroseconds(1000);
			ESC3.writeMicroseconds(1000);
			ESC4.writeMicroseconds(1000);
		}
	}

	void update() 
	{
		ESC1.writeMicroseconds(map(current_throttle * (current_pitch - current_roll) / 100.0f , 0, 100, THR_MIN, THR_MAX));
		ESC2.writeMicroseconds(map(current_throttle * (current_pitch + current_roll) / 100.0f, 0, 100, THR_MIN, THR_MAX));
		ESC3.writeMicroseconds(map(current_throttle, 0, 100, THR_MIN, THR_MAX));
		ESC4.writeMicroseconds(map(current_throttle, 0, 100, THR_MIN, THR_MAX));
	}

	void set_throttle_percent(float percent) 
	{
		current_throttle = clamp(percent, 0, 100);
	}

	void set_roll_percent(float percent) 
	{
		current_roll = clamp(percent, -100, 100);
	}

	void set_pitch_percent(float percent) 
	{
		current_pitch = clamp(percent, -100, 100);
	}

	void set_yaw_percent(float percent) 
	{
		current_yaw = clamp(percent, -100, 100);
	};
}

