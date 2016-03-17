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

// Motor controllers
//const int LEFT_DRIVE_PIN  = 0;

//PWMTalon RIGHT_DRIVE_CONTROLLER;
NetComm comm;
ControlData control;
bool dead = true;

void printData(ControlData& data) {
    char out[64];
/*    if(data.id == 17) {
        sprintf(out, "DPAD, Y: %d, X: %d", data.dpadY, data.dpadX);
    } else {
        sprintf(out, "Data update, id: %d, value: %d", data.id, data.val);
    }*/
    Serial.println(out);
}

void killMotors() {
    // TODO tell all motors to reset to 0.0
    dead = true;
}

void motorControl(ControlData& data) {
    // Update state
/*    if(data.id == DEADMAN) {
        // Pressed = active, released = dead
        dead = !data.val;
        digitalWrite(ACTIVE_LED, data.val);
        if(dead) {
            killMotors();
        }
    }*/
    if(dead) {
        return;
    }
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
    bzero(&control, sizeof(control));
    // Activate LED to indicate readiness
    pinMode(READY_LED, OUTPUT);
    pinMode(ACTIVE_LED, OUTPUT);
    // Initialize motor controllers
    PWMTalon::talon_init();
//    LEFT_DRIVE_CONTROLLER.attach(LEFT_DRIVE_PIN, true);
//    RIGHT_DRIVE_CONTROLLER.attach(RIGHT_DRIVE_PIN);
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
