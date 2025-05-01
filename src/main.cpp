#include <Arduino.h>
#include "EspEasyTimer.h"
#include "Servo.h"
#include "Rot_Servo.h"
#include "Robo.h"
#include "Speaker.h"
#include "Gyro.h"

#include "RemoteXY_Header.h"
#include "UltrasonicSensor.h"

/*
char* ssid = "F660A-xRb9-G";
char* password = "x9eyusp7";
#include "Udp_Receiver.h"
Udp_Receiver receiver = Udp_Receiver(ssid, password);
*/

/*
#include "Dabble_Receiver.h"
Dabble_Receiver receiver = Dabble_Receiver();
*/

/*
#include "UART_Receiver.h"
UART_Receiver receiver = UART_Receiver();
*/

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
float cycle = 10;

// 割り込み処理
void timer1Task()
{
  speaker.update();
  gyro.update();
  robo.execute(cycle);
}

// 割り込み用タイマー
EspEasyTimer timer1(TIMER_GROUP_0, TIMER_0);

void setup() {
  // RemoteXY初期化
  RemoteXY_Init();

  // put your setup code here, to run once:

  // ロボット初期化
  robo.setup(); 
  robo.stop();

  // 起動時の音
  speaker.beep(5);
  delay(200);
  speaker.beep(1);
  delay(200);
  speaker.beep(5);
  delay(200);
  speaker.stop();
  delay(200);

  // 1秒待つ
  delay(1000); 

  Serial.begin(115200);
  Serial.println("hello from tiny soccer robot");

  // 割り込み開始
  timer1.begin(timer1Task, cycle);

  // いいかんじのメロディーを鳴らす
  Speaker::tone_type start_melody[]{{5, 5}, {4, 5}, {5, 5}, {0, 5}, {5, 5}, {4, 5}, {5, 5}, {0, 5}, {Speaker::STOP, 20}};
  speaker.set_melody(start_melody);

}

void loop() {
  // RemoteXYを更新
  RemoteXY_Handler();

  if (RemoteXY.button_01)
  {
    // ボタンが押されたら

    // 音を鳴らす
    static Speaker::tone_type kick_sound[]{{3, 30}, {4, 10}, {Speaker::STOP, 0}};
    speaker.set_melody(kick_sound);

    // キック
    robo.kick();
  }

  // ジョイスティックの値をもとに、ロボットの速度を設定
  robo.set_target_vel({(float)(RemoteXY.joystick_01_y * 3.0),
                       (float)(RemoteXY.joystick_01_x * -3.0),
                       (float)(RemoteXY.joystick_02_x * -0.04)});
}
