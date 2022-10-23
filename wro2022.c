#pragma config(Sensor, S1,     line_follower_left, sensorEV3_Color)
#pragma config(Sensor, S2,     line_follower_right, sensorEV3_Color)
#pragma config(Motor,  motorA,          motor_grab,    tmotorEV3_Medium, PIDControl, encoder)
#pragma config(Motor,  motorB,          motor_drive_left, tmotorEV3_Medium, PIDControl, reversed, driveLeft, encoder)
#pragma config(Motor,  motorC,          motor_drive_right, tmotorEV3_Medium, PIDControl, driveRight, encoder)
#pragma config(Motor,  motorD,          motor_dropper, tmotorEV3_Medium, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
#include "robotcdriversuite/include/hitechnic-colour-v2.h"
#pragma debuggerWindows("debugStream");

typedef struct {
	long r;       // a fraction between 0 and 1
	long g;       // a fraction between 0 and 1
	long b;       // a fraction between 0 and 1
} rgb;

typedef struct {
	float h;       // angle in degrees
	float s;       // a fraction between 0 and 1
	float v;       // a fraction between 0 and 1
} hsv;

float LF_P_a = 0.000065;
float LF_P_u = 45;
float LF_P_v = 0.12;
float LF_D_a = 0.0018;
float LF_D_u = 35;
float LF_D_v = 6.2;

bool dropped = false;

float tireDiameter = 6.24;
float tireDistance = 17.20;
float brakeCons = 0.08;
float brakeConsTurn = 0.11;

bool colors[4];
int washables[4];

int side = 0;
int measureIndex = 0;

bool loop_stop = false;
bool reset = false;


int borderRGB = 120;

tHTCS2 color_left;
tHTCS2 color_right;



//_displayLogic
void displayLogic() {
	for(int i = 0; i < 4; i++)
	{
		displayTextLine(i, "INDI %d: %d", i, colors[i]);
		displayTextLine(i+4, "WASH %d: %d", i, washables[i]);
	}
	writeDebugStreamLine("[IND] B%d Y%d R%d G%d", colors[0], colors[1], colors[2], colors[3]);
	writeDebugStreamLine("[WSH] %d %d %d %d", washables[0], washables[1],washables[2],washables[3]);
}

//_lfPDcm
void lfPDcm(float speed, float distance)
{
	float kP = 0;
	float kD = 0;
	float error = 0;
	float lastError = 0;
	float derivative = 0;
	float correction = 0;
	float sum = 0;
	float value_left = 0;
	float value_right = 0;
	float time = 0;
	if(speed == 15)
	{
		kP = 0.1;
		kD = 1.5;
		} else {
		kP = LF_P_a*pow(speed - LF_P_u, 2) + LF_P_v;
		kD = LF_D_a*pow(speed - LF_D_u, 2) + LF_D_v;
	}
	while(distance >= getMotorEncoder(motor_drive_right)/360*PI*tireDiameter)
	{
		value_left = getColorReflected(line_follower_left);
		value_right = getColorReflected(line_follower_right);
		error = value_left - value_right;
		sum = value_left + value_right;
		time = time1[timer1];
		clearTimer(timer1);
		derivative = error - lastError;
		correction = error*kP + derivative*kD/time;
		if(sum <= 55)
		{
			setMotorSpeed(motorB, speed);
			setMotorSpeed(motorC, speed);
			} else {
			setMotorSpeed(motorB, speed + correction);
			setMotorSpeed(motorC, speed - correction);
		}
		lastError = error;
		waitUntil(time1[timer1] >= 5);
	}
}


