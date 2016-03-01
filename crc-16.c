#include "crc-16.h"

unsigned short crc16(unsigned char* data_p, unsigned short length) {
    unsigned char x;
    unsigned short crc = 0xFFFF;

    while(length--) {
        x = crc >> 8 ^ *data_p++;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((unsigned short) (x << 12))
                ^ ((unsigned short) (x << 5)) ^ ((unsigned short) x);
    }
    return crc;
}
