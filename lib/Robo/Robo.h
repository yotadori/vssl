// Robo.h
#ifndef ROBO_H
#define ROBO_H 

#include "Context.h"
#include "Arduino.h"
#include "Rot_Servo.h"
#include "Servo.h"
#include "Speaker.h"
#include "Gyro.h"

// ロボットの状態を管理するクラス
class Robo {
    public:
        // ロボットの半径(mm)
        constexpr static float RADIUS = 40;

        /**
         * @brief コンストラクタ
        */
        Robo(Rot_Servo& rot1, Rot_Servo& rot2, Rot_Servo& rot3, Servo& servo, Gyro& gyro);

        void setup();

        /**
         * @brief 値を更新
         * @param 推定速度
        */
        void update(xyz_t vel);

        /**
         * @brief 制御値を更新
         * @param 目標速度
        */
        void execute(xyz_t target_vel);

        /**
         * @brief kick
         */
        void kick();

        /**
         * @brief stop all motors
         */
        void stop();

    private:
        xyz_t vel_; // 推定速度 (x, y, angular)
        xyz_t out_vel_; // 出力速度 (x, y, angular)
        xyz_t error_;
        xyz_t integral_;

        Rot_Servo& rot1_;
        Rot_Servo& rot2_;
        Rot_Servo& rot3_;

        Servo& servo_;

        Gyro& gyro_;

        bool kicking_;
        unsigned int kick_count_;
};

#endif // ROBO_H