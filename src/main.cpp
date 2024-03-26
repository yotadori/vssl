#include <Arduino.h>
#include "EspEasyTimer.h"
#include "Servo.h"
#include "Rot_Servo.h"
#include "Gyro.h"
#include "Robo.h"
#include "Udp_Receiver.h"

const int SERVO_PIN = 2;
const int ROT_PIN_1 = 13;
const int ROT_PIN_2 = 19;
const int ROT_PIN_3 = 32;

Servo servo0 = Servo(0, SERVO_PIN);
Rot_Servo servo1 = Rot_Servo(1, ROT_PIN_1, 0);
Rot_Servo servo2 = Rot_Servo(2, ROT_PIN_2, 100);
Rot_Servo servo3 = Rot_Servo(3, ROT_PIN_3, 300);

Gyro gyro_sens = Gyro();

Robo robo = Robo();

char* ssid = "F660A-xRb9-G";
char* password = "x9eyusp7";

Udp_Receiver receiver = Udp_Receiver(ssid, password);

void timer1Task() {
  gyro_sens.update();
  
  // 推定速度
  xyz_t vel {-1 * gyro_sens.vel().z, -1 * gyro_sens.vel().y, -1 * gyro_sens.gyro().x};
  // 目標速度
  xyz_t target_vel{0, 0, 0};
  const float lost_time = 1000.0; // 通信が途切れてからロスト判定するまでの時間
  if (millis() - receiver.updated_time() < lost_time) {
      target_vel = receiver.vel();
  }

  robo.update(vel);
  xyz_t out_vel = robo.execute(target_vel);

  constexpr float MAX_SPEED = 400.0; // 最大速度(mm/s) 

  // サーボへの出力に変換
  servo1.set_speed(( 0.866 * out_vel.x +0.500 * out_vel.y + Robo::RADIUS * out_vel.z) / MAX_SPEED);
  servo2.set_speed(( 0.000 * out_vel.x -1.000* out_vel.y + Robo::RADIUS * out_vel.z) / MAX_SPEED);
  servo3.set_speed((-0.866 * out_vel.x +0.500 * out_vel.y + Robo::RADIUS * out_vel.z) / MAX_SPEED);

  

  Serial.printf("%f, %f, %f", receiver.vel().x, receiver.vel().y, receiver.vel().z);
  Serial.println(receiver.kick_flag());
}

EspEasyTimer timer1(TIMER_GROUP_0, TIMER_0);

void setup() {

  // put your setup code here, to run once:
  gyro_sens.setup();

  servo0.set_angle(-90);
  servo1.set_speed(0);
  servo2.set_speed(0);
  servo3.set_speed(0);

  Serial.begin(115200);

  receiver.setup();

  // 加速度センサ用割り込み
  // interval 10ms
  timer1.begin(timer1Task, 10);

  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:

  // キック
  if (receiver.kick_flag()) {
    servo0.set_angle(-60);
    delay(50);
    servo0.set_angle(-90);
    delay(100);
  }
  delay(10);
  
}
