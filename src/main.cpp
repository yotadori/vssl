#include <Arduino.h>
#include "EspEasyTimer.h"
#include "Servo.h"
#include "Rot_Servo.h"
#include "Robo.h"
#include "Speaker.h"
#include "Gyro.h"

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
constexpr int ROT_PIN_1 = D10;
constexpr int ROT_PIN_2 = D2;
constexpr int ROT_PIN_3 = D3;
constexpr int SPEAKER_PIN = D6;
 constexpr int ADC_PIN = A1;
constexpr int BALL_SENSE_PIN = A0;

Servo servo0 = Servo(0, SERVO_PIN);
Rot_Servo servo1 = Rot_Servo(1, ROT_PIN_1, 0.2);
Rot_Servo servo2 = Rot_Servo(2, ROT_PIN_2, 0);
Rot_Servo servo3 = Rot_Servo(3, ROT_PIN_3, 0.2);

Gyro gyro = Gyro();

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
float ball_sense_average = 0;

void timer1Task() {

  speaker.update(); 
  gyro.update(); 
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
  ball_sense_average = analogRead(BALL_SENSE_PIN);

  gyro.setup();

#ifndef USE_DABBLE
  receiver.setup();
#endif

  speaker.beep(1);
  delay(200);
  speaker.beep(1);
  delay(200);
  speaker.beep(5);
  delay(200);
  speaker.stop();
  delay(200);

  Speaker::tone_type doremi[]{{5, 5}, {4, 5}, {5, 5}, {0, 5}, {5, 5}, {4, 5}, {5, 5}, {0, 5}, {Speaker::STOP, 20}};
  speaker.set_melody(doremi);

  // Dabble ゲームパッド
  Dabble.begin("VSSL");
  
  delay(1000);

  // 割り込み 60Hz
  // interval 17ms
  timer1.begin(timer1Task, 17);
}

// 目標角度
static float target_angle = 0;

void loop() {
  // put your main code here, to run repeatedly:

  // キック
#ifndef USE_DABBLE
  if (receiver.kick_flag()) {
#endif
#ifdef USE_DABBLE
  if (GamePad.isTrianglePressed()) {
#endif
    if (ball_sense > ball_sense_average * 1.2)
    {
      // ボールセンサが反応しているとき
      static Speaker::tone_type kick_sound[]{{3, 30}, {4, 10}, {Speaker::STOP, 0}};
      speaker.set_melody(kick_sound);
      servo0.set_angle(-20);
      delay(300);
      servo0.set_angle(-90);
      delay(100);
      ball_sense = 0;
    }
  } else {
    servo0.stop();
  }

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
  constexpr float BALL_LPF_C = 0.8;
  ball_sense = BALL_LPF_C * ball_sense + (1 - BALL_LPF_C) * analogRead(BALL_SENSE_PIN);
  constexpr float AVERAGE_LPF_C = 0.9999;
  ball_sense_average = AVERAGE_LPF_C * ball_sense_average + (1 - AVERAGE_LPF_C) * ball_sense;
  Serial.printf(">ball_sense:%f\n", (float)ball_sense);
  Serial.printf(">ball_sense_average:%f\n", (float)ball_sense_average);
  Serial.printf(">ball_on:%f\n", ball_sense - ball_sense_average * 1.2);

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
  if (GamePad.isCrossPressed()) {
    float time = millis();
    target_vel.x = 200 * sin(time / 500);
    target_vel.y = 200 * cos(time / 500);
  }
  else
  {
    target_vel.x = GamePad.getYaxisData() * 60.0;
    target_vel.y = GamePad.getXaxisData() * -60.0;
  }
  if (GamePad.isCirclePressed())
  {
    target_angle -= 0.1;
  }
  if (GamePad.isSquarePressed())
  {
    target_angle += 0.1;
  }
#endif

  target_vel.z = 9 * (target_angle - gyro.angle());
  xyz_t out_vel = robo.execute(target_vel);

  constexpr float MAX_SPEED = 400.0; // 最大速度(mm/s) 

  // サーボへの出力に変換
  servo1.set_speed(( 0.866 * out_vel.x +0.500 * out_vel.y + Robo::RADIUS * out_vel.z) / MAX_SPEED);
  servo2.set_speed(( 0.000 * out_vel.x -1.000* out_vel.y + Robo::RADIUS * out_vel.z) / MAX_SPEED);
  servo3.set_speed((-0.866 * out_vel.x +0.500 * out_vel.y + Robo::RADIUS * out_vel.z) / MAX_SPEED);
  
  /*
  // 連続回転サーボ校正用
  static float duty = 0;
  Serial.printf(">duty:%f\n", (float)duty);
  if (Serial.available())
  {
    char key = Serial.read();
    if (key == 'w') {
      duty += 0.01;
    } else if (key == 's') {
      duty -= 0.01;
    }
    servo2.set_duty(duty);
  }
  */

  delay(17);
}
