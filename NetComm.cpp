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

    // Initalize network interface structure
    bzero(&ifr, sizeof(ifr));
    strcpy(ifr.ifr_name, "wlan0");
}

NetComm::~NetComm() {
    close(recvSock);
}

bool NetComm::getData(ControlData* data) {
    // TODO implement
/*    CommData rawData;
    int size = sizeof(CommData);
    bzero(data, sizeof(ControlData));
    bzero(&rawData, size);
    int len = recvfrom(recvSock, &rawData, size, 0, NULL, NULL);
    if (len < size) {
        return false;
    }
    unsigned short crc16Check = crc16((unsigned char*) &rawData,
            sizeof(CommData) - sizeof(unsigned short));
    if(crc16Check != rawData.crc16) {
        return false;
    }
    data->id = rawData.id;
    data->val = rawData.val;
    unsigned int dpad = rawData.val;
    if(dpad & (1<<6)) {
        data->dpadY = 1;
    } else if(dpad & (1<<4)) {
        data->dpadY = -1;
    } else if(dpad & (1<<2)) {
        data->dpadX = 1;
    } else if(dpad & 1) {
        data->dpadX = -1;
    }
    return true;*/
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
