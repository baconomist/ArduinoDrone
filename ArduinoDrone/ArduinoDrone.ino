
#include "controller.h"
#include "motors.h"
#include "orientation.h"
#include "utils.h"

bool armed = false;
const int armDelay = 1000;
float armTimer = 0;

void setup()
{
	Serial.begin(9600);

	Orientation::initialize();
	Orientation::calibrate();

	Controller::initialize();
	Motors::initialize();
}

void loop()
{
	Orientation::update();

	//Serial.println(Orientation::get_axis_degrees(Orientation::Axis::ROLL));
	//Serial.println(Orientation::get_axis_degrees(Orientation::Axis::PITCH));
	
	if (Controller::get_axis(Controller::Pins::CH_5) < 1200)
	{
		Motors::disarm();
		armed = false;

		if (millis() - armTimer >= armDelay)
		{
			Serial.println("Disarmed.");
			armTimer = millis();
		}

		return;
	}
	else if (!armed) 
	{
		Serial.println("Arming...");
		armed = true;
	}

	Motors::set_throttle_percent(Controller::get_axis_percent(Controller::Pins::THROTTLE));
	Motors::set_pitch_percent(Controller::get_axis_percent(Controller::Pins::PITCH));
	Motors::set_roll_percent(Controller::get_axis_percent(Controller::Pins::ROLL));
	Motors::set_yaw_percent(Controller::get_axis_percent(Controller::Pins::YAW));
}