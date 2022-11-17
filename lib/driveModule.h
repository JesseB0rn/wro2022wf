float LF_P_a = 0.000065;
float LF_P_u = 45;
float LF_P_v = 0.13;
float LF_D_a = 0.0018;
float LF_D_u = 35;
float LF_D_v = 6.3;
float tireDiameter = 6.24;
float tireDistance = 17.20;
float brakeCons = 0.08;
float brakeConsTurn = 0.11;
bool loop_stop = false;

#define DR_P 0.15
#define DR_D 1.40

#define ACC 1.8
#define DCC 0.4

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
    if (speed == 15)
    {
        kP = 0.1;
        kD = 1.5;
    }
    else
    {
        kP = LF_P_a * pow(speed - LF_P_u, 2) + LF_P_v;
        kD = LF_D_a * pow(speed - LF_D_u, 2) + LF_D_v;
    }
    while (distance >= getMotorEncoder(motor_drive_right) / 360 * PI * tireDiameter)
    {
        value_left = getColorReflected(line_follower_left);
        value_right = getColorReflected(line_follower_right);
        error = value_left - value_right;
        sum = value_left + value_right;
        time = time1[timer1];
        clearTimer(timer1);
        derivative = error - lastError;
        correction = error * kP + derivative * kD / time;
        if (sum <= 55)
        {
            setMotorSpeed(motorB, speed);
            setMotorSpeed(motorC, speed);
        }
        else
        {
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
    if (speed == 15)
    {
        kP = 0.1;
        kD = 1.5;
    }
    else
    {
        kP = LF_P_a * pow(speed - LF_P_u, 2) + LF_P_v;
        kD = LF_D_a * pow(speed - LF_D_u, 2) + LF_D_v;
    }
    while (loop_stop == false)
    {
        value_left = getColorReflected(line_follower_left);
        value_right = getColorReflected(line_follower_right);
        error = value_left - value_right;
        sum = value_left + value_right;
        time = time1[timer1];
        clearTimer(timer1);
        derivative = error - lastError;
        correction = error * kP + derivative * kD / time;
        if (sum <= 55)
        {
            setMotorSpeed(motorB, speed);
            setMotorSpeed(motorC, speed);
        }
        else
        {
            setMotorSpeed(motorB, speed + correction);
            setMotorSpeed(motorC, speed - correction);
        }
        lastError = error;
        if (sensor1 == true && sensor4 == true)
        {
            loop_stop = (sum <= 40);
        }
        else
        {
            if (sensor1 == true)
            {
                loop_stop = value_right <= 15;
            }
            else
            {
                loop_stop = value_left <= 15;
            }
        }
        waitUntil(time1[timer1] >= 5);
    }
}

//_lfPDend
void lfPDend(float speed)
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
    if (speed == 15)
    {
        kP = 0.1;
        kD = 1.5;
    }
    else
    {
        kP = LF_P_a * pow(speed - LF_P_u, 2) + LF_P_v;
        kD = LF_D_a * pow(speed - LF_D_u, 2) + LF_D_v;
    }
    while (loop_stop == false)
    {
        value_left = getColorReflected(line_follower_left);
        value_right = getColorReflected(line_follower_right);
        error = value_left - value_right;
        sum = value_left + value_right;
        time = time1[timer1];
        clearTimer(timer1);
        derivative = error - lastError;
        correction = error * kP + derivative * kD / time;

        setMotorSpeed(motorB, speed + correction);
        setMotorSpeed(motorC, speed - correction);
        lastError = error;

        loop_stop = (sum >= 120);

        waitUntil(time1[timer1] >= 5);
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
    float degrees = abs(2 * PI * (AbsRadius + tireDistance / 2) / (tireDiameter * PI) * angle);
    float left_rightRatio = (AbsRadius - tireDistance / 2) / (AbsRadius + tireDistance / 2);
    resetMotorEncoder(motor_drive_left);
    resetMotorEncoder(motor_drive_right);
    if (radius < 0)
    {
        if (speed1 <= speed2)
        {
            while (speed1 + 1.8 * counter < speed2)
            {
                rotataionsB = getMotorEncoder(motor_drive_left) * (-1);
                rotataionsC = getMotorEncoder(motor_drive_right);
                error = abs(rotataionsC * left_rightRatio) - abs(rotataionsB);
                Correction = error * kP;
                setMotorSpeed(motor_drive_left, (speed1 + 1.8 * counter) * left_rightRatio * (1 + Correction));
                setMotorSpeed(motor_drive_right, (speed1 + 1.8 * counter) * (1 - Correction));
                wait1Msec(5);
                counter++;
            }
        }
        else
        {
            while (speed1 - 1.8 * counter > speed2)
            {
                rotataionsB = getMotorEncoder(motor_drive_left) * (-1);
                rotataionsC = getMotorEncoder(motor_drive_right);
                error = abs(rotataionsC * left_rightRatio) - abs(rotataionsB);
                Correction = error * kP;
                setMotorSpeed(motor_drive_left, (speed1 - 1.8 * counter) * left_rightRatio * (1 + Correction));
                setMotorSpeed(motor_drive_right, (speed1 - 1.8 * counter) * (1 - Correction));
                wait1Msec(5);
                counter++;
            }
        }
        if (speed3 < speed2)
        {
            while (degrees - abs(((speed2 - speed3) * (speed2 + speed3)) / 2 * brakeConsTurn) > abs(getMotorEncoder(motor_drive_right)))
            {
                rotataionsB = getMotorEncoder(motor_drive_left) * (-1);
                rotataionsC = getMotorEncoder(motor_drive_right);
                error = abs(rotataionsC * left_rightRatio) - abs(rotataionsB);
                Derivative = error - lastError;
                Correction = error * kP + Derivative * kD;
                setMotorSpeed(motor_drive_left, speed2 * left_rightRatio * (1 + Correction));
                setMotorSpeed(motor_drive_right, speed2 * (1 - Correction));
                lastError = error;
            }
            while (degrees - abs(getMotorEncoder(motor_drive_right)) > 3)
            {
                speedBrake = sqrt(2 * (degrees - abs(getMotorEncoder(motor_drive_right))) / brakeConsTurn + pow(speed3, 2)) * speed2 / abs(speed2);
                rotataionsB = getMotorEncoder(motor_drive_left) * (-1);
                rotataionsC = getMotorEncoder(motor_drive_right);
                setMotorSpeed(motor_drive_left, speedBrake * left_rightRatio);
                setMotorSpeed(motor_drive_right, speedBrake);
            }
        }
        else
        {
            while (abs(getMotorEncoder(motor_drive_right)) <= degrees)
            {
                rotataionsB = getMotorEncoder(motor_drive_left) * (-1);
                rotataionsC = getMotorEncoder(motor_drive_right);
                error = abs(rotataionsC * left_rightRatio) - abs(rotataionsB);
                Derivative = error - lastError;
                Correction = error * kP + Derivative * kD;
                setMotorSpeed(motor_drive_left, speed2 * left_rightRatio * (1 + Correction));
                setMotorSpeed(motor_drive_right, speed2 * (1 - Correction));
                lastError = error;
            }
        }
    }
    else
    {
        if (speed1 <= speed2)
        {
            while (speed1 + 1.8 * counter < speed2)
            {
                rotataionsB = getMotorEncoder(motor_drive_left) * (-1);
                rotataionsC = getMotorEncoder(motor_drive_right);
                error = abs(rotataionsB * left_rightRatio) - abs(rotataionsC);
                Correction = error * kP;
                setMotorSpeed(motor_drive_left, (speed1 + 1.8 * counter) * (1 - Correction));
                setMotorSpeed(motor_drive_right, (speed1 + 1.8 * counter) * left_rightRatio * (1 + Correction));
                wait1Msec(5);
                counter++;
            }
        }
        else
        {
            while (speed1 - 1.8 * counter > speed2)
            {
                rotataionsB = getMotorEncoder(motor_drive_left) * (-1);
                rotataionsC = getMotorEncoder(motor_drive_right);
                error = abs(rotataionsB * left_rightRatio) - abs(rotataionsC);
                Correction = error * kP;
                setMotorSpeed(motor_drive_left, (speed1 - 1.8 * counter) * (1 - Correction));
                setMotorSpeed(motor_drive_right, (speed1 - 1.8 * counter) * left_rightRatio * (1 + Correction));
                wait1Msec(5);
                counter++;
            }
        }
        if (speed3 < speed2)
        {
            while (degrees - abs(((speed2 - speed3) * (speed2 + speed3)) / 2 * brakeConsTurn) > abs(getMotorEncoder(motor_drive_left)))
            {
                rotataionsB = getMotorEncoder(motor_drive_left) * (-1);
                rotataionsC = getMotorEncoder(motor_drive_right);
                error = abs(rotataionsB * left_rightRatio) - abs(rotataionsC);
                Derivative = error - lastError;
                Correction = error * kP + Derivative * kD;
                setMotorSpeed(motor_drive_left, speed2 * (1 - Correction));
                setMotorSpeed(motor_drive_right, speed2 * left_rightRatio * (1 + Correction));
                lastError = error;
            }
            while (degrees - abs(getMotorEncoder(motor_drive_left)) > 3)
            {
                speedBrake = sqrt(2 * (degrees - abs(getMotorEncoder(motor_drive_left))) / brakeConsTurn + pow(speed3, 2)) * speed2 / abs(speed2);
                rotataionsB = getMotorEncoder(motor_drive_left) * (-1);
                rotataionsC = getMotorEncoder(motor_drive_right);
                setMotorSpeed(motor_drive_left, speedBrake);
                setMotorSpeed(motor_drive_right, speedBrake * left_rightRatio);
            }
        }
        else
        {
            while (abs(getMotorEncoder(motor_drive_left)) <= degrees)
            {
                rotataionsB = getMotorEncoder(motor_drive_left) * (-1);
                rotataionsC = getMotorEncoder(motor_drive_right);
                error = abs(rotataionsB * left_rightRatio) - abs(rotataionsC);
                Derivative = error - lastError;
                Correction = error * kP + Derivative * kD;
                setMotorSpeed(motor_drive_left, speed2 * (1 - Correction));
                setMotorSpeed(motor_drive_right, speed2 * left_rightRatio * (1 + Correction));
                lastError = error;
            }
        }
    }
    if (speed3 == 0)
    {
        setMotorSpeed(motor_drive_left, 0);
        setMotorSpeed(motor_drive_right, 0);
    }
}

//_brake
void brake(float speed, float distance)
{
    float degrees = distance / (tireDiameter * PI) * 360;
    float speedBrake = speed;
    setMotorSync(motor_drive_left, motor_drive_right, -100, speed);
    waitUntil(abs(degrees) - abs((speed * (speed + 1)) / 2 * brakeCons) <= abs(getMotorEncoder(motor_drive_right)));
    while (abs(degrees) - abs(getMotorEncoder(motor_drive_right)) > 2)
    {
        speedBrake = (-1 + sqrt(1 + 8 * (degrees - abs(getMotorEncoder(motor_drive_right))) / brakeCons)) / 2 * speed / abs(speed);
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
    if (startSpeed <= endSpeed)
    {
        while (startSpeed + 1.8 * counter < endSpeed)
        {
            setMotorSync(motor_drive_left, motor_drive_right, -100, startSpeed + 1.8 * counter);
            wait1Msec(5);
            counter++;
        }
    }
    else
    {
        while (startSpeed - 1.8 * counter > endSpeed)
        {
            setMotorSync(motor_drive_left, motor_drive_right, -100, startSpeed - 1.8 * counter);
            wait1Msec(5);
            counter++;
        }
    }
    setMotorSync(motor_drive_left, motor_drive_right, -100, endSpeed);
}

//_driveCm
void driveCm(float leftSpeed, float rightSpeed, float distance)
{
    if (leftSpeed == rightSpeed)
    {
        setMotorSync(motor_drive_left, motor_drive_right, -100, leftSpeed);
    }
    setMotorSpeed(motor_drive_left, leftSpeed);
    setMotorSpeed(motor_drive_right, rightSpeed);
    waitUntil(abs(distance) / (tireDiameter * PI) * 360 <= abs(getMotorEncoder(motor_drive_right)));
}
//_driveMs
void driveMs(float leftSpeed, float rightSpeed, int ms)
{
    if (leftSpeed == rightSpeed)
    {
        setMotorSync(motor_drive_left, motor_drive_right, -100, leftSpeed);
    }
    setMotorSpeed(motor_drive_left, leftSpeed);
    setMotorSpeed(motor_drive_right, rightSpeed);
    delay(ms);
}
// ----- regulated x transfer

float renc_l, renc_r, delta;

float xt_getEncoderL() {
	return (-getMotorEncoder(motor_drive_left))-renc_l;
}
float xt_getEncoderR() {
	return getMotorEncoder(motor_drive_right)-renc_r;
}

void xt_reset_drive() {
	delta = xt_getEncoderL() - xt_getEncoderR();
	//if (!full)
	//	return;
 //   renc_l = xt_getEncoderL();
 //   renc_r = xt_getEncoderR();
}

void xt__drive(float vl, float vr, float &lerr) {
	float err;

	err = (xt_getEncoderL()*(vl/vr)) - xt_getEncoderR()*(vr/vl) - delta;
	float corr = err * DR_P + (err-lerr) * DR_D;
	setMotorSpeed(motor_drive_left, (vl*(vl/vr))-corr);
	setMotorSpeed(motor_drive_right, (vr*(vr/vl))+corr);
	lerr = err;
	//writeDebugStreamLine("%f", err);
}
void xt_drive(float vm, float dist, float curva = 0.98) {
	float lerr;
	xt_reset_drive();
	while (abs(dist) / (tireDiameter * PI) * 360 >= abs(getMotorEncoder(motor_drive_right))) {
		xt__drive(vm*curva, vm, lerr);
	}
}
