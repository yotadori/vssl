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

