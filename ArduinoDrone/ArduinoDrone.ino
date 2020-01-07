
#include "controller.h"
#include "motors.h"


void setup() 
{
	Serial.begin(9600);
	Controller::initialize();
	Motors::initialize();
}


void loop() 
{
	Serial.println(Controller::get_axis_percent(Controller::Pins::THROTTLE));
	Motors::set_throttle_percent(Controller::get_axis_percent(Controller::Pins::THROTTLE));
	/*Motors::set_throttle_percent(10);
	delay(5000);
	Motors::set_throttle_percent(0);

	while (true);*/

	//delay(500);
}