/*
	Written by Jesse "Jesster" Born on 10.11.22 because he was severly pissed of at Xander Soldaat (@botbench)
*/
#pragma systemFile

typedef struct {
	int color;
	float r,g,b, w;
} rgbw;
bool waitForI2CBus(tSensors link)
{
	while (true)
	{
		TI2CStatus i2cstatus = nI2CStatus[link];
		//writeDebugStreamLine("nI2CStatus[%d]: %d", link, i2cstatus);
		switch(i2cstatus) {
		case i2cStatusStopped:
		case i2cStatusNoError:
			return true;

		case i2cStatusPending:
		case i2cStatusStartTransfer:
			break;

		case i2cStatusFailed:
		case i2cStatusBadConfig:
			sleep(1);
		}
	}
	return false;
}
void getRGBW(tSensors port, rgbw &res) {
	ubyte rpl[5];
	byte msg[3] = {2, 0x02, 0x42};
	sendI2CMsg(port, &msg[0], 5);
	waitForI2CBus(port);
	readI2CReply(port, &rpl[0], 5);
	//for (int i = 0; i < 5; i++) {
	//	writeDebugStream("%d ", (short)rpl[i]);
	//}
	//writeDebugStream("\n");

	res.color = rpl[0];
	res.r = (short)rpl[1];
	res.g = (short)rpl[2];
	res.b = (short)rpl[3];
	res.w = (short)rpl[4];
}
/**
	Check for correct hardware spec
**/
bool verifySetupHTCOL(tSensors port) {
	char reply[32];
	byte msg[3] = {2, 0x02, 0x00};
	sendI2CMsg(port, &msg[0], 32);
	waitForI2CBus(port);
	readI2CReply(port, &reply[0], 32);
	writeDebugStream("[VERIFY SENSOR] ");
	for (int i = 0; i < 32; i++) {
		writeDebugStream("%c", reply[i]);
	}
	writeDebugStream("\n");
	return true;
}
/**
	Chooses max of each component
**/
void rgbwMaxComponent(rgbw &max, rgbw &curr) {
		max.r = max2(max.r, curr.r);
		max.g = max2(max.g, curr.g);
		max.b = max2(max.b, curr.b);
		max.w = max2(max.w, curr.w);
}

/**
	Chooses max of each component
**/
void rgbwMax(rgbw &max, rgbw &curr) {
		if ((max.r+max.g+max.b) <= (curr.r+curr.g+curr.b)) {
			max.r = curr.r;
			max.g = curr.g;
			max.b = curr.b;
			max.w = curr.w;
		}
}
