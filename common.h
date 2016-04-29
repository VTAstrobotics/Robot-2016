/*
 * constants.h
 * Defines common constants, including controller constants and button/axis assignments
 *
 *  Created on: Apr 10, 2015
 *      Author: Anirudh Bagde
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <time.h>

// Pin configurations
static const int LEFT_DRIVE_PIN   = 1;
static const int RIGHT_DRIVE_PIN  = 3;
static const int BUCKET_ARM_PIN   = 5;
static const int BUCKET_ANGLE_PIN = 7;
static const int BED_ELEV_1_PIN   = 9;
static const int BED_ELEV_2_PIN   = 11;

// Xbox controller IDs
static const int LTHUMBX   = 0;
static const int LTHUMBY   = 1;
static const int RTHUMBX   = 2;
static const int RTHUMBY   = 3;
static const int RTRIGGER  = 4;
static const int LTRIGGER  = 5;
static const int A         = 6;
static const int B         = 7;
static const int X         = 8;
static const int Y         = 9;
static const int LB        = 10;
static const int RB        = 11;
static const int BACK      = 12;
static const int START     = 13;
static const int XBOX      = 14;
static const int LTHUMBBTN = 15;
static const int RTHUMBBTN = 16;
static const int DPAD      = 17;
// L2 and R2 are for controllers that have buttons instead of triggers
static const int L2        = 18;
static const int R2        = 19;

// Specific controls
// Axes
// Buttons

// Helper functions
inline double getSeconds(timespec time) {
    return time.tv_sec + (time.tv_nsec / 1.0e9);
}

inline double getCurrentSeconds() {
    timespec time;
    bzero(&time, sizeof(time));
    clock_gettime(CLOCK_MONOTONIC, &time);
    return getSeconds(time);
}

#endif /* COMMON_H_ */
