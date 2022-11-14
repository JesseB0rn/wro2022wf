#pragma config(Sensor, S1,     line_follower_left, sensorEV3_Color)
#pragma config(Sensor, S2,     line_follower_right, sensorEV3_Color)
#pragma config(Sensor, S3,     HT_color_l,               sensorI2CCustom)
#pragma config(Sensor, S4,     HT_color_r,               sensorI2CCustom)
#pragma config(Motor,  motorA,          motor_grab,    tmotorEV3_Medium, PIDControl, encoder)
#pragma config(Motor,  motorB,          motor_drive_left, tmotorEV3_Medium, PIDControl, reversed, driveLeft, encoder)
#pragma config(Motor,  motorC,          motor_drive_right, tmotorEV3_Medium, PIDControl, driveRight, encoder)
#pragma config(Motor,  motorD,          motor_dropper, tmotorEV3_Medium, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "lib/minmaxclip.h"
#include "lib/colorV2jester.h"
#include "lib/hsvjester.h"
#include "lib/driveModule.h"

#pragma debuggerWindows("debugStream");

#define color_left S3
#define color_right S4

#define borderW 90

// FLAGS for ITC
bool dropped = false;
bool enableMeasure = true;
bool reset = false;

// solveSide side effect
int side = 0;
// measureLB side effect
int measureIndex = 0;

// LB and Frames colors
bool colors[4];
int washables[4];
int frames[3];
rgbw maxValuesBlocks[4];
hsv maxHSVBlocks[4];

// rankLB
void rankLB(int *res)
{

	// memset(res,0,4 * sizeof(*res));
	memset(res, 0, 4 * 4);

	for (int i = 0; i < 4; i++)
	{
		res[i] = -2;
	}
	// ranking function
	// 1. kleinsten w-wert finden
	int smallest_wi, smallest_w;
	smallest_w = 255;
	smallest_wi = -1;
	rgbw *val;

	for (int i = 0; i < 4; i++)
	{
		val = &maxValuesBlocks[i];
		if (val->w < smallest_w)
		{
			smallest_w = val->w;
			smallest_wi = i;
		}
	}
	res[smallest_wi] = -1;

	// 2. kleinsten s-wert finden f�r schwarz
	hsv *hsvval;
	float smallest_s = 1.0;
	int smallest_si = -1;
	for (int i = 0; i < 4; i++)
	{
		hsvval = &maxHSVBlocks[i];
		if (hsvval->s < smallest_s && i != smallest_wi)
		{
			smallest_s = hsvval->s;
			smallest_si = i;
		}
	}
	res[smallest_si] = 0;

	// 3. find the red one that wasn't picked yet
	for (int i = 0; i < 4; i++)
	{
		hsvval = &maxHSVBlocks[i];
		if ((hsvval->h < 25 || hsvval->h > 335) && res[i] == -2)
		{
			res[i] = 2;
			break;
		}
	}

	// 4. the last one is yellow
	for (int i = 0; i < 4; i++)
	{

		if (res[i] == -2)
		{
			res[i] = 1;
			break;
		}
	}

	writeDebugStreamLine("LAUNDRY BLOCKS: %d %d %d %d", res[0], res[1], res[2], res[3]);
}

//_displayLogic
void displayLogic()
{
	for (int i = 0; i < 4; i++)
	{
		displayTextLine(i, "INDI %d: %d", i, colors[i]);
		displayTextLine(i + 4, "WASH %d: %d", i, washables[i]);
	}
	writeDebugStreamLine("[IND] B%d Y%d R%d G%d", colors[0], colors[1], colors[2], colors[3]);
	writeDebugStreamLine("[WSH] %d %d %d %d", washables[0], washables[1], washables[2], washables[3]);
}

