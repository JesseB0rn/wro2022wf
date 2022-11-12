#pragma config(Sensor, S3,     HT_color_l,               sensorI2CCustom)
#pragma config(Sensor, S4,     HT_color_r,               sensorI2CCustom)

#include "../lib/minmaxclip.h"
#include "../lib/colorV2jester.h"
#include "../lib/hsvjester.h"

task main()
{
	clearDebugStream();
	rgbw left, right;
	hsv lhsv, rhsv;
	while(true) {
			getRGBW(S3, left);
			getRGBW(S4, right);
			rgb2hsv(left, lhsv);
			rgb2hsv(right, rhsv);
			displayTextLine(0, "%f %f %f %f", right.r/255.0, right.g/255.0, right.b/255.0, right.w/255.0);
			displayTextLine(1, "%d %f %f %d", rhsv.h, rhsv.s, rhsv.v, right.w);
			displayTextLine(4, "RESULT: %d", hsvToColorBlocks(rhsv, right.w));

	}


}