//_lfPDline
void lfPDline(float speed, bool sensor1, bool sensor4)
{
	float kP = 0;
	float kD = 0;
	float error = 0;
	float lastError = 0;
	float derivative = 0;
	float correction = 0;
	float sum = 0;
	float value_left = 0;
	float value_right = 0;
	float time = 0;
	loop_stop = false;
	if(speed == 15)
	{
		kP = 0.1;
		kD = 1.5;
		} else {
		kP = LF_P_a*pow(speed - LF_P_u, 2) + LF_P_v;
		kD = LF_D_a*pow(speed - LF_D_u, 2) + LF_D_v;
	}
	while(loop_stop == false)
	{
		value_left = getColorReflected(line_follower_left);
		value_right = getColorReflected(line_follower_right);
		error = value_left - value_right;
		sum = value_left + value_right;
		time = time1[timer1];
		clearTimer(timer1);
		derivative = error  - lastError;
		correction = error*kP + derivative*kD/time;
		if(sum <= 55)
		{
			setMotorSpeed(motorB, speed);
			setMotorSpeed(motorC, speed);
			} else {
			setMotorSpeed(motorB, speed + correction);
			setMotorSpeed(motorC, speed - correction);
		}
		lastError = error;
		if(sensor1 == true && sensor4 == true)
		{
			loop_stop = (sum <= 40);
			} else {
			if(sensor1 == true)
			{
				loop_stop = value_right <= 15;
				} else {
				loop_stop = value_left <= 15;
			}
		}
		waitUntil(time1[timer1] >= 5);
	}
}


//_lfPDacc
void lfPDacc(float speedStart, float speedEnd)
{
	float kP = 0;
	float kD = 0;
	float error = 0;
	float lastError = 0;
	float derivative = 0;
	float correction = 0;
	float sum = 0;
	float value_left = 0;
	float value_right = 0;
	float time = 0;
	float accConstant = 0;
	float currentSpeed = 0;
	float counter = 0;
	if(speedStart < speedEnd)
	{
		accConstant = 1;
		} else {
		accConstant = -1;
	}
	while(speedStart+accConstant*counter != speedEnd)
	{
		currentSpeed = speedStart+accConstant*counter;
		if(currentSpeed < 6)
		{
			currentSpeed = 6;
		}
		kP = (LF_P_a*pow(currentSpeed - LF_P_u, 2) + LF_P_v);
		kD = (LF_D_a*pow(currentSpeed - LF_D_u, 2) + LF_D_v);
		//kP = pow(currentSpeed, 0.5)*0.024;
		//kD = pow(currentSpeed, 0.5)*0.25;
		value_left = getColorReflected(line_follower_left);
		value_right = getColorReflected(line_follower_right);
		error = value_left - value_right;
		sum = value_left + value_right;
		time = time1[timer1];
		clearTimer(timer1);
		derivative = error - lastError;
		correction = error*kP + derivative*kD/time;
		if(sum <= 55)
		{
			setMotorSpeed(motorB, currentSpeed);
			setMotorSpeed(motorC, currentSpeed);
			} else {
			setMotorSpeed(motorB, currentSpeed + correction);
			setMotorSpeed(motorC, currentSpeed - correction);
		}
		lastError = error;
		waitUntil(time1[timer1] >= 5);
		counter ++;
	}
}


//_lfPDstart
void lfPDstart(float speed, float distance)
{
	float kP = 0.15;
	float kD = 10;
	if(speed == 60)
	{
		kP = 0.15;
		kD = 10;
	}
	float error = 0;
	float lastError = 0;
	float derivative = 0;
	float correction = 0;
	float time = 0;
	while(distance >= getMotorEncoder(motor_drive_right)/360*PI*tireDiameter)
	{
		error = (32 - getColorReflected(line_follower_right)) * -1;
		time = time1[timer1];
		clearTimer(timer1);
		derivative = error - lastError;
		correction = error*kP + derivative*kD/time;
		setMotorSpeed(motorB, speed + correction);
		setMotorSpeed(motorC, speed - correction);
		lastError = error;
		waitUntil(time1[timer1] >= 8);
	}
}


