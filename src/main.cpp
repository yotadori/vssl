#include <Arduino.h>
#include "EspEasyTimer.h"
#include "Servo.h"
#include "Rot_Servo.h"
#include "Robo.h"
#include "Speaker.h"
#include "Gyro.h"

/*
constexpr int SERVO_PIN = D7;
constexpr int ROT_PIN_1 = D10;
constexpr int ROT_PIN_2 = D2;
constexpr int ROT_PIN_3 = D3;
constexpr int SPEAKER_PIN = D6;
*/

// AI module adapted circuit
constexpr int SERVO_PIN = D10;
constexpr int ROT_PIN_1 = D1;
constexpr int ROT_PIN_2 = D2;
constexpr int ROT_PIN_3 = D3;
constexpr int SPEAKER_PIN = D0;
constexpr int US_TRIG_PIN = D6;
constexpr int US_ECHO_PIN = D7;

// サーボモーター（キック用モーター）
Servo servo0 = Servo(0, SERVO_PIN);
// 連続回転サーボモーター（ホイールのモーター）
Rot_Servo rot1 = Rot_Servo(1, ROT_PIN_1, 0);
Rot_Servo rot2 = Rot_Servo(2, ROT_PIN_2, 0);
Rot_Servo rot3 = Rot_Servo(3, ROT_PIN_3, 0);

// ジャイロセンサー
Gyro gyro = Gyro();

// ロボット
Robo robo = Robo(rot1, rot2, rot3, servo0, gyro);

// スピーカー
Speaker speaker = Speaker(4, SPEAKER_PIN);

// 割り込みの周期
float cycle = 1;

// 割り込み処理
void timer1Task() {

}

// 割り込み用タイマー
EspEasyTimer timer1(TIMER_GROUP_0, TIMER_0);

void setup() {
  
}

void loop() {
  
}