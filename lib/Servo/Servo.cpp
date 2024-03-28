// Servo.cpp

#include <Arduino.h>

#include "Servo.h"

Servo::Servo(int channel, int pin) : channel_(channel) {
    // pwmのセットアップ
    // 50Hz, 8bit
    pinMode(pin, OUTPUT);
    ledcSetup(channel_, 50, 12);
    ledcAttachPin(pin, channel_);
}

void Servo::set_angle(int angle) {
    // リミッタ
    if (angle > 90) {
        angle = 90;
    } else if (angle < -90) {
        angle = -90;
    }

    // -90~90を2.5%~12%に変換
    int duty = (angle * 9.5 / 180 + 7.25) * 4095 / 100;

    ledcWrite(channel_, duty);
}