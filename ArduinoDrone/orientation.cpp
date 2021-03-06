#include "globals.h"
#include "orientation.h"

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"
//#include "MPU6050.h" // not necessary if using MotionApps include file

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;


/**
FIFO overflow means that the mainloop is too slow!
This is caused by a buffer overflow. There needs to be constant querying to the accel to prevent this and thus no using "delay()"!
**/
namespace Orientation
{
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

	const int CALIB_TIME = 1000;
	float ypr_offsets[3] = { 0, 0, 0 };
	unsigned long fifo_loop_start = 0;

	// MPU control/status vars
	bool dmpReady = false;  // set true if DMP init was successful
	uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
	uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
	uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
	uint16_t fifoCount;     // count of all bytes currently in FIFO
	uint8_t fifoBuffer[64]; // FIFO storage buffer

	// orientation/motion vars
	Quaternion q;           // [w, x, y, z]         quaternion container
	VectorInt16 aa;         // [x, y, z]            accel sensor measurements
	VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
	VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
	VectorFloat gravity;    // [x, y, z]            gravity vector
	float euler[3];         // [psi, theta, phi]    Euler angle container
	float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

	// ================================================================
	// ===               INTERRUPT DETECTION ROUTINE                ===
	// ================================================================

	volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
	void dmpDataReady()
	{
		mpuInterrupt = true;
	}

	// ================================================================
	// ===                      INITIAL SETUP                       ===
	// ================================================================

	void initialize()
	{
		// join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
		Wire.begin();
		TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
		Fastwire::setup(400, true);
#endif

		// NOTE: 8MHz or slower host processors, like the Teensy @ 3.3v or Ardunio
		// Pro Mini running at 3.3v, cannot handle this baud rate reliably due to
		// the baud timing being too misaligned with processor ticks. You must use
		// 38400 or slower in these cases, or use some kind of external separate
		// crystal solution for the UART timer.

		// initialize device
		Serial.println(F("Initializing I2C devices..."));
		mpu.initialize();

		// verify connection
		Serial.println(F("Testing device connections..."));
		Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

		// load and configure the DMP
		Serial.println(F("Initializing Accel DMP..."));
		devStatus = mpu.dmpInitialize();

		// supply your own gyro offsets here, scaled for min sensitivity
		mpu.setXGyroOffset(220);
		mpu.setYGyroOffset(76);
		mpu.setZGyroOffset(-85);
		mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

		// make sure it worked (returns 0 if so)
		if (devStatus == 0)
		{
			// turn on the DMP, now that it's ready
			Serial.println(F("Enabling DMP..."));
			mpu.setDMPEnabled(true);

			// enable Arduino interrupt detection
			Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
			attachInterrupt(0, dmpDataReady, RISING);
			mpuIntStatus = mpu.getIntStatus();

			// set our DMP Ready flag so the main loop() function knows it's okay to use it
			Serial.println(F("DMP ready! Waiting for first interrupt..."));
			dmpReady = true;

			// get expected DMP packet size for later comparison
			packetSize = mpu.dmpGetFIFOPacketSize();
		}
		else
		{
			// ERROR!
			// 1 = initial memory load failed
			// 2 = DMP configuration updates failed
			// (if it's going to break, usually the code will be 1)
			Serial.print(F("DMP Initialization failed (code "));
			Serial.print(devStatus);
			Serial.println(F(")"));
		}
	}

	void calibrate()
	{
		Serial.print("Orientation calibrating...(Taking ");
		Serial.print(CALIB_TIME);
		Serial.println("ms)");

		float start = millis();
		float total_ypr[3] = { 0, 0, 0 };
		int samples = 0;
		while (millis() - start < CALIB_TIME)
		{
			update();
			total_ypr[Axis::YAW] += ypr[Axis::YAW];
			total_ypr[Axis::PITCH] += ypr[Axis::PITCH];
			total_ypr[Axis::ROLL] += ypr[Axis::ROLL];
			samples++;
		}
		
		ypr_offsets[Axis::YAW] = total_ypr[Axis::YAW] / (float)samples * 180 / M_PI;
		ypr_offsets[Axis::PITCH] = total_ypr[Axis::PITCH] / (float)samples * 180 / M_PI;
		ypr_offsets[Axis::ROLL] = total_ypr[Axis::ROLL] / (float)samples * 180 / M_PI;
	}

	void update()
	{
		// if programming failed, don't try to do anything
		if (!dmpReady) return;

		// wait for MPU interrupt or extra packet(s) available
		while (!mpuInterrupt && fifoCount < packetSize)
		{
			// other program behavior stuff here
			// .
			// .
			// .
			// if you are really paranoid you can frequently test in between other
			// stuff to see if mpuInterrupt is true, and if so, "break;" from the
			// while() loop to immediately process the MPU data
			// .
			// .
			// .
		}

		// reset interrupt flag and get INT_STATUS byte
		mpuInterrupt = false;
		// Using MPU status causes the arduino to freeze if accel is shaken around
		// https://forum.arduino.cc/index.php?topic=408851.0
		//mpuIntStatus = mpu.getIntStatus();

		// get current FIFO count
		fifoCount = mpu.getFIFOCount();

		// check for overflow (this should never happen unless our code is too slow)
		//if ((mpuIntStatus & 0x10) || fifoCount == 1024)
		if(fifoCount == 1024)
		{
			// reset so we can continue cleanly
			mpu.resetFIFO();
			Serial.println(F("FIFO overflow! Turn on RC to possibly fix this."));

			// otherwise, check for DMP data ready interrupt (this should happen frequently)
		}
		//else if (mpuIntStatus & 0x02)
		else
		{
			fifo_loop_start = millis();
			// wait for correct available data length, should be a VERY short wait
			while (fifoCount < packetSize) { 
				fifoCount = mpu.getFIFOCount();

				if (millis() - fifo_loop_start >= 500)
				{
					Serial.println("FIFO LOOP TOOK TOO LONG! Is the accel disconnected? Disarming...");
					failsafe = true;
					return;
				}
			}

			// read a packet from FIFO
			mpu.getFIFOBytes(fifoBuffer, packetSize);

			// track FIFO count here in case there is > 1 packet available
			// (this lets us immediately read more without waiting for an interrupt)
			fifoCount -= packetSize;

			// Update accel readings
			mpu.dmpGetQuaternion(&q, fifoBuffer);
			mpu.dmpGetGravity(&gravity, &q);
			mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
		}

		Serial.println(millis());
	}

	float get_axis_degrees(int axis)
	{
		if (axis == Axis::PITCH || axis == Axis::ROLL)
			return -ypr[axis] * 180 / M_PI - ypr_offsets[axis];
		else
			return ypr[axis] * 180 / M_PI - ypr_offsets[axis];
	}
}