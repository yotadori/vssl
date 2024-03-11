// Rot_Servo.cpp

#include <Arduino.h>

#include "Rot_Servo.h"

Rot_Servo::Rot_Servo(int channel, int pin, int offset = 0) :
 channel_(channel), offset_(offset)
{
    // pwmのセットアップ
    // 50Hz, 8bit
    ledcSetup(channel_, 50, 16);
    ledcAttachPin(pin, channel_);
}

void Rot_Servo::set_speed(float speed) {
    if (speed > 1) {
        speed = 1;
    } else if (speed < -1) {
        speed = -1;
    }
    // -1.0~1.0を2.5%~12%に変換
    int duty = (speed * 9.5 / 2.0 + 7.25) * 65535 / 100.0 + offset_;
    ledcWrite(channel_, duty);
}