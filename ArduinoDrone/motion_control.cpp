#include <Arduino.h>

namespace MotionControl 
{
	float p_controller(float current, float end, float kP) 
	{
		return (end - current) * kP;
	}
}