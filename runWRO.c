#pragma config(Sensor, S1,     line_follower_left, sensorEV3_Color)
#pragma config(Sensor, S2,     line_follower_right, sensorEV3_Color)
#pragma config(Motor,  motorA,          motor_lift,    tmotorEV3_Medium, PIDControl, encoder)
#pragma config(Motor,  motorB,          motor_drive_left, tmotorEV3_Medium, PIDControl, reversed, driveLeft, encoder)
#pragma config(Motor,  motorC,          motor_drive_right, tmotorEV3_Medium, PIDControl, driveRight, encoder)
#pragma config(Motor,  motorD,          motor_grab,    tmotorEV3_Medium, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "lib/driveModule.h"

#pragma debuggerWindows("debugStream");

#define color_left S3
#define color_right S4

#define borderW 90

bool reset = false;

// _resetStart
task resetStart()
{
	setMotorSpeed(motor_lift, 25);
	setMotorSpeed(motor_grab, -25);
	wait1Msec(2000);
	setMotorSpeed(motor_lift, 0);
	setMotorSpeed(motor_grab, 0);
	wait1Msec(200);
	resetMotorEncoder(motor_grab);
	resetMotorEncoder(motor_lift);
	playTone(440, 10);
	reset = true;
}




// _main
task main()
{
	clearDebugStream();
	setMotorBrakeMode(motorA, motorBrake);
	setMotorBrakeMode(motorB, motorBrake);
	setMotorBrakeMode(motorC, motorBrake);
	setMotorBrakeMode(motorD, motorBrake);
	startTask(resetStart);
	eraseDisplay();
	waitUntil(reset);
	setLEDColor(ledGreen);
	waitForButtonPress();
	eraseDisplay();
	setLEDColor(ledOff);
	delay(150);
	clearTimer(T4);

	setMotorTarget(motor_lift, -185, 20);
	setMotorTarget(motor_grab, 160, 50);
	waitUntilMotorStop(motor_lift);

	driveCm(30, 30, 10);
	brake(0,0);
	setMotorTarget(motor_grab, 60, 50);

	delay(5000);

	setMotorSpeed(motor_grab, -5);
	delay(250);

	setMotorTarget(motor_lift, -5, 20);
	waitUntilMotorStop(motor_lift);

	brake(0, 0);
	eraseDisplay();
	displayCenteredBigTextLine(4, "%d.0 s", (float)time100[T4]/10.0);
	writeDebugStreamLine("TIME: %d", (float)time100[T4]/10.0);
	delay(10000);
	stopAllTasks();
}
