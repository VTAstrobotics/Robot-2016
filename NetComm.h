/*
 * NetComm.h
 * Network communication library
 *
 *  Created on: Mar 6, 2015
 *      Authors: Anirudh Bagde and Matthew Conner
 */

#ifndef ROBOT_2016_NETCOMM_H_
#define ROBOT_2016_NETCOMM_H_

#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#define PING_ENABLED 1

const int NETCOMM_RECVPORT = 6800;
const int NETCOMM_PINGPORT = 6900;
const int NETCOMM_PINGVALUE = 216;
const int NETCOMM_SENDPORT = 6850;

const char* const IP_send = "10.0.0.25";

const double PING_TIMEOUT = 0.6;     // in seconds


struct __attribute__((__packed__)) PingData {
    uint8_t pingValue;
};

struct __attribute__((__packed__)) ControlData {
    //data used by robot
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
    //raw data from DS
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

struct __attribute__((__packed__)) CommSend {

    uint8_t padding :7;
    uint8_t deadman :1;
    uint8_t battery;
    uint16_t crc16;

};

class NetComm {
public:
    NetComm();
    ~NetComm();

    bool getData(ControlData* data);
    bool isNetworkUp();
    bool sendData(bool dead, float battery);

private:
    int recvSock;

    int pingSock;

    int sendSock;

    bool pingReceived;
    double lastPingTime;

    ifreq ifr; // For detecting if network interface is up

    bool currentDead;
    float currentBatt;

    sockaddr_in dest; //destination socket for output
    void sendPing();
    inline int sendSocket(int port);
};

#endif
