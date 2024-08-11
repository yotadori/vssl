#include <Arduino.h>
#include "EspEasyTimer.h"
#include "Servo.h"
#include "Rot_Servo.h"
#include "Robo.h"
#include "Speaker.h"
#include "Gyro.h"

#include "Receiver.h"
/*
char* ssid = "F660A-xRb9-G";
char* password = "x9eyusp7";
#include "Udp_Receiver"
Receiver receiver = Udp_Receiver(ssid, password);
*/

#include "Dabble_Receiver.h"
Dabble_Receiver receiver = Dabble_Receiver();

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

Servo servo0 = Servo(0, SERVO_PIN);
Rot_Servo rot1 = Rot_Servo(1, ROT_PIN_1, 0);
Rot_Servo rot2 = Rot_Servo(2, ROT_PIN_2, 0);
Rot_Servo rot3 = Rot_Servo(3, ROT_PIN_3, 0);

Gyro gyro = Gyro();

Robo robo = Robo(rot1, rot2, rot3, servo0, gyro);

Speaker speaker = Speaker(4, SPEAKER_PIN);

void timer1Task() {

  speaker.update(); 
  gyro.update(); 
}

EspEasyTimer timer1(TIMER_GROUP_0, TIMER_0);

void setup() {

  // put your setup code here, to run once:

  robo.setup(); 

  Serial.begin(115200);

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

  receiver.setup();

  delay(1000);

  // 割り込み 60Hz
  // interval 17ms
  timer1.begin(timer1Task, 17);
}

// 目標角度
static float target_angle = 0;

void loop() {
  // put your main code here, to run repeatedly:

  receiver.update();

  // キック
  if (receiver.kick_flag()) {
    static Speaker::tone_type kick_sound[]{{3, 30}, {4, 10}, {Speaker::STOP, 0}};
    speaker.set_melody(kick_sound);
    robo.kick();
  }

  xyz_t target_vel{0, 0, 0};

  constexpr float cycle = 17;
  const float lost_time = 1000.0; // 通信が途切れてからロスト判定するまでの時間

  target_vel = receiver.vel();
  target_angle += target_vel.z * cycle / 1000.0;
  Serial.printf("%f, %f, %f", receiver.vel().x, receiver.vel().y, receiver.vel().z);
  Serial.println(receiver.kick_flag());

  target_vel.z = 9 * (target_angle - gyro.angle());
  robo.execute(target_vel);

  delay(cycle);
}
