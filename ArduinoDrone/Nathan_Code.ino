/*

Luigiman54Today at 8:33 AM
//MIN = 1040  MAX = 2000
#include <Servo.h>
Servo ESC1;
Servo ESC2;
Servo ESC3;
Servo ESC4;
void setup() {
	ESC1.attach(11);
	ESC2.attach(10);
	ESC3.attach(9);
	ESC4.attach(6);

	int startTime = millis();
	int x = 8000;
	while (millis() < startTime + x) {
		ESC1.writeMicroseconds(1000);
		ESC2.writeMicroseconds(1000);
		ESC3.writeMicroseconds(1000);
		ESC4.writeMicroseconds(1000);
	}

	Serial.begin(9600);
	pinMode(A0, INPUT);

}
void loop() {
	ESC1.writeMicroseconds(1200);
	ESC2.writeMicroseconds(1200);
	ESC3.writeMicroseconds(1200);
	ESC4.writeMicroseconds(1200);
	int roll = map(pulseIn(A0, HIGH), 984, 2003, 1000, 2000);
	int pitch = map(pulseIn(A0, HIGH), 984, 2003, 1000, 2000);
	int throttle = map(pulseIn(A0, HIGH), 984, 2002, 1000, 2000);
	int yaw = map(pulseIn(A0, HIGH), 984, 2002, 1000, 2000);
	//if (roll > 1520 or roll < 1460){
	Serial.println(roll);
	Serial.println(pitch);
	Serial.println(throttle);
	Serial.println(yaw);
	//}
}





Luigiman54Today at 9:34 AM
#include<Wire.h>
const int MPU = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
int offsetX = 0;
int offsetY = 0;

void setup() {
	Wire.begin();
	Wire.beginTransmission(MPU);
	Wire.write(0x6B);
	Wire.write(0);
	Wire.endTransmission(true);
	Wire.beginTransmission(MPU);
	Wire.write(0x3B);
	Wire.endTransmission(false);
	Wire.requestFrom(MPU, 12, true);
	offsetX = Wire.read() << 8 | Wire.read();
	offsetY = Wire.read() << 8 | Wire.read();
	Serial.begin(9600);
}
void loop() {
	Wire.beginTransmission(MPU);
	Wire.write(0x3B);
	Wire.endTransmission(false);
	Wire.requestFrom(MPU, 12, true);
	AcX = Wire.read() << 8 | Wire.read();
	AcY = Wire.read() << 8 | Wire.read();

	Serial.print("Accelerometer: ");
	if (abs(AcX - offsetX) < 500 and abs(AcY - offsetY) < 500) {
		Serial.println("level");
	}
	else {
		Serial.print("X = "); Serial.print((((AcX + 100) / 100)100) - offsetX);
		Serial.print(" | Y = "); Serial.print((((AcY + 100) / 100)100) - offsetY);
	}

	Serial.println(" ");
	Serial.println(offsetX);
	Serial.println(offsetY);
}
*/