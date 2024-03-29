// Speaker.h
#ifndef SPEAKER_H
#define SPEAKER_H

#include "Arduino.h"

// 圧電スピーカで音を鳴らすためのクラス
class Speaker {
    public:
        /**
         * @brief コンストラクタ
         * @param channel pwmのチャンネル
         * @param pin ピン
        */
        Speaker(int channel, int pin);

        /**
        * @brief 音を鳴らす
        * @param tone 高さ
        */
        void beep(int tone);

        /**
         * @brief 音を止める
        */
        void stop();

    private:
        int channel_;
};

#endif // SPEAKER_H