//_turn
void turn(float speed1, float speed2, float speed3, float radius, float angle)
{
	float speedBrake = 0;
	int counter = 0;
	float rotataionsB = 0;
	float rotataionsC = 0;
	float error = 0;
	float lastError = 0;
	float Derivative = 0;
	float kP = 0.012;
	float kD = 0.05;
	float Correction = 0;
	float AbsRadius = abs(radius);
	float degrees = abs(2*PI*(AbsRadius+tireDistance/2)/(tireDiameter*PI)*angle);
	float left_rightRatio = (AbsRadius-tireDistance/2)/(AbsRadius+tireDistance/2);
	resetMotorEncoder(motor_drive_left);
	resetMotorEncoder(motor_drive_right);
	if(radius < 0)
	{
		if(speed1 <= speed2)
		{
			while(speed1+1.8*counter < speed2)
			{
				rotataionsB = getMotorEncoder(motor_drive_left)*(-1);
				rotataionsC = getMotorEncoder(motor_drive_right);
				error = abs(rotataionsC*left_rightRatio)-abs(rotataionsB);
				Correction = error*kP;
				setMotorSpeed(motor_drive_left, (speed1+1.8*counter)*left_rightRatio*(1+Correction));
				setMotorSpeed(motor_drive_right, (speed1+1.8*counter)*(1-Correction));
				wait1Msec(5);
				counter ++;
			}
			} else {
			while(speed1-1.8*counter > speed2)
			{
				rotataionsB = getMotorEncoder(motor_drive_left)*(-1);
				rotataionsC = getMotorEncoder(motor_drive_right);
				error = abs(rotataionsC*left_rightRatio)-abs(rotataionsB);
				Correction = error*kP;
				setMotorSpeed(motor_drive_left, (speed1-1.8*counter)*left_rightRatio*(1+Correction));
				setMotorSpeed(motor_drive_right, (speed1-1.8*counter)*(1-Correction));
				wait1Msec(5);
				counter ++;
			}
		}
		if(speed3 < speed2)
		{
			while(degrees-abs(((speed2-speed3)*(speed2+speed3))/2*brakeConsTurn) > abs(getMotorEncoder(motor_drive_right)))
			{
				rotataionsB = getMotorEncoder(motor_drive_left)*(-1);
				rotataionsC = getMotorEncoder(motor_drive_right);
				error = abs(rotataionsC*left_rightRatio)-abs(rotataionsB);
				Derivative = error - lastError;
				Correction = error*kP + Derivative*kD;
				setMotorSpeed(motor_drive_left, speed2*left_rightRatio*(1+Correction));
				setMotorSpeed(motor_drive_right, speed2*(1-Correction));
				lastError = error;
			}
			while(degrees-abs(getMotorEncoder(motor_drive_right)) > 3)
			{
				speedBrake = sqrt(2*(degrees-abs(getMotorEncoder(motor_drive_right)))/brakeConsTurn+pow(speed3,2))*speed2/abs(speed2);
				rotataionsB = getMotorEncoder(motor_drive_left)*(-1);
				rotataionsC = getMotorEncoder(motor_drive_right);
				setMotorSpeed(motor_drive_left, speedBrake*left_rightRatio);
				setMotorSpeed(motor_drive_right, speedBrake);
			}
			} else {
			while(abs(getMotorEncoder(motor_drive_right)) <= degrees)
			{
				rotataionsB = getMotorEncoder(motor_drive_left)*(-1);
				rotataionsC = getMotorEncoder(motor_drive_right);
				error = abs(rotataionsC*left_rightRatio)-abs(rotataionsB);
				Derivative = error - lastError;
				Correction = error*kP + Derivative*kD;
				setMotorSpeed(motor_drive_left, speed2*left_rightRatio*(1+Correction));
				setMotorSpeed(motor_drive_right, speed2*(1-Correction));
				lastError = error;
			}
		}
		} else {
		if(speed1 <= speed2)
		{
			while(speed1+1.8*counter < speed2)
			{
				rotataionsB = getMotorEncoder(motor_drive_left)*(-1);
				rotataionsC = getMotorEncoder(motor_drive_right);
				error = abs(rotataionsB*left_rightRatio)-abs(rotataionsC);
				Correction = error*kP;
				setMotorSpeed(motor_drive_left, (speed1+1.8*counter)*(1-Correction));
				setMotorSpeed(motor_drive_right, (speed1+1.8*counter)*left_rightRatio*(1+Correction));
				wait1Msec(5);
				counter ++;
			}
			} else {
			while(speed1-1.8*counter > speed2)
			{
				rotataionsB = getMotorEncoder(motor_drive_left)*(-1);
				rotataionsC = getMotorEncoder(motor_drive_right);
				error = abs(rotataionsB*left_rightRatio)-abs(rotataionsC);
				Correction = error*kP;
				setMotorSpeed(motor_drive_left, (speed1-1.8*counter)*(1-Correction));
				setMotorSpeed(motor_drive_right, (speed1-1.8*counter)*left_rightRatio*(1+Correction));
				wait1Msec(5);
				counter ++;
			}
		}
		if(speed3 < speed2)
		{
			while(degrees-abs(((speed2-speed3)*(speed2+speed3))/2*brakeConsTurn) > abs(getMotorEncoder(motor_drive_left)))
			{
				rotataionsB = getMotorEncoder(motor_drive_left)*(-1);
				rotataionsC = getMotorEncoder(motor_drive_right);
				error = abs(rotataionsB*left_rightRatio)-abs(rotataionsC);
				Derivative = error - lastError;
				Correction = error*kP + Derivative*kD;
				setMotorSpeed(motor_drive_left, speed2*(1-Correction));
				setMotorSpeed(motor_drive_right, speed2*left_rightRatio*(1+Correction));
				lastError = error;
			}
			while(degrees-abs(getMotorEncoder(motor_drive_left)) > 3)
			{
				speedBrake = sqrt(2*(degrees-abs(getMotorEncoder(motor_drive_left)))/brakeConsTurn+pow(speed3,2))*speed2/abs(speed2);
				rotataionsB = getMotorEncoder(motor_drive_left)*(-1);
				rotataionsC = getMotorEncoder(motor_drive_right);
				setMotorSpeed(motor_drive_left, speedBrake);
				setMotorSpeed(motor_drive_right, speedBrake*left_rightRatio);
			}
			} else {
			while(abs(getMotorEncoder(motor_drive_left)) <= degrees)
			{
				rotataionsB = getMotorEncoder(motor_drive_left)*(-1);
				rotataionsC = getMotorEncoder(motor_drive_right);
				error = abs(rotataionsB*left_rightRatio)-abs(rotataionsC);
				Derivative = error - lastError;
				Correction = error*kP + Derivative*kD;
				setMotorSpeed(motor_drive_left, speed2*(1-Correction));
				setMotorSpeed(motor_drive_right, speed2*left_rightRatio*(1+Correction));
				lastError = error;
			}
		}
	}
	if(speed3 == 0)
	{
		setMotorSpeed(motor_drive_left, 0);
		setMotorSpeed(motor_drive_right, 0);
	}
}


