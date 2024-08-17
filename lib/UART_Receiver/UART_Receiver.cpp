// UART_Receiver.cpp

#include "UART_Receiver.h"

UART_Receiver::UART_Receiver() : Receiver()
{
}

void UART_Receiver::setup() {
    const int BAUD_RATE = 115200;

    Serial1.begin(BAUD_RATE, SERIAL_8N1, D7, D6);

    Serial.println("UART Receiver started");
}

void UART_Receiver::update()
{
    int val[4] = {0};

  //受信データがある場合if内を処理
    if (Serial1.available() > 0)
    {
        last_updated_time_ = millis();
        delay(10);
        for (int i = 0; i < 4; i++)
        {
            val[i] = Serial1.parseInt(); // 文字列データを数値に変換
        }

        while (Serial1.available() > 0)
        { // 受信バッファクリア
            char t = Serial1.read();
        }

        vel_.x = val[0];
        vel_.y = val[1];
        vel_.z = val[2];
        kick_flag_ = val[3] != 0;
    }
}