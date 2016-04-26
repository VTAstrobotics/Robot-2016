#include "NetComm.h"
#include "crc-16.h"
#include <Servo.h>
#include <Arduino.h>
#include <Astrobotics_Shield_Header.h>
#include "common.h"

const int LOOP_HZ = 50;
const int LOOP_DELAY = (int) (1000 / LOOP_HZ);
const int READY_LED = 12;
const int ACTIVE_LED = 13;

// FIXME Using 2016 Kent State pin configuration for driving

PWMTalon driveLeft;
PWMTalon driveRight;
PWMTalon bucketAngle;
PWMTalon bucketArm;

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
    bucketAngle.set_speed(0.0f);
    bucketArm.set_speed(0.0f);
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

    // New tank drive
    if(data.LY > 0) {
        if(data.RX > 0) {
            leftRatio = data.LY - data.RX;
            rightRatio = fmax(data.LY, data.RX);
        } else {
            leftRatio = fmax(data.LY, -data.RX);
            rightRatio = data.LY + data.RX;
        }
    } else {
        if(data.RX > 0) {
            leftRatio = -fmax(-data.LY, data.RX);
            rightRatio = data.LY + data.RX;
        } else {
            leftRatio = data.LY - data.RX;
            rightRatio = -fmax(-data.LY, -data.RX);
        }
    }

    // Bucket angle
    if(data.trigL > 0.01f) {
        bucketArm.set_speed(-data.trigL * 0.5f);
    } else if(data.trigR > 0.01f) {
        bucketArm.set_speed(data.trigR * 0.5f);
    } else {
        bucketArm.set_speed(0.0f);
    }

    // Bucket arm
    if(data.button_y) {
        bucketAngle.set_speed(-0.5f);
    } else if(data.button_a) {
        bucketAngle.set_speed(0.5f);
    } else {
        bucketAngle.set_speed(0.0f);
    }

    driveLeft.set_speed(leftRatio);
    driveRight.set_speed(rightRatio);
    char out[128];
    sprintf(out, "left drive: %f\n\rright drive: %f\nleft trig: %f\nright trig: %f", leftRatio, rightRatio, data.trigL, data.trigR);
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
    bucketAngle.attach(BUCKET_ANGLE_PIN);
    bucketArm.attach(BUCKET_ARM_PIN);

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
