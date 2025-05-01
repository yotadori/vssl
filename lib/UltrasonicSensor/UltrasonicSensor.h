// UltrasonicSensor.h
#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include "Arduino.h"

// 超音波距離センサを使うためのクラス
class UltraSonicSensor {
    public:

        /**
         * @brief コンストラクタ
         * @param trig_pin トリガピン
         * @param echo_pin エコーピン
        */
        UltraSonicSensor(int trig_pin, int echo_pin);

        /**
         * @brief 距離を測る
         * @return 距離(mm)
         */
        int distance_mm();

    private:
        int trig_pin;
        int echo_pin;
};

#endif // ULTRASONIC_SENSOR_H