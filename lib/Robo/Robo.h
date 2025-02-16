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
         * @param rot1 モーター1
         * @param rot2 モーター2
         * @param rot3 モーター3
         * @param servo キック用サーボモーター
         * @param gyro ジャイロ
        */
        Robo(Rot_Servo& rot1, Rot_Servo& rot2, Rot_Servo& rot3, Servo& servo, Gyro& gyro);

        /**
         * @brief setup （setupで呼ぶ）
         */
        void setup();

        /**
         * @brief 制御値を更新（一定周期で呼ぶこと）
         * @param cycle 周期
        */
        void execute(float cycle);

        /**
         * @brief キック
         */
        void kick();

        /**
         * @brief 足回りのモーターをすべて停止
         */
        void stop();

        //// 以下 setter ////
        
        /**
         * @brief 目標速度を設定
         */
        void set_target_vel(xyz_t target_vel);

        /**
         * @brief 目標角度を設定
         */
        void set_target_angle(float angle);

    private:
        xyz_t target_vel_; // 目標速度
        float target_angle_; // 目標角度

        xyz_t vel_; // 推定速度 (x, y, angular)
        xyz_t error_; // 速度誤差 (x, y, angular)
        xyz_t integral_; // errorの積分

        // モーター
        Rot_Servo& rot1_;
        Rot_Servo& rot2_;
        Rot_Servo& rot3_;

        // キック用モーター
        Servo& servo_;

        // ジャイロ
        Gyro& gyro_;

        bool kicking_; // キック動作中か
        unsigned int kick_count_;
};

#endif // ROBO_H