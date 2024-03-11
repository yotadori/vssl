#include "Robo.h"

Robo::Robo() :
    vel_{0}, out_vel_{0}, error_{0}, integral_{0}
{}

void Robo::update(xyz_t vel) {
    vel_ = vel;
}

xyz_t Robo::execute(xyz_t target_vel) {
  auto clamp = [](float a, float limit) {
    if (a > limit) {
      a = limit;
    } else if (a < -limit) {
      a = -limit;
    }
    return a;
  };

  xyz_t prev = error_; 
  error_.x = target_vel.x - vel_.x;
  error_.y = target_vel.y - vel_.y;
  error_.z = target_vel.z - vel_.z;

  // 前回計算した時から今までの経過時間を算出
  static float preInterval = 0.0;
  static float interval = millis() - preInterval;
  preInterval = millis();

  // 積分
  integral_.x += (prev.x + error_.x) / 2.0 * interval * 0.001;
  integral_.y += (prev.y + error_.y) / 2.0 * interval * 0.001;
  integral_.z += (prev.z + error_.z) / 2.0 * interval * 0.001;

  // 微分
  xyz_t diff{0};
  diff.x = (error_.x - prev.x) / (interval * 0.001);
  diff.y = (error_.y - prev.y) / (interval * 0.001);
  diff.z = (error_.z - prev.z) / (interval * 0.001);

  float c = 0.7;
  // PIDパラメータ
  float p = 0.1;
  float i = 0;
  float d = 0.005;

  // PIDの式
  out_vel_.x = clamp(c * target_vel.x + p * error_.x + i * integral_.x + d * diff.x, 400); 
  out_vel_.y = clamp(c * target_vel.y + p * error_.y + i * integral_.y + d * diff.y, 400); 
  out_vel_.z = clamp(c * target_vel.z + p * error_.z + i * integral_.z + d * diff.z, 400); 

  return out_vel_;
}
