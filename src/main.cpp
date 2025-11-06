#include <Arduino.h>
#include "EspEasyTimer.h"
#include "Servo.h"
#include "Rot_Servo.h"
#include "Robo.h"
#include "Speaker.h"
#include "Gyro.h"
#include "UltrasonicSensor.h"
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

Udp_Receiver udp_receiver = Udp_Receiver("yota-HP-OmniBook", "yotakunhappy");

// ボールセンサ
bool is_ball_on = false;

// PICスイッチ
bool is_switch_on = false;

// debug mode
bool is_debug_mode = false;

// 割り込みの周期
float cycle = 1;

// 割り込み処理
void timer1Task() {
  udp_receiver.update();
  speaker.update();
  gyro.update();
  if (!is_debug_mode && udp_receiver.updated_time() + 1000 < millis()) {
    // 1秒以上データが来ていないときは停止
    robo.stop();
  } else {
    robo.execute(cycle);
  }
}

// 割り込み用タイマー
EspEasyTimer timer1(TIMER_GROUP_0, TIMER_0);

void setup() {
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

  robo.set_use_gyro(true); // ジャイロを使う

  timer1.begin(timer1Task, cycle); // 割り込み

  Serial1.begin(115200, SERIAL_8N1, D7, D6);

  // 一度キック動作をはさんで、キッカーを引き戻す
  robo.kick();

  // 接続待ち
  robo.stop();
  Speaker::tone_type melody[] = {
    {5, 100},
    {0, 300},
    {Speaker::REPEAT, 0}
  };
  speaker.set_melody(melody);

  // 接続
  udp_receiver.setup();

  if (udp_receiver.isConnected()) {
    // connection success
    Speaker::tone_type melody[] = {
        {5, 200},
        {6, 200},
        {7, 200},
        {Speaker::STOP, 0}};
    speaker.set_melody(melody);
  } else {
    // connection fail
    Speaker::tone_type melody[] = {
        {5, 200},
        {1, 200},
        {1, 200},
        {Speaker::STOP, 0}};
    speaker.set_melody(melody);
  }

}  

// 前回のキックフラグの状態
bool last_kick_flag = false;

void loop() {
  if (Serial.available() > 0) {
    // シリアルモニタからのコマンドを読む
    String input = Serial.readString();
    // split with space
    String command = input.substring(0, input.indexOf(' '));

    if (command == "restart") {
      // 再起動コマンド
      ESP.restart();
    } else if (command == "debug") {
      // デバッグモード開始コマンド
      Serial.println("debug mode start");
      is_debug_mode = true;
    } else if (command == "dribble") {
      // ドリブルパワー設定コマンド
      int power = input.substring(input.indexOf(' ') + 1).toInt();
      Serial1.write(power);
    } else if (command == "beep") {
      // ビープ音コマンド
      int tone = input.substring(input.indexOf(' ') + 1).toInt();
      speaker.beep(tone);
    } else if (command == "kick") {
      robo.kick();
    } else {
      // 未知のコマンド
      Serial.println("Unknown command");
      // ヘルプ表示
      Serial.println(" restart - restart the robot");
      Serial.println(" debug - enter debug mode");
      Serial.println("Available commands:");
      Serial.println(" dribble <power> - set dribble power (0-12)");
      Serial.println(" beep <tone> - play beep sound (0-9), 0 to stop");
      Serial.println(" kick - perform kick action");
    }
  }

  if (Serial1.available() > 0) {
    // PICからのデータを読む
    int data = Serial1.read();
    is_ball_on = (data & 0x01) == 0;
    is_switch_on = (data & 0x02) == 0;
  }

  if (udp_receiver.updated_time() + 1000 < millis()) {
    // 1秒以上データが来ていないときは停止
    return;
  }

  robo.set_target_vel(udp_receiver.vel());

  if (udp_receiver.kick_flag() || is_switch_on) {
    if (!last_kick_flag) {
      // キックフラグが立ったときに音を鳴らす
      Speaker::tone_type melody[] = {
          {1, 50},
          {2, 50},
          {Speaker::REPEAT, 0}};
      speaker.set_melody(melody);
      last_kick_flag = true;
    }
    if (is_ball_on) {
      // キックフラグが立っていて、ボールセンサがオンのときキック
      robo.kick();
    }
  } else if (last_kick_flag) {
    // キックフラグが下がったときに音を止める
    speaker.stop_melody();
    last_kick_flag = false;
  }

  // ドリブルパワーをそのまま送る
  Serial1.write(udp_receiver.dribble_pow());

  // デバッグ用情報をシリアルモニタに表示
  // Serial.printf("Ball: %d, Switch: %d\n", is_ball_on ? 1 : 0, is_switch_on ? 1 : 0);
  
}