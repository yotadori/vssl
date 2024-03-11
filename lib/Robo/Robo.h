// Robo.h
#ifndef ROBO_H
#define ROBO_H 

#include "Context.h"
#include "Arduino.h"

// ロボットの状態を管理するクラス
class Robo {
    public:
        // ロボットの半径(mm)
        constexpr static float RADIUS = 40;

        /**
         * @brief コンストラクタ
        */
        Robo();

        /**
         * @brief 値を更新
         * @param 推定速度
        */
        void update(xyz_t vel);

        /**
         * @brief 制御値を更新
         * @param 目標速度
         * @return 出力速度
        */
        xyz_t execute(xyz_t target_vel);

    private:
        xyz_t vel_; // 推定速度 (x, y, angular)
        xyz_t out_vel_; // 出力速度 (x, y, angular)
        xyz_t error_;
        xyz_t integral_;
};

#endif // ROBO_H