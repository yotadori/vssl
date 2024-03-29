// Speaker.h
#ifndef SPEAKER_H
#define SPEAKER_H

#include "Arduino.h"

#include "EspEasyTimer.h"

// 圧電スピーカで音を鳴らすためのクラス
class Speaker {
    public:
        typedef struct
        {
            int tone;
            int time;
        } tone_type;

        static constexpr int STOP = 99;
        static constexpr int REPEAT = 98;

        /**
         * @brief コンストラクタ
         * @param channel pwmのチャンネル
         * @param pin ピン
        */
        Speaker(int channel, int pin);

        /**
         * @brief 一定時間ごとに呼ぶ
        */
        void update();

        /**
        * @brief 音を鳴らす
        * @param tone 高さ
        */
        void beep(int tone);

        /**
         * @brief 音を止める
        */
        void stop();
        

        /**
         * @brief melodyをセット
         * @param melody{{tone, time}}
        */
        void set_melody(tone_type melody[]);

    private:
        int channel_;
        tone_type melody_[99];
        int counter_;
        int melody_step_;
};

#endif // SPEAKER_H