//_measureIndicators
task measureIndicators()
{
	enableMeasure = true;
	rgbw curr;
	rgbw max;
	max.r = 0;
	max.g = 0;
	max.b = 0;
	max.w = 0;

	while(enableMeasure)
	{
		getRGBW(color_right, curr);
		rgbwMaxComponent(max, curr);

		// writeDebugStreamLine("IND R: %d %d %d %d", max.r, max.g, max.b, max.w);
		colors[side + 1] = (max.w) >  borderW;
	}
	writeDebugStreamLine("IND R: SUM: %d  W: %d", (max.r + max.g + max.b), max.w);
}
//_measureIndicators_l
task measureIndicators_l()
{
	enableMeasure  = true;
	rgbw curr;
	rgbw max;
	max.r = 0;
	max.g = 0;
	max.b = 0;
	max.w = 0;

	while(enableMeasure)
	{
		getRGBW(color_left, curr);
		rgbwMaxComponent(max, curr);
		// writeDebugStreamLine("IND L: %d %d %d %d", max.r, max.g, max.b, max.w);
		colors[side] = (max.w) >  borderW;
	}
	writeDebugStreamLine("IND L: SUM: %d  W: %d", (max.r + max.g + max.b), max.w);
}

//_measureLB
task measureLB()
{
	rgbw curr;
	rgbw max;
	max.r = 0;
	max.g = 0;
	max.b = 0;
	max.w = 0;
	writeDebugStreamLine("--- LAUNDRY SCAN STARTS HERE --------------------");
	while(true)
	{
		getRGBW(color_right, curr);
		rgbwMax(max, curr);

		hsv hsvres;
		rgb2hsv(max, hsvres);
		washables[measureIndex] = hsvToColorBlocks(hsvres, max.w);
		// writeDebugStreamLine("[MWASH RGB SUM] %d", sum);
		writeDebugStreamLine("[MWASH] %d %f %f %d", hsvres.h, hsvres.s, hsvres.v, max.w);
		maxValuesBlocks[measureIndex].r = max.r;
		maxValuesBlocks[measureIndex].g = max.g;
		maxValuesBlocks[measureIndex].b = max.b;
		maxValuesBlocks[measureIndex].w = max.w;
		maxHSVBlocks[measureIndex].h = hsvres.h;
		maxHSVBlocks[measureIndex].s = hsvres.s;
		maxHSVBlocks[measureIndex].v = hsvres.v;
	}
}
//_dropDrink()
task dropDrink() {
	dropped = false;
	setMotorTarget(motor_dropper, -200, 20);
	waitUntilMotorStop(motor_dropper);
	delay(500);
	setMotorTarget(motor_dropper, -5, 40);
	waitUntilMotorStop(motor_dropper);
	dropped = true;
}

// _resetStart
task resetStart()
{
	setMotorSpeed(motor_dropper, 10);
	setMotorSpeed(motor_grab, -25);
	wait1Msec(2000);
	setMotorSpeed(motor_dropper, 0);
	setMotorSpeed(motor_grab, 0);
	resetMotorEncoder(motor_dropper);
	wait1Msec(200);
	setMotorTarget(motor_dropper, -10, 10);
	waitUntilMotorStop(motor_dropper);
	resetMotorEncoder(motor_grab);
	resetMotorEncoder(motor_dropper);
	playTone(440, 10);
	reset = true;
}

//_pickupSingleBottle
void pickupSingleBottle() {
	setMotorTarget(motor_grab, 300, 30);
	waitUntilMotorStop(motor_grab);
	setMotorTarget(motor_grab, 70, 15);
	waitUntilMotorStop(motor_grab);
	setMotorTarget(motor_grab, 65, 15);
	delay(20);
	setMotorTarget(motor_grab, 120, 15);
	waitUntilMotorStop(motor_grab);
}

