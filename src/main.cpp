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

const int SERVO_PIN = D7;
const int ROT_PIN_1 = D2;
const int ROT_PIN_2 = D3;
const int ROT_PIN_3 = D4;
const int SPEAKER_PIN = D6;

Servo servo0 = Servo(0, SERVO_PIN);
Rot_Servo servo1 = Rot_Servo(1, ROT_PIN_1, 0);
Rot_Servo servo2 = Rot_Servo(2, ROT_PIN_2, 0);
Rot_Servo servo3 = Rot_Servo(3, ROT_PIN_3, 0);

Robo robo = Robo();

#ifndef USE_DABBLE
char* ssid = "F660A-xRb9-G";
char* password = "x9eyusp7";

Udp_Receiver receiver = Udp_Receiver(ssid, password);
#endif

Speaker speaker = Speaker(4, SPEAKER_PIN);

void timer1Task() {

  speaker.update();

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
  target_vel.x = GamePad.getYaxisData() * 50.0;
  target_vel.y = GamePad.getXaxisData() * -50.0;
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
  servo1.set_speed(0);
  servo2.set_speed(0);
  servo3.set_speed(0);

  Serial.begin(115200);

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

  // 加速度センサ用割り込み
  // interval 10ms
  timer1.begin(timer1Task, 10);
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
    servo0.set_angle(-60);
    delay(50);
    servo0.set_angle(-90);
    delay(100);
  }
  delay(10);


  
}
