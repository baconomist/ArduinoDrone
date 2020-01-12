#include "utils.h"

float clamp(float val, float min, float max)
{
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

float map_float(float val, float val_min, float val_max, float map_min, float map_max)
{
	return map_min + (val - val_min) / (val_max - val_min) * (map_max - map_min);
}