// _pickupBottles
void pickupBottles() {
#define __amt 95
	setMotorTarget(motor_dropper, 140, 50);

	turn(0, -40, 0, tireDistance/2, __amt);
	turn(0, -40, -40, -tireDistance/2, (__amt+45)*0.10);
	setMotorTarget(motor_grab, 520, 50);
	turn(-40, -40, 0, -tireDistance/2, (__amt+45)*0.90);
	delay(100);
	resetMotorEncoder(motor_drive_right);
	driveCm(-30, -30, 8);
	brake(-30, 9.8);

	pickupSingleBottle();
	setMotorTarget(motor_grab, 480, 30);
	waitUntilMotorStop(motor_grab);
	setMotorTarget(motor_grab, 510, 30);
	delay(200);

	resetMotorEncoder(motor_drive_right);
	driveCm(29, 30, 8.5);
	brake(30, 9.5);

	pickupSingleBottle();
	setMotorTarget(motor_grab, 380, 30);
	turn(0, 60, 0, -tireDistance/2, 45);

	resetMotorEncoder(motor_drive_right);
	driveCm(60, 60, 28);
	brake(60, 34);
	setMotorTarget(motor_grab, 520, 15);
	turn(0, 60, 0, tireDistance/2, 45);
	setMotorTarget(motor_grab, 520, 15);
	resetMotorEncoder(motor_drive_right);
	lfPDcm(40, 20);

}
//_solveSide
void solveSide() {
	startTask(measureIndicators);
	startTask(measureIndicators_l);
	lfPDline(15, true, true);
	resetMotorEncoder(motor_drive_right);

	// ADJUST HERE -------------------------------------------------------------------------- >
	lfPDcm(15, side == 0 ? 5.5: 5.5);
	// ADJUST HERE FOR ~5mm of area next to grey table surrounding (long table direction) ---->

	//setMotorTarget(motor_grab, 520, 30);
	enableMeasure = false;

	turn(15, 40, 15, 21, side == 0 ? 89.0 : 89.0);

	resetMotorEncoder(motor_drive_right);

	//_curva/_drift/_curl
	int curva = side == 0 ? 0 : -1;

	// side A drink
	measureIndex = side;
	if (colors[side + 1]) {
		startTask(measureLB);
		brake(40, 7);
		stopTask(measureLB);
		delay(200);

		displayLogic();

		resetMotorEncoder(motor_drive_right);
		if (washables[side] != -1) setMotorTarget(motor_grab, 380, 30);
		driveCm(-15, -15, 3.5);
		brake(-15, 4.75);

		startTask(dropDrink);
		// if theres a washable
		if (washables[side] != -1) {
			setMotorTarget(motor_grab, 70, 30);
			waitUntilMotorStop(motor_grab);
			setMotorTarget(motor_grab, 120, 30);
			waitUntilMotorStop(motor_grab);
			setMotorTarget(motor_grab, 520, 20);
		}
		waitUntil(dropped);

		driveCm(-40 - curva, -40, 25.0);
	}
	// side A ball
	else {
		startTask(measureLB);
		driveCm(57, 60, 5.0);
		stopTask(measureLB);

		displayLogic();
		driveCm(57, 60, 23.0);
		driveCm(57, 60, 26.0);
		//brake(60, 28.5);
		driveMs(20, 20, 300);
		brake(0,0);
		setMotorTarget(motor_grab, 135, 30);
		waitUntilMotorStop(motor_grab);
		delay(500);
		setMotorTarget(motor_grab, 520, 20);
		waitUntilMotorStop(motor_grab);

		// if washable
		resetMotorEncoder(motor_drive_right);
		if (washables[side] != -1) {
			setMotorTarget(motor_grab, 380, 30);
			driveCm(-40 - curva, -40, 21.5);
			brake(-40, 26.75);

			setMotorTarget(motor_grab, 70, 30);
			waitUntilMotorStop(motor_grab);
			setMotorTarget(motor_grab, 120, 30);
			waitUntilMotorStop(motor_grab);
		}
		driveCm(-40 - curva, -40, 31.0);
		setMotorTarget(motor_grab, 520, 20);
		driveCm(-40 - curva, -40, 47.0);
	}
	// side B drink
	measureIndex = side + 1;
	resetMotorEncoder(motor_drive_right);
	if (colors[side]) {
		driveCm(-40, -40, 33.0);
		startTask(measureLB);
		brake(-40, 36.75);
		stopTask(measureLB);

		displayLogic();
		dropped = false;
		startTask(dropDrink);
		// if theres a washable
		if (washables[side+1] != -1) {
			setMotorTarget(motor_grab, 70, 30);
			waitUntilMotorStop(motor_grab);
			setMotorTarget(motor_grab, 120, 30);
			waitUntilMotorStop(motor_grab);
			//setMotorTarget(motor_grab, 520, 20);
		}
		waitUntil(dropped);

	}
	// side B ball
	else {
		driveCm(-39, -40, 30.0);
		startTask(measureLB);
		driveCm(-40, -40, 36.0);
		stopTask(measureLB);

		displayLogic();
		driveCm(-40, -40, 46.0);
		driveCm(-40, -40, 50.6);
		driveMs(-20, -20, 300);
		brake(0,0);
		resetMotorEncoder(motor_drive_right);
		setMotorTarget(motor_grab, 135, 30);
		waitUntilMotorStop(motor_grab);
		driveCm(10, 10, 2);
		brake(0, 0);
		delay(100);
		setMotorTarget(motor_grab, 520, 20);
		waitUntilMotorStop(motor_grab);

		if (washables[side + 1] != -1) {
			driveCm(30, 30, 13);
			brake(30, 16);
			setMotorTarget(motor_grab, 70, 30);
			waitUntilMotorStop(motor_grab);
			setMotorTarget(motor_grab, 120, 30);
			waitUntilMotorStop(motor_grab);
			} else {
			setMotorTarget(motor_grab, 70, 30);
		}

		driveCm(40, 40, 17.5);
	}
	if (side == 2) {
		resetMotorEncoder(motor_drive_right);
		driveCm(40, 40, 8.5);
		turn(40, 40, 0, 24.5, 88.0);
		return;
	}
	turn(40, 40, 0, 33.25, 88.0);
}
//_gotoSide2
void gotoSide2() {
	side = 2; // <- side effect for solve side
	// if(true) {
	resetMotorEncoder(motor_drive_right);
	setMotorTarget(motor_grab, 50, 30);
	lfPDcm(15, 8);
	lfPDline(40, true, true);
	resetMotorEncoder(motor_drive_right);
	lfPDcm(40, 19);
	driveCm(40, 40, 43);
	brake(40, 48);

	turn(40, 60, 40, tireDistance/2, 25);
	turn(40, 60, 40, -40, 48);
	turn(40, 60, 40, tireDistance/2, 15);
	setMotorTarget(motor_grab, 520, 30);
	//resetMotorEncoder(motor_drive_right);
	//stopAllTasks();
	resetMotorEncoder(motor_drive_right);
	lfPDcm(30, 15);
	lfPDcm(15, 20);
	// 	} else {
	// 	resetMotorEncoder(motor_drive_right);
	// 	lfPDcm(15, 8);
	// 	lfPDline(60, true, true);
	// 	resetMotorEncoder(motor_drive_right);
	// 	lfPDcm(60, 19);
	// 	driveCm(60, 60, 40);
	// 	setMotorTarget(motor_grab, 520, 30);
	// 	driveCm(60,60,90);
	// 	resetMotorEncoder(motor_drive_right);
	// 	lfPDcm(30, 15);
	// 	lfPDcm(15, 20);
	// }

}

