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

    // 受信したデータを保存する変数
    String receivedData;
    float x, y, z;
    int kick;

    // データがあるか確認
    if (Serial1.available())
    {
        // 時刻を取得
        last_updated_time_ = millis();

        receivedData = Serial1.readStringUntil('\n'); // データを1行（改行まで）読み取る

        // カンマでデータを分割
        int firstComma = receivedData.indexOf(',');
        int secondComma = receivedData.indexOf(',', firstComma + 1);
        int thirdComma = receivedData.indexOf(',', secondComma + 1);

        if (firstComma > 0 && secondComma > firstComma && thirdComma > secondComma)
        {
            x = receivedData.substring(0, firstComma).toFloat();
            y = receivedData.substring(firstComma + 1, secondComma).toFloat();
            z = receivedData.substring(secondComma + 1, thirdComma).toFloat();
            kick = receivedData.substring(thirdComma + 1).toInt();

            // 受け取ったデータをシリアルモニタに表示
            Serial.print("X: ");
            Serial.println(x);
            Serial.print("Y: ");
            Serial.println(y);
            Serial.print("Z: ");
            Serial.println(z);
            Serial.print("Kick: ");
            Serial.println(kick);

            kick_flag_ = (kick != 0);

            vel_.x = x;
            vel_.y = y;
            vel_.z = z;
        }
        else
        {
            Serial.println("データ形式が不正です。");
        }
    }
}