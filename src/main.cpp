#include <Arduino.h>
#include "EspEasyTimer.h"
#include "Servo.h"
#include "Rot_Servo.h"
#include "Robo.h"
#include "Speaker.h"
#include "Gyro.h"
#include "UltrasonicSensor.h"
//#include "RemoteXY_Header.h"
#include "Udp_Receiver.h"

/*
constexpr int SERVO_PIN = D7;
constexpr int ROT_PIN_1 = D10;
constexpr int ROT_PIN_2 = D2;
constexpr int ROT_PIN_3 = D3;
constexpr int SPEAKER_PIN = D6;
*/

// AI module adapted circuit
// ピン番号の設定
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
  speaker.update();
  gyro.update();
  robo.execute(cycle);
}

Udp_Receiver udp_receiver = Udp_Receiver("yota-HP-OmniBook","yotakunhappy");

// 割り込み用タイマー
EspEasyTimer timer1(TIMER_GROUP_0, TIMER_0);

void setup(){
  Serial.begin(115200);

  // 各種初期化
  gyro.setup();
  robo.setup();
  robo.stop();
  udp_receiver.setup();

  // 割り込み設定
  timer1.begin(timer1Task, cycle);

  // いいかんじのメロディーを鳴らす
  Speaker::tone_type start_melody[]{{5, 50}, {4, 50}, {5, 50}, {0, 50}, {5, 50}, {4, 50}, {5, 50}, {0, 50}, {Speaker::STOP, 20}};
  speaker.set_melody(start_melody);

}

void loop() {
  udp_receiver.update();

  udp_receiver.updated_time();

  robo.set_target_vel(udp_receiver.vel());
  if (udp_receiver.kick_flag()) {
    // 音を鳴らす
    static Speaker::tone_type kick_sound[]{{3, 30}, {4, 10}, {Speaker::STOP, 0}};
    speaker.set_melody(kick_sound);
    robo.kick();
  }
  
  delay(10);
}