#include <Arduino.h>
#include "EspEasyTimer.h"
#include "Servo.h"
#include "Rot_Servo.h"
#include "Robo.h"
#include "Speaker.h"
#include "Gyro.h"
#include "UltrasonicSensor.h"
#include "RemoteXY_Header.h"
#include "UART_Receiver.h"

/*
constexpr int SERVO_PIN = D7;
constexpr int ROT_PIN_1 = D10;
constexpr int ROT_PIN_2 = D2;
constexpr int ROT_PIN_3 = D3;
constexpr int SPEAKER_PIN = D6;
*/

// AI module adapted circuit
// ピン番号の設定
constexpr int SERVO_PIN = D3;
constexpr int ROT_PIN_1 = D2;
constexpr int ROT_PIN_2 = D10;
constexpr int ROT_PIN_3 = D1;
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

UART_Receiver uart_receiver = UART_Receiver();

int dribble_speed = 0; // ドリブル用の速度

// 割り込みの周期
float cycle = 1;

// 割り込み処理
void timer1Task() {
  speaker.update();
  gyro.update();
  robo.execute(cycle);
}

// 割り込み用タイマー
EspEasyTimer timer1(TIMER_GROUP_0, TIMER_0);

void setup() {
  RemoteXY_Init();
  speaker.beep(1); // 起動時に音を鳴らす
  delay(500);
  speaker.beep(2);
  delay(500);
  speaker.stop(); // 音を止める

  /*
  rot1.set_speed(10);
  delay(1000);
  rot1.set_speed(0);
  rot2.set_speed(10);
  delay(1000);
  rot2.set_speed(0);
  rot3.set_speed(10);
  delay(1000);
  rot3.set_speed(0);
  */

  Serial.begin(115200);
  robo.setup();
  gyro.setup(); // ジャイロの初期化

  uart_receiver.setup(); // UARTレシーバーの初期化

  robo.set_use_gyro(true); // ジャイロを使う

  timer1.begin(timer1Task, cycle); // 割り込み

  Serial1.begin(115200, SERIAL_8N1, D7, D6);

  // 一度キック動作をはさんで、キッカーを引き戻す
  robo.kick();
}  

void loop() {
  RemoteXY_Handler();
  robo.set_target_vel(RemoteXY.joystick_01_y * 2.0,
                      RemoteXY.joystick_01_x * -2.0,
                      RemoteXY.joystick_02_x / -30.0);
  if (RemoteXY.button_01) {
    robo.kick();
  }

  if (RemoteXY.button_02) {
    dribble_speed++;
    speaker.beep(5);
    RemoteXY_delay(500);
    speaker.stop();
  } else if (RemoteXY.button_03) {
    dribble_speed--;
    speaker.beep(3);
    RemoteXY_delay(500);
    speaker.stop();
  }
  if (dribble_speed < 0) {
    dribble_speed = 0;
  } else if (dribble_speed > 9) {
    dribble_speed = 9;
  }

  Serial1.printf("%c", '0' + dribble_speed);

  if (Serial.available() > 0) {
    String input = Serial.readString();
    Serial1.printf("%s", input.c_str());
  }
  if (Serial1.available() > 0) {
    Serial.printf("%s", Serial1.readString().c_str());
  }

  RemoteXY_delay(10);
}