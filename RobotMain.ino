#include "NetComm.h"
#include "crc-16.h"
#include <Servo.h>
#include <Arduino.h>
#include <Astrobotics_Shield_Header.h>
#include "common.h"

const int LOOP_HZ = 50;
const int LOOP_DELAY = (int) (1000 / LOOP_HZ);
const int READY_LED = 12;
const int ACTIVE_LED = 11;

// FIXME Using 2016 Kent State pin configuration for driving

PWMTalon driveLeft;
PWMTalon driveRight;
NetComm comm;
ControlData control;
bool dead = true;
float leftRatio;
float rightRatio;

void printData(ControlData& data) {
//    char out[512];
//    sprintf(out, "A: %d\nB: %d\nX: %d\nY: %d\n", data.button_a, data.button_b, data.button_x, data.button_y);
//    Serial.println(out);
}

void killMotors() {
    driveLeft.set_speed(0.0f);
    driveRight.set_speed(0.0f);
    dead = true;
}

void motorControl(ControlData& data) {
    // Update state
    // TODO Some way of using constants to define controls

    // Deadman is left bumper
    // Pressed = alive, released = dead
    dead = !data.bumper_l;
    digitalWrite(ACTIVE_LED, !dead); // Turn on when alive
    if(dead) {
        killMotors();
        return;
    }

    // Drive control, left and right control sticks
    //implementing tank drive here
//    if (data.LX < 0) { //determines ratio
//        leftRatio = data.LX;
//        rightRatio = -data.LX;
//    }
//    else {
//        rightRatio = data.LX;
//        leftRatio = -data.LX;
//    }
//    leftRatio += data.RY;
//    rightRatio += data.RY;
//    leftRatio = leftRatio / fmax(abs(leftRatio), abs(rightRatio));
//    rightRatio = leftRatio / fmax(abs(leftRatio), abs(rightRatio));

    // New tank drive
    if(data.RY > 0) {
        if(data.LX > 0) {
            leftRatio = data.RY - data.LX;
            rightRatio = fmax(data.RY, data.LX);
        } else {
            leftRatio = fmax(data.RY, -data.LX);
            rightRatio = data.RY + data.LX;
        }
    } else {
        if(data.LX > 0) {
            leftRatio = -fmax(-data.RY, data.LX);
            rightRatio = data.RY + data.LX;
        } else {
            leftRatio = data.RY - data.LX;
            rightRatio = -fmax(-data.RY, -data.LX);
        }
    }

    driveLeft.set_speed(leftRatio);
    driveRight.set_speed(rightRatio);
    char out[64];
    sprintf(out, "left drive: %f\n\rright drive: %f", leftRatio, rightRatio);
    Serial.println(out);
}

void check_connected() {
    if(comm.isNetworkUp()) {
        digitalWrite(READY_LED, HIGH);
    } else {
        Serial.println("Ping timed out!");
        digitalWrite(READY_LED, LOW);
        killMotors();
    }
}

void setup() {
    Serial.begin(9600);
    // Initialize default values for control
    memset(&control, 0, sizeof(control));
    // Activate LED to indicate readiness
    pinMode(READY_LED, OUTPUT);
    pinMode(ACTIVE_LED, OUTPUT);
    // Initialize motor controllers
    PWMTalon::talon_init();
    driveLeft.attach(LEFT_DRIVE_PIN, true);
    driveRight.attach(RIGHT_DRIVE_PIN);
    killMotors();
}

void loop() {
    check_connected();
    if(comm.getData(&control)) {
        if(!dead) {
            printData(control);
        }
        motorControl(control);
    } else {
        if(dead) {
            Serial.println("dead");
        }
        delay(LOOP_DELAY);
    }
}
