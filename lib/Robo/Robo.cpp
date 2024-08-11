#include "Robo.h"

Robo::Robo(Rot_Servo& rot1, Rot_Servo& rot2, Rot_Servo& rot3, Servo& servo, Gyro& gyro)
   :
    vel_{0}, out_vel_{0}, error_{0}, integral_{0},
    rot1_{rot1},
    rot2_{rot2},
    rot3_{rot3},
    servo_{servo},
    gyro_{gyro}
{}

void Robo::setup() {
  servo_.set_angle(-90);
  delay(500);
  servo_.stop();
  rot1_.set_speed(0);
  rot2_.set_speed(0);
  rot3_.set_speed(0);
  gyro_.setup();
}

void Robo::update(xyz_t vel) {
    vel_ = vel;
}

void Robo::execute(xyz_t target_vel) {
  constexpr float MAX_SPEED = 400.0; // 最大速度(mm/s) 

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
  out_vel_.x = clamp(c * target_vel.x, MAX_SPEED);
  out_vel_.y = clamp(c * target_vel.y, MAX_SPEED);
  out_vel_.z = clamp(c * target_vel.z, MAX_SPEED);

  // サーボへの出力に変換
  rot1_.set_speed((0.866 * out_vel_.x + 0.500 * out_vel_.y + Robo::RADIUS * out_vel_.z) / MAX_SPEED);
  rot2_.set_speed((0.000 * out_vel_.x - 1.000 * out_vel_.y + Robo::RADIUS * out_vel_.z) / MAX_SPEED);
  rot3_.set_speed((-0.866 * out_vel_.x + 0.500 * out_vel_.y + Robo::RADIUS * out_vel_.z) / MAX_SPEED);
}

void Robo::kick()
{
  servo_.set_angle(-45);
  delay(300);
  servo_.set_angle(-90);
  delay(100);
  servo_.stop();
}
