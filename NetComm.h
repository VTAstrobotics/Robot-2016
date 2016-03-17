/*
 * NetComm.h
 * Network communication library
 *
 *  Created on: Mar 6, 2015
 *      Author: Anirudh Bagde
 */

#ifndef ROBOT_2016_NETCOMM_H_
#define ROBOT_2016_NETCOMM_H_

#include <stdint.h>
#include <net/if.h>

#define PING_ENABLED 0

const int NETCOMM_RECVPORT = 6800;
const int NETCOMM_PINGPORT = 6900;
const int NETCOMM_PINGVALUE = 216;
const double PING_TIMEOUT = 3;     // in seconds

// Little-endian on Galileo (x86)
/*struct __attribute__((__packed__)) Joystick {
    float thumbLX;
    float thumbLY;
    float thumbRX;
    float thumbRY;
    float triggerL;
    float triggerR;
    unsigned int btnA :1;
    unsigned int btnB :1;
    unsigned int btnX :1;
    unsigned int btnY :1;
    unsigned int btnLB :1;
    unsigned int btnRB :1;
    unsigned int btnBack :1;
    unsigned int btnStart :1;
    unsigned int btnThumbL :1;
    unsigned int btnThumbR :1;
    unsigned int btnXbox :1;
    signed int dpadX :2;
    signed int dpadY :2;
};

struct __attribute__((__packed__)) ControlData {
    Joystick joy1;
    unsigned short crc16;
};

struct __attribute__((__packed__)) CommData {
    uint8_t  id;
    uint8_t  val;
    uint16_t crc16;
};
*/
struct __attribute__((__packed__)) PingData {
    uint8_t pingValue;
};

struct __attribute__((__packed__)) ControlData {
	float LX;
	float LY;
	float RX;
	float RY;
	float trigL;
	float trigR;

	bool button_a;
	bool button_b;
	bool button_x;
	bool button_y;
	bool bumper_l;
	bool bumper_r;
	bool back;
	bool start;
	bool thumb_l;
	bool thumb_r;

	int dpad_x;
	int dpad_y;
};

struct __attribute__((__packed__)) CommData {
	// axes, all signed
	int8_t LX;
	int8_t LY;
	int8_t RX;
	int8_t RY;
	int8_t trigL;
	int8_t trigR;

	// buttons, unsigned (0 or 1)
	uint8_t button_a :1;
	uint8_t button_b :1;
	uint8_t button_x :1;
	uint8_t button_y :1;
	uint8_t bumper_l :1;
	uint8_t bumper_r :1;
	uint8_t back     :1;
	uint8_t start    :1;
	uint8_t thumb_l  :1;
	uint8_t thumb_r  :1;
	uint8_t unused   :6;

	// DPad
	uint8_t x_pos :2;
	uint8_t x_neg :2;
	uint8_t y_pos :2;
	uint8_t y_neg :2;

	uint16_t crc16;
};

class NetComm {
public:
    NetComm();
    ~NetComm();

    bool getData(ControlData* data);
    bool isNetworkUp();

private:
    int recvSock;

    int pingSock;
    bool pingReceived;
    double lastPingTime;

    ifreq ifr; // For detecting if network interface is up

    void sendPing();
};

#endif
