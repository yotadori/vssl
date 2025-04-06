#include <Arduino.h>
#include "EspEasyTimer.h"
#include "Servo.h"
#include "Rot_Servo.h"
#include "Robo.h"
#include "Speaker.h"
#include "Gyro.h"


//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// you can enable debug logging to Serial at 115200
//#define REMOTEXY__DEBUGLOG    

// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__ESP32CORE_BLE

#include <BLEDevice.h>

// RemoteXY connection settings 
// 近くでやっている人と同じにならないようにすること！
#define REMOTEXY_BLUETOOTH_NAME "TIPS-REMOTE"


#include <RemoteXY.h>

// RemoteXY GUI configuration  
#pragma pack(push, 1)  
uint8_t RemoteXY_CONF[] =   // 76 bytes
  { 255,7,0,0,0,69,0,19,0,0,0,84,73,80,83,45,66,79,84,0,
  31,1,200,84,1,1,5,0,5,13,21,60,60,32,2,26,31,5,127,21,
  60,60,32,177,26,31,1,78,24,44,17,3,249,31,49,0,1,78,42,44,
  17,3,249,31,50,0,1,78,60,44,17,3,249,31,51,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  int8_t joystick_01_x; // from -100 to 100
  int8_t joystick_01_y; // from -100 to 100
  int8_t joystick_02_x; // from -100 to 100
  int8_t joystick_02_y; // from -100 to 100
  uint8_t button_01; // =1 if button pressed, else =0
  uint8_t button_02; // =1 if button pressed, else =0
  uint8_t button_03; // =1 if button pressed, else =0

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   
#pragma pack(pop)
 
/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////


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

  Serial.begin(115200);
  Serial.println("hello from tiny soccer robot");

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
                       (float)(RemoteXY.joystick_02_x * -0.03)});
}
