// Gyro.cpp

#include "Gyro.h"

#include <Arduino.h>
#include<Wire.h>
const int MPU_addr=0x68;  // I2C address of the MPU-6050

Gyro::Gyro() :
    acc_{0}, gyro_{0}, drift_{0}, tmp_(0), vel_{0}, angle_(0)
{}

void Gyro::setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  // 補正値を安定させる
  /*
  for (int i = 0; i < 100; i++) {
    update();
    delay(50);
  }
  */
  vel_.x = 0;
  vel_.y = 0;
  vel_.z = 0;
}

void Gyro::update() {
  // センサの値を取得
  int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14, true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  // 単位変換 mm*s-2, ℃, rad*s-1
  xyz_t acc;
  acc.x = AcX / 65536.0 * 4.0 * 9800;
  acc.y = AcY / 65536.0 * 4.0 * 9800;
  acc.z = AcZ / 65536.0 * 4.0 * 9800;
  tmp_ = Tmp/340.00+36.53;
  //gyro_.x = GyX / 65536.0 * 500.0 / 360.0 * 6.283;
  gyro_.x = GyX * 0.0000009026;

  // alpha is calculated as t / (t + dT)
  // with t, the low-pass filter's time-constant
  // and dT, the event delivery rate
  const float alpha = 0.99;

  // 重力成分
  gravity_.x = alpha * gravity_.x + (1 - alpha) * acc.x;
  gravity_.y = alpha * gravity_.y + (1 - alpha) * acc.y;
  gravity_.z = alpha * gravity_.z + (1 - alpha) * acc.z;

  // 重力成分除去
  acc_.x = acc.x - gravity_.x;
  acc_.y = acc.y - gravity_.y;
  acc_.z = acc.z - gravity_.z;

  if (abs(gyro_.x) < 0.001)
  {
    // 角速度が小さい時
    // ジャイロのドリフト成分を計算する
    drift_.x = alpha * drift_.x + (1 - alpha) * gyro_.x;
  }

  // ドリフト成分除去
  gyro_.x = gyro_.x - drift_.x;

  // 前回計算した時から今までの経過時間を算出
  static float preInterval = 0.0;
  static float interval = millis() - preInterval;
  preInterval = millis();

  const float beta = 0.99;

  // 加速度を積分して速度を算出    
  // ここでもLPFをかける
  vel_.x = beta * vel_.x + (1 - beta) * (vel_.x + acc_.x * interval * 0.001);
  vel_.y = beta * vel_.y + (1 - beta) * (vel_.y + acc_.y * interval * 0.001);
  vel_.z = beta * vel_.z + (1 - beta) * (vel_.z + acc_.z * interval * 0.001);

  // 角速度を積分して角度を算出
  angle_ = angle_ + (gyro_.x * interval * 0.001);
  Serial.printf(">angle:%f\n", (float)angle_);
  Serial.printf(">gyro:%f\n", (float)gyro_.x);
}

xyz_t Gyro::acc() {
    return acc_;
}

xyz_t Gyro::gyro() {
    return gyro_;
}

xyz_t Gyro::vel() {
    return vel_;
}

float Gyro::angle()
{
  return angle_;
}