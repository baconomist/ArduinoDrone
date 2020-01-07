
#include "controller.h"
#include "motors.h"


void setup() {
	

	Serial.begin(9600);
	Controller::initialize();
	Motors::initialize();
}
void loop() {
	/*ESC1.writeMicroseconds(1200);
	ESC2.writeMicroseconds(1200);
	ESC3.writeMicroseconds(1200);
	ESC4.writeMicroseconds(1200);*/

	Serial.println(Controller::get_axis(Controller::Pins::PITCH));
	Motors::set_throttle_percent(1);

	delay(500);
}