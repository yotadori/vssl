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
  
  // 速度調整用係数
  constexpr float c = 1.0;

  // PIDの式
  out_vel_.x = clamp(c * target_vel.x, 400); 
  out_vel_.y = clamp(c * target_vel.y, 400); 
  out_vel_.z = clamp(c * target_vel.z, 400); 

  return out_vel_;
}
