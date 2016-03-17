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

void printData(ControlData& data) {
//    char out[64];
//    if(data.id == 17) {
//        sprintf(out, "DPAD, Y: %d, X: %d", data.dpadY, data.dpadX);
//    } else {
//        sprintf(out, "Data update, id: %d, value: %d", data.id, data.val);
//    }
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
    driveLeft.set_speed(data.LY);
    driveRight.set_speed(data.RY);
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
