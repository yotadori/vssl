#include <Arduino.h>
#include "EspEasyTimer.h"
#include "Servo.h"
#include "Rot_Servo.h"
#include "Robo.h"
#include "Speaker.h"

#define USE_DABBLE // Dabbleを使わないならここをコメントアウト

#ifdef USE_DABBLE
#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include "DabbleESP32.h"
#endif

#ifndef USE_DABBLE
#include "Udp_Receiver.h"
#endif

constexpr int SERVO_PIN = D7;
constexpr int ROT_PIN_1 = D2;
constexpr int ROT_PIN_2 = D3;
constexpr int ROT_PIN_3 = D4;
constexpr int SPEAKER_PIN = D6;
constexpr int ADC_PIN = D0;
constexpr int BALL_SENSE_PIN = D1;

constexpr float BALL_SENSE_THRESHOLD = 14;

Servo servo0 = Servo(0, SERVO_PIN);
Rot_Servo servo1 = Rot_Servo(1, ROT_PIN_1, 10);
Rot_Servo servo2 = Rot_Servo(2, ROT_PIN_2, 0);
Rot_Servo servo3 = Rot_Servo(3, ROT_PIN_3, 10);

Robo robo = Robo();

#ifndef USE_DABBLE
char* ssid = "F660A-xRb9-G";
char* password = "x9eyusp7";

Udp_Receiver receiver = Udp_Receiver(ssid, password);
#endif

Speaker speaker = Speaker(4, SPEAKER_PIN);

// ADCで読んだバッテリー電圧
float batt_v = 4.8;
// ボールセンサの値
float ball_sense = 0;

void timer1Task() {

  speaker.update(); 

  // DACでバッテリー電圧を読む
  long adc_val = analogRead(ADC_PIN);
  Serial.printf(">dac_value:%f\n", (float)adc_val);
  // LPFにかける
  constexpr float LPF_C = 0.999;
  batt_v = LPF_C * batt_v + (1 - LPF_C) * (adc_val * 0.0014);
  Serial.printf(">batt_vol:%f\n", (float)batt_v);
  // バッテリー電圧が低ければ警告音
  if (batt_v < 4.5 && !speaker.playing()) {
    Speaker::tone_type low_batt_melody[]{{7, 30}, {6, 30}, {0, 30}, {Speaker::STOP, 0}};
    speaker.set_melody(low_batt_melody);
  }

  // DACでボールセンサを読む
  constexpr float BALL_LPF_C = 0.9;
  ball_sense = BALL_LPF_C * ball_sense + (1 - BALL_LPF_C) * analogRead(BALL_SENSE_PIN);
  Serial.printf(">ball_sense:%f\n", (float)ball_sense);

  // 目標速度
  xyz_t target_vel{0, 0, 0};
  const float lost_time = 1000.0; // 通信が途切れてからロスト判定するまでの時間
#ifndef USE_DABBLE
  if (millis() - receiver.updated_time() < lost_time)
  {
    target_vel = receiver.vel();

    Serial.printf("%f, %f, %f", receiver.vel().x, receiver.vel().y, receiver.vel().z);
    Serial.println(receiver.kick_flag());
  }
#endif

#ifdef USE_DABBLE
  // Dabble ゲームパッド
  Dabble.processInput();
  target_vel.x = GamePad.getYaxisData() * 30.0;
  target_vel.y = GamePad.getXaxisData() * -30.0;
  if (GamePad.isCirclePressed())
  {
    target_vel.z = -1;
  }
  if (GamePad.isSquarePressed())
  {
    target_vel.z = 1;
  }
#endif

  xyz_t out_vel = robo.execute(target_vel);

  constexpr float MAX_SPEED = 400.0; // 最大速度(mm/s) 

  // サーボへの出力に変換
  servo1.set_speed(( 0.866 * out_vel.x +0.500 * out_vel.y + Robo::RADIUS * out_vel.z) / MAX_SPEED);
  servo2.set_speed(( 0.000 * out_vel.x -1.000* out_vel.y + Robo::RADIUS * out_vel.z) / MAX_SPEED);
  servo3.set_speed((-0.866 * out_vel.x +0.500 * out_vel.y + Robo::RADIUS * out_vel.z) / MAX_SPEED);

}

EspEasyTimer timer1(TIMER_GROUP_0, TIMER_0);

void setup() {

  // put your setup code here, to run once:

  servo0.set_angle(-90);
  delay(500);
  servo0.stop();
  servo1.set_speed(0);
  servo2.set_speed(0);
  servo3.set_speed(0);

  Serial.begin(115200);

  // ADCでバッテリー電圧を読む
  pinMode(ADC_PIN, INPUT);
  // ADCでボールセンサの値を読む
  pinMode(BALL_SENSE_PIN, INPUT);

#ifndef USE_DABBLE
  receiver.setup();
#endif

  delay(1000);

  speaker.beep(4);
  delay(200);
  speaker.beep(4);
  delay(200);
  speaker.beep(5);
  delay(200);
  speaker.stop();

  Speaker::tone_type doremi[]{{5, 20}, {4, 20}, {5, 20}, {0, 20}, {5, 20}, {4, 20}, {5, 20}, {0, 20}, {Speaker::STOP, 20}};
  speaker.set_melody(doremi);

  // Dabble ゲームパッド
  Dabble.begin("VSSL");

  // 割り込み 60Hz
  // interval 17ms
  timer1.begin(timer1Task, 17);
}

void loop() {
  // put your main code here, to run repeatedly:

  // キック
#ifndef USE_DABBLE
  if (receiver.kick_flag()) {
#endif
#ifdef USE_DABBLE
  if (GamePad.isTrianglePressed()) {
#endif
    if (ball_sense > BALL_SENSE_THRESHOLD)
    {
      // ボールセンサが反応しているとき
      static Speaker::tone_type kick_sound[]{{3, 30}, {4, 10}, {Speaker::STOP, 0}};
      speaker.set_melody(kick_sound);
      servo0.set_angle(-20);
      delay(100);
      servo0.set_angle(-90);
      delay(100);
      ball_sense = 0;
    }
  } else {
    servo0.stop();
  }
  delay(10);


  
}