//_brake
void brake(float speed, float distance)
{
	float degrees = distance/(tireDiameter*PI)*360;
	float speedBrake = speed;
	setMotorSync(motor_drive_left, motor_drive_right, -100, speed);
	waitUntil(abs(degrees)-abs((speed*(speed+1))/2*brakeCons) <= abs(getMotorEncoder(motor_drive_right)));
	while(abs(degrees)-abs(getMotorEncoder(motor_drive_right)) > 2)
	{
		speedBrake = (-1+sqrt(1+8*(degrees-abs(getMotorEncoder(motor_drive_right)))/brakeCons))/2*speed/abs(speed);
		setMotorSync(motor_drive_left, motor_drive_right, -100, speedBrake);
	}
	setMotorSpeed(motor_drive_left, 0);
	setMotorSpeed(motor_drive_right, 0);
	wait1Msec(10);
}


//_speedChange
void speedChange(float startSpeed, float endSpeed)
{
	int counter = 0;
	if(startSpeed <= endSpeed)
	{
		while(startSpeed+1.8*counter < endSpeed)
		{
			setMotorSync(motor_drive_left, motor_drive_right, -100, startSpeed+1.8*counter);
			wait1Msec(5);
			counter ++;
		}
		} else {
		while(startSpeed-1.8*counter > endSpeed)
		{
			setMotorSync(motor_drive_left, motor_drive_right, -100, startSpeed-1.8*counter);
			wait1Msec(5);
			counter ++;
		}
	}
	setMotorSync(motor_drive_left, motor_drive_right, -100, endSpeed);
}


