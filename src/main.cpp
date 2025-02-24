#include <Arduino.h>
#include "EspEasyTimer.h"
#include "Servo.h"
#include "Rot_Servo.h"
#include "Robo.h"
#include "Speaker.h"
#include "Gyro.h"
#include "Receiver.h"


//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// you can enable debug logging to Serial at 115200
//#define REMOTEXY__DEBUGLOG    

// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__WIFI_POINT

#include <WiFi.h>

// RemoteXY connection settings 
#define REMOTEXY_WIFI_SSID "RemoteXY"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377


#include <RemoteXY.h>

// RemoteXY GUI configuration  
#pragma pack(push, 1)  
uint8_t RemoteXY_CONF[] =   // 75 bytes
  { 255,5,0,0,0,68,0,19,0,0,0,0,31,1,200,84,1,1,5,0,
  5,13,21,60,60,32,2,26,31,5,127,21,60,60,32,177,26,31,129,34,
  9,18,12,64,17,88,45,89,0,129,138,9,38,12,64,17,65,78,71,76,
  69,0,1,78,22,44,57,3,249,31,75,73,67,75,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  int8_t joystick_01_x; // from -100 to 100
  int8_t joystick_01_y; // from -100 to 100
  int8_t joystick_02_x; // from -100 to 100
  int8_t joystick_02_y; // from -100 to 100
  uint8_t button_01; // =1 if button pressed, else =0

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   
#pragma pack(pop)
 
/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

#include "RemoteXY_Receiver.h"
RemoteXY_Receiver receiver = RemoteXY_Receiver();

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

Servo servo0 = Servo(0, SERVO_PIN);
Rot_Servo rot1 = Rot_Servo(1, ROT_PIN_1, 0);
Rot_Servo rot2 = Rot_Servo(2, ROT_PIN_2, 0);
Rot_Servo rot3 = Rot_Servo(3, ROT_PIN_3, 0);

Gyro gyro = Gyro();

Robo robo = Robo(rot1, rot2, rot3, servo0, gyro);

Speaker speaker = Speaker(4, SPEAKER_PIN);

bool timeout_ = false;

float cycle = 10;

void timer1Task() {
  speaker.update(); 
  gyro.update(); 
  robo.execute(cycle);
}

EspEasyTimer timer1(TIMER_GROUP_0, TIMER_0);

void setup() {
  RemoteXY_Init();

  // put your setup code here, to run once:

  robo.setup(); 
  robo.stop();

  Serial.begin(115200);
  Serial.println("hello from tiny soccer robot");

  speaker.beep(1);
  delay(200);
  speaker.beep(1);
  delay(200);
  speaker.beep(5);
  delay(200);
  speaker.stop();
  delay(200);

  receiver.setup();

  Speaker::tone_type doremi[]{{5, 5}, {4, 5}, {5, 5}, {0, 5}, {5, 5}, {4, 5}, {5, 5}, {0, 5}, {Speaker::STOP, 20}};
  speaker.set_melody(doremi);

  // 割り込み
  timer1.begin(timer1Task, cycle);
  delay(1000);
}

// 目標角度
static float target_angle = 0;

static unsigned long loop_time = millis();

void loop() {
  robo.set_target_vel({80, 0, 0});
  /*
  sleep(2);
  robo.set_target_angle(-0.5 * PI);
  sleep(2);
  robo.set_target_angle(-1.0 * PI);
  sleep(2);
  robo.set_target_angle(-1.5 * PI);
  sleep(2);
  robo.set_target_angle(-2.0 * PI);
  */

  // put your main code here, to run repeatedly:
  constexpr unsigned long lost_time = 1000.0; // 通信が途切れてからロスト判定するまでの時間

  // 一定の周期で回す
  while (millis() - loop_time < 1.0 / 60 * 1000);
  loop_time = millis();

  RemoteXY_Handler();
  xyz_t remote_vel{
    RemoteXY.joystick_01_y * 3.0,
    RemoteXY.joystick_01_x * -3.0,
    RemoteXY.joystick_02_x * -0.03
  };
  receiver.set_values(remote_vel, RemoteXY.button_01);

  receiver.update(); 

  xyz_t target_vel{0, 0, 0};

  if (millis() - receiver.updated_time() < lost_time)
  {
    if (timeout_)
    {
      Speaker::tone_type connected_sound[]{{1, 10}, {3, 10}, {5, 10}, {Speaker::STOP, 20}};
      speaker.set_melody(connected_sound);
      timeout_ = false;
    }
    // キック
    if (receiver.kick_flag())
    {
      static Speaker::tone_type kick_sound[]{{3, 30}, {4, 10}, {Speaker::STOP, 0}};
      speaker.set_melody(kick_sound);
      robo.kick();
    }

    robo.set_target_vel(receiver.vel());
  }
  else
  {
    if (!timeout_)
    {
      Speaker::tone_type timeout_sound[]{{5, 10}, {3, 10}, {1, 10}, {Speaker::STOP, 20}};
      speaker.set_melody(timeout_sound);
      timeout_ = true;
    }
    robo.stop();
    Serial.println("receiver timeout");
  }

}
