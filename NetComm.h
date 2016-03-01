/*
 * NetComm.h
 * Network communication library
 *
 *  Created on: Mar 6, 2015
 *      Author: Anirudh Bagde
 */

#ifndef ROBOT_2015_NETCOMM_H_
#define ROBOT_2015_NETCOMM_H_

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
};*/

/*struct __attribute__((__packed__)) CommData {
    uint8_t  id;
    uint8_t  val;
    uint16_t crc16;
};*/

struct __attribute__((__packed__)) PingData {
    uint8_t pingValue;
};

struct ControlData {
    uint8_t id;
    uint8_t val;
    int8_t  dpadY;
    int8_t  dpadX;
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

#endif /* ROBOT_2015_NETCOMM_H_ */