//_driveCm
void driveCm(float leftSpeed, float rightSpeed, float distance)
{
	if(leftSpeed == rightSpeed)
	{
		setMotorSync(motor_drive_left, motor_drive_right, -100, leftSpeed);
	}
	setMotorSpeed(motor_drive_left, leftSpeed);
	setMotorSpeed(motor_drive_right, rightSpeed);
	waitUntil(abs(distance)/(tireDiameter*PI)*360 <= abs(getMotorEncoder(motor_drive_right)));
}


//_compareRGB
bool compareRGB(int redLow, int redHigh, int greenLow, int greenHigh, int blueLow, int blueHigh, int red, int green, int blue)
{
	bool correct = false;
	if(red > redLow && red < redHigh)
	{
		if(green > greenLow && green < greenHigh)
		{
			if(blue > blueLow && blue < blueHigh)
			{
				correct = true;
			}
		}
	}
	return correct;
}

// _rgb2hsv
void rgb2hsv(rgb in, hsv &out)
{
	float      min, max, delta;

	min = in.r < in.g ? in.r : in.g;
	min = min  < in.b ? min  : in.b;

	max = in.r > in.g ? in.r : in.g;
	max = max  > in.b ? max  : in.b;

	out.v = max;                                // v
	delta = max - min;
	if (delta < 0.00001)
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
	if( in.r >= max )                           // > is bogus, just keeps compilor happy
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

int hsvToColor(hsv in) {
	if ((in.s + in.v) < 15)
		return -1; // nothing
	if (in.h < 25 || in.h > 335)
		return 2; // red
	if (in.h >= 25)
		return 1; // yellow
	return 0; // basecase black
}


//_measureIndicators
task measureIndicators()
{
	rgb curr;
	rgb max;

	max.r = 0;
	max.g = 0;
	max.b = 0;

	while(true)
	{
		readSensor(&color_right);
		curr.r = (color_right.red+1);
		curr.g = (color_right.green+1);
		curr.b = (color_right.blue+1);
		if(curr.r > max.r)
		{
			max.r = curr.r;
		}
		if(curr.g > max.g)
		{
			max.g = curr.g;
		}
		if(curr.b > max.b)
		{
			max.b = curr.b;
		}
		displayTextLine(0, "R: %d %d %d", max.r, max.g, max.b);
		colors[side + 1] = (max.r + max.g + max.b) >  borderRGB;
		writeDebugStreamLine("[MAX R] %d", (max.r + max.g + max.b));
	}
}
//_measureIndicators_l
task measureIndicators_l()
{
	rgb curr;
	rgb max;

	max.r = 0;
	max.g = 0;
	max.b = 0;

	while(true)
	{
		readSensor(&color_left);
		curr.r = (color_left.red+1);
		curr.g = (color_left.green+1);
		curr.b = (color_left.blue+1);
		if(curr.r > max.r)
		{
			max.r = curr.r;
		}
		if(curr.g > max.g)
		{
			max.g = curr.g;
		}
		if(curr.b > max.b)
		{
			max.b = curr.b;
		}
		displayTextLine(1, "L: %d %d %d", max.r, max.g, max.b);
		colors[side] = (max.r + max.g + max.b) >  borderRGB;
		writeDebugStreamLine("[MAX L] %d ", (max.r + max.g + max.b));
	}
}

//_measureWashable_r
task measureWashable_r()
{
	rgb curr;
	rgb max;
	max.r = 0;
	max.g = 0;
	max.b = 0;

	while(true)
	{
		readSensor(&color_right);
		curr.r = (color_right.red+1);
		curr.g = (color_right.green+1);
		curr.b = (color_right.blue+1);
		if(curr.r > max.r)
		{
			max.r = curr.r;
		}
		if(curr.g > max.g)
		{
			max.g = curr.g;
		}
		if(curr.b > max.b)
		{
			max.b = curr.b;
		}
		displayTextLine(1, "L: %d %d %d", max.r, max.g, max.b);

		hsv res;
		rgb2hsv(max, res);

		washables[measureIndex] = hsvToColor(res);

	}
}
//_dropDrink()
task dropDrink() {
	dropped = false;
	setMotorTarget(motor_dropper, -200, 20);
	waitUntilMotorStop(motor_dropper);
	delay(500);
	setMotorTarget(motor_dropper, 0, 60);
	waitUntilMotorStop(motor_dropper);
	dropped = true;
}

// _reset_start
task reset_start()
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



// _pickupBottles
void pickupBottles() {
	turn(0, 40, 0, tireDistance/2, 125);
	setMotorTarget(motor_dropper, 140, 50);
	setMotorTarget(motor_grab, 435, 30);
	turn(0, 40, 0, -tireDistance/2, 172);

	setMotorTarget(motor_grab, 70, 20);
	waitUntilMotorStop(motor_grab);
	wait1Msec(200);
	setMotorTarget(motor_grab, 435, 30);
	waitUntilMotorStop(motor_grab);
	delay(200);

	resetMotorEncoder(motor_drive_right);
	driveCm(29, 30, 9.5);
	brake(30, 9.5);

	setMotorTarget(motor_grab, 70, 15);
	waitUntilMotorStop(motor_grab);
}
// _solve_side
void solve_side() {
	startTask(measureIndicators);
	startTask(measureIndicators_l);
	lfPDline(15, true, true);
	resetMotorEncoder(motor_drive_right);

	// ADJUST HERE -------------------------------------------------------------------------- >
	lfPDcm(15, 4);
	// ADJUST HERE FOR ~5mm of area next to grey table surrounding (long table direction) ---->

	setMotorTarget(motor_grab, 435, 30);
	stopTask(measureIndicators);
	stopTask(measureIndicators_l);

	turn(15, 40, 15, 21, 88.5);

	resetMotorEncoder(motor_drive_right);

	// if A side need drink
	if (colors[side + 1]) {
		startTask(measureWashable_r);
		brake(40, 2);
		stopTask(measureWashable_r);

		displayLogic();


		startTask(dropDrink);
		// if theres a washable
		if (washables[side] != -1) {
			setMotorTarget(motor_grab, 70, 30);
			waitUntilMotorStop(motor_grab);
			setMotorTarget(motor_grab, 120, 30);
			waitUntilMotorStop(motor_grab);
			setMotorTarget(motor_grab, 435, 20);
		}
		waitUntil(dropped);



		resetMotorEncoder(motor_drive_right);
		driveCm(-40, -40, 20.0);
	}
	else {
		startTask(measureWashable_r);
		driveCm(60, 60, 2.0);
		stopTask(measureWashable_r);
		driveCm(60, 60, 23.0);
		brake(60, 28.5);

		setMotorTarget(motor_grab, 135, 30);
		waitUntilMotorStop(motor_grab);
		delay(500);
		setMotorTarget(motor_grab, 435, 20);
		waitUntilMotorStop(motor_grab);

		// if washable
		resetMotorEncoder(motor_drive_right);
		if (washables[side] != -1) {
			driveCm(-40, -40, 21.5);
			brake(-40, 26.75);

			setMotorTarget(motor_grab, 70, 30);
			waitUntilMotorStop(motor_grab);
			setMotorTarget(motor_grab, 120, 30);
			waitUntilMotorStop(motor_grab);
		}
		driveCm(-37, -40, 32.0);
		setMotorTarget(motor_grab, 435, 20);
		driveCm(-40, -40, 47.0);
	}
	// B side needs drink
	resetMotorEncoder(motor_drive_right);
	if (colors[side]) {
		driveCm(-40, -40, 30.0);
		measureIndex = side + 1;
		startTask(measureWashable_r);
		brake(-40, 36.0);
		stopTask(measureWashable_r);
		displayLogic();

		startTask(dropDrink);
		// if theres a washable
		if (washables[side+1] != -1) {
			setMotorTarget(motor_grab, 70, 30);
			waitUntilMotorStop(motor_grab);
			setMotorTarget(motor_grab, 120, 30);
			waitUntilMotorStop(motor_grab);
			//setMotorTarget(motor_grab, 435, 20);
		}
		waitUntil(dropped);

		} else {
		// ball
		driveCm(-39, -40, 30.0);
		measureIndex = side + 1;
		startTask(measureWashable_r);
		driveCm(-40, -40, 36.0);
		stopTask(measureWashable_r);
		displayLogic();
		driveCm(-40, -40, 46.0);
		brake(-40, 51.0);
		resetMotorEncoder(motor_drive_right);
		setMotorTarget(motor_grab, 135, 30);
		waitUntilMotorStop(motor_grab);
		driveCm(10, 10, 2);
		brake(0, 0);
		delay(100);
		setMotorTarget(motor_grab, 435, 20);
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

		driveCm(40, 40, 16);
	}
	turn(40, 40, 0, 34, 88.0);
}
//_gotoSide2
void gotoSide2() {
	// person present next to start
	side = 2;
	if(true) {
		resetMotorEncoder(motor_drive_right);
		lfPDcm(15, 8);
		lfPDline(60, true, true);
		resetMotorEncoder(motor_drive_right);
		lfPDcm(60, 19);
		driveCm(60, 60, 43);
		brake(60, 48);

		turn(40, 60, 40, tireDistance/2, 25);
		turn(40, 60, 40, -40, 45);
		turn(40, 60, 40, tireDistance/2, 15);
		setMotorTarget(motor_grab, 435, 30);
		//resetMotorEncoder(motor_drive_right);
		//stopAllTasks();
		resetMotorEncoder(motor_drive_right);
		lfPDcm(30, 15);
		lfPDcm(15, 20);
		//lfPDline(15, true, true);
		//resetMotorEncoder(motor_drive_right);
		//startTask(measureIndicators);
		//startTask(measureIndicators_l);
		//// ADJUST HERE -------------------------------------------------------------------------- >
		//lfPDcm(15, 4);
		//// ADJUST HERE FOR ~5mm of area next to grey table surrounding (long table direction) ---->

		//setMotorTarget(motor_grab, 435, 30);
		//stopTask(measureIndicators);
		//stopTask(measureIndicators_l);
		//displayLogic();
		} else {
		resetMotorEncoder(motor_drive_right);
		lfPDcm(15, 8);
		lfPDline(60, true, true);
		resetMotorEncoder(motor_drive_right);
		lfPDcm(60, 19);
		driveCm(60, 60, 40);
		setMotorTarget(motor_grab, 435, 30);
		driveCm(60,60,90);
		resetMotorEncoder(motor_drive_right);
		lfPDcm(30, 15);
		lfPDcm(15, 20);
		//lfPDline(15, true, true);
		//resetMotorEncoder(motor_drive_right);
		//startTask(measureIndicators);
		//startTask(measureIndicators_l);
		//// ADJUST HERE -------------------------------------------------------------------------- >
		//lfPDcm(15, 4);
		//// ADJUST HERE FOR ~5mm of area next to grey table surrounding (long table direction) ---->

		//setMotorTarget(motor_grab, 435, 30);
		//stopTask(measureIndicators);
		//stopTask(measureIndicators_l);
		//displayLogic();
	}

}

// _main
task main()
{
	clearDebugStream();
	setMotorBrakeMode(motorA, motorBrake);
	setMotorBrakeMode(motorB, motorBrake);
	setMotorBrakeMode(motorC, motorBrake);
	setMotorBrakeMode(motorD, motorBrake);
	initSensor(&color_right, S4);
	initSensor(&color_left, S3);
	if (!readSensor(&color_right) || !readSensor(&color_left)) {
		displayTextLine(4, "SENSOR INIT ERROR!!");
		sleep(2000);
		stopAllTasks();
	}
	startTask(reset_start);
	eraseDisplay();
	waitUntil(reset);
	setLEDColor(ledGreen);
	waitForButtonPress();
	eraseDisplay();
	setLEDColor(ledOff);
	delay(150);

	//pickupBottles();


	//startTask(dropDrink);
	//waitUntil(dropped);
	//stopAllTasks()

	// unit test 2x side and path
	setMotorTarget(motor_grab, 435, 20);
	waitUntilMotorStop(motor_grab);
	delay(200);

	resetMotorEncoder(motor_drive_right);
	lfPDcm(15, 10);

	solve_side();
	gotoSide2();
	solve_side();

	brake(0, 0);
	delay(500);
	stopAllTasks();
}
