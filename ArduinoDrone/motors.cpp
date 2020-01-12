#include "motors.h"

#include <Arduino.h>
#include <Servo.h>
#include "utils.h"
#include "orientation.h"
#include "motion_control.h"

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
	struct Mix
	{
		float pitch = 0.0f;
		float roll = 0.0f;
		float yaw = 0.0f;

		void set_mix(float pitch, float roll, float yaw) 
		{
			this->pitch = pitch;
			this->roll = roll;
			this->yaw = yaw;
		}

		float get_mixed()
		{
			return pitch + roll + yaw;
		}
	};

	Servo ESC1;
	Servo ESC2;
	Servo ESC3;
	Servo ESC4;

	Mix Motor1Mix;
	Mix Motor2Mix;
	Mix Motor3Mix;
	Mix Motor4Mix;

	float motor_1_thr = 0;
	float motor_2_thr = 0;
	float motor_3_thr = 0;
	float motor_4_thr = 0;

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
		float kP = 0.2f;
		float thr_kP = 0.25f;

		float yaw_cw = clamp(0.5f * current_yaw, -current_throttle * k, current_throttle * k);
		float yaw_ccw = -yaw_cw;

		float pitch_forward = clamp(0.5f * -current_pitch, -current_throttle * k, current_throttle * k);
		float pitch_back = -pitch_forward;

		float roll_right = clamp(0.5f * -current_roll, -current_throttle * k, current_throttle * k);
		float roll_left = -roll_right;

		float p_pitch = MotionControl::p_controller(Orientation::get_axis_degrees(Orientation::Axis::PITCH), 0, kP);
		float p_roll = MotionControl::p_controller(Orientation::get_axis_degrees(Orientation::Axis::ROLL), 0, kP);

		pitch_forward -= p_pitch;
		pitch_back += p_pitch;
		roll_right -= p_roll;
		roll_left += p_roll;

		Motor1Mix.set_mix(pitch_back, roll_right, yaw_cw);
		Motor2Mix.set_mix(pitch_forward, roll_right, yaw_ccw);
		Motor3Mix.set_mix(pitch_back, roll_left, yaw_ccw);
		Motor4Mix.set_mix(pitch_forward, roll_left, yaw_cw);
		
		float motor_1_thr_next = clamp(current_throttle + Motor1Mix.get_mixed(), 0, 100);
		float motor_2_thr_next = clamp(current_throttle + Motor2Mix.get_mixed(), 0, 100);
		float motor_3_thr_next = clamp(current_throttle + Motor3Mix.get_mixed(), 0, 100);
		float motor_4_thr_next = clamp(current_throttle + Motor4Mix.get_mixed(), 0, 100);

		// PID throttle smoothing; gets rid of jittery motor movements
		motor_1_thr += MotionControl::p_controller(motor_1_thr, motor_1_thr_next, thr_kP);
		motor_2_thr += MotionControl::p_controller(motor_2_thr, motor_2_thr_next, thr_kP);
		motor_3_thr += MotionControl::p_controller(motor_3_thr, motor_3_thr_next, thr_kP);
		motor_4_thr += MotionControl::p_controller(motor_4_thr, motor_4_thr_next, thr_kP);

		ESC1.writeMicroseconds(map_float(motor_1_thr, 0, 100, THR_MIN, THR_MAX));
		ESC2.writeMicroseconds(map_float(motor_2_thr, 0, 100, THR_MIN, THR_MAX));
		ESC3.writeMicroseconds(map_float(motor_3_thr, 0, 100, THR_MIN, THR_MAX));
		ESC4.writeMicroseconds(map_float(motor_4_thr, 0, 100, THR_MIN, THR_MAX));
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
