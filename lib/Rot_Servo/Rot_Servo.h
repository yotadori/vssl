// Rot_Servo.h
#ifndef ROT_SERVO_H
#define ROT_SERVO_H

// 連続回転サーボモータを制御するためのクラス
class Rot_Servo {
    public:
        /**
         * @brief コンストラクタ
         * @param channel pwmのチャンネル
         * @param pin ピン
        */
        Rot_Servo(int channel, int pin, int offset);

        /**
         * @brief 速度をセット
         * @param speed -1.0~1.0 
        */
        void set_speed(float speed);

    private:
        // pwmのチャンネル
        int channel_;
        int offset_;

};

#endif // ROT_SERVO_H