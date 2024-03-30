// Servo.h
#ifndef SERVO_H
#define SERVO_H

// サーボモータを制御するためのクラス
class Servo {
    public:
        /**
         * @brief コンストラクタ
         * @param channel pwmのチャンネル
         * @param pin ピン
        */
        Servo(int channel, int pin);

        /**
         * @brief 角度をセット
         * @param angle -255~255
        */
        void set_angle(int angle);

        /**
         * @brief 力を抜いて止める
        */
        void stop();

    private:
        // pwmのチャンネル
        int channel_;

};

#endif // SERVO_H