//_gotoLaundryArea
void gotoLaundryArea() {
	hsv res;
	rgbw curr;

	setMotorTarget(motor_grab, 30, 30);
	lfPDline(15, true, true);
	turn(40, 60, 40, -tireDistance/2, 17.5);
	turn(40, 60, 35, -70, 37.5);
	turn(30, 60, 20, tireDistance/2, 55.5);
	brake(0, 0);

	resetMotorEncoder(motor_drive_right);
	driveCm(30, 30, 3);
	brake(30, 5);
	delay(200);

	getRGBW(color_left, curr);
	rgb2hsv(curr, res);
	frames[0] = hsvToColorFrames(res, curr.w);
	writeDebugStreamLine("[FRAME 0] %d %f %f %d", res.h, res.s, res.v, curr.w);

	resetMotorEncoder(motor_drive_right);
	driveCm(-30, -30, 11);
	brake(-30, 8);
	delay(200);

	getRGBW(color_left, curr);
	rgb2hsv(curr, res);

	rgb2hsv(curr, res);
	frames[1] = hsvToColorFrames(res, curr.w);
	writeDebugStreamLine("[FRAME 1] %d %f %f %d", res.h, res.s, res.v, curr.w);


	frames[2] = 3 - (frames[1] + frames[0]);
	writeDebugStreamLine("[FRAMES] %d %d %d", frames[0], frames[1], frames[2]);
}
//_dropLB
void dropLB() {
	setMotorTarget(motor_dropper, -100, 30);
	waitUntilMotorStop(motor_dropper);
	setMotorTarget(motor_dropper, -200, 55);
	waitUntilMotorStop(motor_dropper);
	delay(500);
	setMotorTarget(motor_dropper, -10, 60);
	delay(500);
}
//_dropLBs
void dropLBs() {
	int lb[4];
	rankLB(lb);
	setMotorTarget(motor_grab, 120, 15);
	resetMotorEncoder(motor_drive_right);
	int current_pos = 0;
	int centerToCenterCM = 11;
	for (int i = 0; i < 5; i++) {
		int t = 4.25;
		if (i != 4) {
			int value = lb[i];
			if (value == -1) {
				continue;
			}
			int index = 0;
			while ( index < 3 && frames[index] != value ) {
				++index;
			}
			t = (1 - ( index == 3 ? 1 : index )) * centerToCenterCM + 0.1;
		}

		// writeDebugStreamLine("[ Next Target ] %d", t);
		resetMotorEncoder(motor_drive_right);
		if (t <= current_pos) {
			driveCm(-30, -30, current_pos-t);
			} else if (t == current_pos) {} else {
			driveCm(30, 30, current_pos-t);
		}
		brake(0, 0);
		if (i != 4) dropLB();

		current_pos = t;
	}
}
//_end
void end() {
	turn(0, 40, 0, 0, 88.0);
	resetMotorEncoder(motor_drive_right);
	driveCm(-30, -30, -4);
	brake(0, 0);
	resetMotorEncoder(motor_drive_right);
	lfPDcm(15, 8);
	lfPDend(15);
	resetMotorEncoder(motor_drive_right);
	speedChange(15, 40);
	driveCm(40, 40, 18.0);
	brake(40, 23.0);
	turn(0, 40, 0, 0, 44);
}

// _main
task main()
{
	clearDebugStream();
	setMotorBrakeMode(motorA, motorBrake);
	setMotorBrakeMode(motorB, motorBrake);
	setMotorBrakeMode(motorC, motorBrake);
	setMotorBrakeMode(motorD, motorBrake);
	if (!verifySetupHTCOL(S3) || !verifySetupHTCOL(S4)) {
		writeDebugStreamLine("[INIT] SENSOR ERROR, check ht colors");
	}
	startTask(resetStart);
	eraseDisplay();
	waitUntil(reset);
	setLEDColor(ledGreen);
	waitForButtonPress();
	eraseDisplay();
	setLEDColor(ledOff);
	delay(150);
	clearTimer(T4);

	pickupBottles();

	solveSide();
	gotoSide2();
	solveSide();
	gotoLaundryArea();
	dropLBs();

	end();

	brake(0, 0);
	eraseDisplay();
	displayCenteredBigTextLine(4, "%d.0 s", (float)time100[T4]/10.0);
	writeDebugStreamLine("TIME: %d", (float)time100[T4]/10.0);
	delay(10000);
	stopAllTasks();
}
