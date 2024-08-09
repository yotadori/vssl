// Dabble_Receiver.cpp

#include "Dabble_Receiver.h"

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>

Dabble_Receiver::Dabble_Receiver() : Receiver() {}

void Dabble_Receiver::setup() {
    // Dabble ゲームパッド
    Dabble.begin("VSSL1");
}

void Dabble_Receiver::update() {

    Serial.println("Dabble input");
    // 時刻を取得
    last_updated_time_ = millis();

    Dabble.processInput();

    kick_flag_ = GamePad.isTrianglePressed();

    if (GamePad.isCrossPressed())
    {
        float time = millis();
        vel_.x = 200 * sin(time / 500);
        vel_.y = 200 * cos(time / 500);
    }
    else
    {
        vel_.x = GamePad.getYaxisData() * 60.0;
        vel_.y = GamePad.getXaxisData() * -60.0;
    }
    vel_.z = 0;
    if (GamePad.isCirclePressed())
    {
        vel_.z = -5;
    }
    if (GamePad.isSquarePressed())
    {
        vel_.z = 5;
    }
}