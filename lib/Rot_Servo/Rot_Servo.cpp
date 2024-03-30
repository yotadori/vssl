// Rot_Servo.cpp

#include <Arduino.h>

#include "Rot_Servo.h"

Rot_Servo::Rot_Servo(int channel, int pin, int offset = 0) :
 channel_(channel), offset_(offset)
{
    // pwmのセットアップ
    // 50Hz, 8bit
    pinMode(pin, OUTPUT);
    ledcSetup(channel_, 50, 12);
    ledcAttachPin(pin, channel_);
}

void Rot_Servo::set_speed(float speed) {
    if (speed > 1) {
        speed = 1;
    } else if (speed < -1) {
        speed = -1;
    }

    int duty = 0; // speed = 0 のときは確実に止める
    if (speed != 0) {
        // -1.0~1.0を2.5%~12%に変換
        duty = (speed * 9.5 / 2.0 + 7.25) * 4095 / 100.0 + offset_;
    }
    ledcWrite(channel_, duty);
}