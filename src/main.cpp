#include <Arduino.h>
#include "EspEasyTimer.h"



//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// you can enable debug logging to Serial at 115200
//#define REMOTEXY__DEBUGLOG    

// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__ESP32CORE_BLE
#include <BLEDevice.h>

#include <RemoteXY.h>

// RemoteXY connection settings 
#define REMOTEXY_BLUETOOTH_NAME "RemoteXY"



// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =   // 46 bytes
  { 255,5,0,0,0,39,0,17,0,0,0,31,1,106,200,1,1,3,0,5,
  24,17,60,60,32,2,26,31,5,23,106,60,60,32,2,26,31,1,8,166,
  24,24,0,2,31,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  int8_t joystick_01_x; // from -100 to 100}
  int8_t joystick_01_y; // from -100 to 100}
  int8_t joystick_02_x; // from -100 to 100}
  int8_t joystick_02_y; // from -100 to 100}
  uint8_t button_01; // =1 if button pressed, else =0

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;
#pragma pack(pop)
 
/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

#include "Servo.h"
#include "Rot_Servo.h"
#include "Gyro.h"
#include "Robo.h"

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

void timer1Task() {
  gyro_sens.update();
}

EspEasyTimer timer1(TIMER_GROUP_0, TIMER_0);

void setup() {

  RemoteXY_Init (); 

  // put your setup code here, to run once:
  gyro_sens.setup();

  servo0.set_angle(-90);
  servo1.set_speed(0);
  servo2.set_speed(0);
  servo3.set_speed(0);

  Serial.begin(115200);

  // 加速度センサ用割り込み
  // interval 10ms
  timer1.begin(timer1Task, 10);

  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:

  RemoteXY_Handler ();

  // 推定速度
  xyz_t vel {-1 * gyro_sens.vel().z, -1 * gyro_sens.vel().y, -1 * gyro_sens.gyro().x};
  // 目標速度
  xyz_t target_vel{0, 0, 0};
  target_vel.x = RemoteXY.joystick_01_x;
  target_vel.y = RemoteXY.joystick_01_y;
  target_vel.z = RemoteXY.joystick_02_y / 100.0;

  robo.update(vel);
  xyz_t out_vel = robo.execute(target_vel);

  constexpr float MAX_SPEED = 400.0; // 最大速度(mm/s) 

  // サーボへの出力に変換
  servo1.set_speed(( 0.866 * out_vel.x +0.500 * out_vel.y + Robo::RADIUS * out_vel.z) / MAX_SPEED);
  servo2.set_speed(( 0.000 * out_vel.x -1.000* out_vel.y + Robo::RADIUS * out_vel.z) / MAX_SPEED);
  servo3.set_speed((-0.866 * out_vel.x +0.500 * out_vel.y + Robo::RADIUS * out_vel.z) / MAX_SPEED);

  // キック
  if (RemoteXY.button_01 == 1) {
    servo0.set_angle(-60);
    delay(100);
    servo0.set_angle(-90);
  }

  Serial.printf(">x_tar:%f\n", target_vel.x);
  Serial.printf(">y_tar:%f\n", target_vel.y);
  Serial.printf(">z_tar:%f\n", target_vel.z);

  Serial.printf(">x:%f\n", vel.x);
  Serial.printf(">y:%f\n", vel.y);
  Serial.printf(">z:%f\n", vel.z);

  Serial.printf(">x_out:%f\n", out_vel.x);
  Serial.printf(">y_out:%f\n", out_vel.y);
  Serial.printf(">z_out:%f\n", out_vel.z);

  RemoteXY_delay(20);
}
