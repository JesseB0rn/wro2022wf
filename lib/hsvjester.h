#define HSV_DELTA_MIN 0.0000000001

typedef struct {
	float h;       // angle in degrees
	float s;       // a fraction between 0 and 1
	float v;       // a fraction between 0 and 1
} hsv;

// _rgb2hsv
void rgb2hsv(rgbw _in, hsv &out)
{
	rgbw in;
	in.r = _in.r/255.0;
	in.g = _in.g/255.0;
	in.b = _in.b/255.0;

	float min, max, delta;

	min = min3(in.r, in.g, in.b);
	max = max3(in.r, in.g, in.b);

	out.v = max;                                // v
	delta = max - min;
	if (delta < HSV_DELTA_MIN)
	{
		out.s = 0;
		out.h = 0; // undefined, maybe nan?
		return;
	}
	if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
		out.s = (delta / max);                  // s
		} else {
		// if max is 0, then r = g = b = 0
		// s = 0, h is undefined
		out.s = 0.0;
		out.h = 0;                            // its now undefined
		return;
	}
	if( in.r >= max )                           // > is bogus, just keeps compiler happy
		out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
	else
		if( in.g >= max )
		out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
	else
		out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

	out.h *= 60.0;                              // degrees

	if( out.h < 0.0 )
		out.h += 360.0;

	return;
}

//_hsvToColor
int hsvToColorFrames(hsv in, float w) {
	if (in.s < 0.35 && w < 65)
		return 0;

	if (in.h < 25 || in.h > 335)
		return 2;

	if (in.h >= 30 && in.h <= 55)
		return 1;
	return 0;
}

//_hsvToColorBlocks
int hsvToColorBlocks(hsv in, float w) {
	if (w < 20)
		return -1;
	if (w < 100 || in.s < 0.2)
		return 0;

	if (in.h < 25 || in.h > 335)
		return 2;

	if (in.h >= 30 && in.h <= 55)
		return 1;
	return 0;
}
