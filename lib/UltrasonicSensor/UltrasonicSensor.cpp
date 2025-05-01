// UltrasonicSensor.cpp

#include "UltrasonicSensor.h"

UltraSonicSensor::UltraSonicSensor (int trig_pin, int echo_pin) :
trig_pin(trig_pin), echo_pin(echo_pin)
{
    pinMode(trig_pin, OUTPUT);
    pinMode(echo_pin, OUTPUT);
}

int UltraSonicSensor::distance_mm() {
    // 初期化
    digitalWrite(trig_pin, LOW);
    digitalWrite(echo_pin, LOW);
    delayMicroseconds(1);
    // 超音波パルスを発射
    digitalWrite(trig_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_pin, LOW);
    // 時間計測
    double duration = pulseIn(echo_pin, HIGH);
    // 距離を計算
    int distance = duration * 0.000001 * 340000 / 2;
    return distance;
}