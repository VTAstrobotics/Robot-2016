/*
 * NetComm.cpp
 *
 *  Created on: Mar 6, 2015
 *      Author: Anirudh Bagde
 */

#include "NetComm.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include "crc-16.h"
#include "common.h"

inline int bindSocket(int port) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    int reuse = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0
    addr.sin_port = htons(port);
    bind(sock, (sockaddr*) &addr, sizeof(addr));
    fcntl(sock, F_SETFL, O_NONBLOCK);
    return sock;
}

NetComm::NetComm() : pingReceived(true), lastPingTime(getCurrentSeconds()) {
    // Initialize sockets
    recvSock = bindSocket(NETCOMM_RECVPORT);
    pingSock = bindSocket(NETCOMM_PINGPORT);

    // Initialize network interface structure
    bzero(&ifr, sizeof(ifr));
    strcpy(ifr.ifr_name, "wlan0");
}

NetComm::~NetComm() {
    close(recvSock);
}

bool NetComm::getData(ControlData* data) {
    // rawData represents CommData
    // data represents ControlData
    CommData rawData;
    int size = sizeof(CommData);
    //memset(data, 0, sizeof(ControlData));
    memset(&rawData, 0, size);

    // Receive from network
    int len = recvfrom(recvSock, &rawData, size, 0, NULL, NULL);
    if(len < size) {
        return false;
    }

    // Do CRC check
    unsigned short crc16Check = crc16((unsigned char*) &rawData,
            sizeof(CommData) - sizeof(unsigned short));
    if(crc16Check != rawData.crc16) {
        return false;
    }

    // Axes
    float conversion;
    conversion = ((float) rawData.LX) / 100;
    data->LX = conversion;
    conversion = ((float) rawData.LY) / 100;
    data->LY = conversion;
    conversion = ((float) rawData.RX) / 100;
    data->RX = conversion;
    conversion = ((float) rawData.RY) / 100;
    data->RY = conversion;
    conversion = ((float) rawData.trigL) / 100;
    data->trigL = conversion;
    conversion = ((float) rawData.trigR) / 100;
    data->trigR = conversion;

    // Buttons
    data->button_a = rawData.button_a;
    data->button_b = rawData.button_b;
    data->button_x = rawData.button_x;
    data->button_y = rawData.button_y;
    data->bumper_l = rawData.bumper_l;
    data->bumper_r = rawData.bumper_r;
    data->back = rawData.back;
    data->start = rawData.start;
    data->thumb_l = rawData.thumb_l;
    data->thumb_r = rawData.thumb_r;

    // DPad
    int temp = 0;
    if(rawData.x_pos == 1) {
        temp = 1;
    } else if(rawData.x_neg == 1) {
        temp = -1;
    }
    data->dpad_x = temp;
    temp = 0;
    if(rawData.x_pos == 1) {
        temp = 1;
    } else if(rawData.x_neg == 1) {
        temp = -1;
    }
    data->dpad_y = temp;

    return true;
}

bool NetComm::sendData() {
    code;
    deadman;
    battery;
    crc16;

}
bool NetComm::isNetworkUp() {
    // Check interface status first
    if(ioctl(recvSock, SIOCGIFFLAGS, &ifr) != -1) {
        int check_flags = IFF_UP | IFF_RUNNING;
        if((ifr.ifr_flags & check_flags) != check_flags) {
            return false;
        }
    }

#if PING_ENABLED
    // Check if last ping was received
    PingData pingData;
    int size = sizeof(PingData);
    bzero(&pingData, size);
    int len = recvfrom(pingSock, &pingData, size, 0, NULL, NULL);
    if(len == size && pingData.pingValue == NETCOMM_PINGVALUE) {
        // Ping respond is valid, so return true
        pingReceived = true;
        lastPingTime = getCurrentSeconds();
    } else if(getCurrentSeconds() - lastPingTime >= PING_TIMEOUT) {
        // Ping timed out, so return false
        pingReceived = false;
    }
#endif

    // Return last status of ping
    return pingReceived;
}
