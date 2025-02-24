#include "Robo.h"

Robo::Robo(Rot_Servo& rot1, Rot_Servo& rot2, Rot_Servo& rot3, Servo& servo, Gyro& gyro)
   :
    target_vel_{0},
    target_angle_(0),
    angle_error_(0),
    angle_integral_(0),
    vel_{0},
    rot1_{rot1},
    rot2_{rot2},
    rot3_{rot3},
    servo_{servo},
    gyro_{gyro},
    kicking_(false),
    kick_count_(0)
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

void Robo::execute(float cycle) {

  constexpr float MAX_SPEED = 300.0; // 最大速度(mm/s) 
  constexpr float MAX_OMEGA = 2; // 最大角速度(rad/s)

  auto clamp = [](float a, float limit) {
    if (a > limit) {
      a = limit;
    } else if (a < -limit) {
      a = -limit;
    }
    return a;
  };
   
  // 目標角度に目標速度を加える
  target_angle_ += clamp(target_vel_.z, MAX_OMEGA) * cycle / 1000.0;

  // 出力速度
  xyz_t out_vel = target_vel_;
  
  // 目標角度エラー
  float angle_error = target_angle_ - gyro_.angle();
  // +-PIに収める
  while (angle_error > PI) {
    angle_error -= 2 * PI;
  }
  while (angle_error < -PI) {
    angle_error += 2 * PI;
  }

  // 積分
  angle_integral_ += (angle_error + angle_error_) / 2 * cycle;
  // 微分
  float angle_diff = (angle_error - angle_error_) / cycle;

  Serial.printf(">angle_error:%f\n", (float)angle_error);
  Serial.printf(">error_integ:%f\n", (float)angle_integral_);
  Serial.printf(">error_diff:%f\n", (float)angle_diff);

  // 角速度に角度をフィードバック（PID）
  const float k_p = 8;
  const float k_i = 0;
  const float k_d = 0.005;
  out_vel.z = k_p * angle_error + k_i * angle_integral_ + k_d * angle_diff;

  angle_error_ = angle_error;

  // 速度調整用係数
  constexpr float c = 1.0;
 
  // 速度調整
  out_vel.x = clamp(c * out_vel.x, MAX_SPEED);
  out_vel.y = clamp(c * out_vel.y, MAX_SPEED);
  out_vel.z = clamp(c * out_vel.z, MAX_OMEGA);

  // サーボへの出力に変換
  rot1_.set_speed((0.866 * out_vel.x + 0.500 * out_vel.y + Robo::RADIUS * out_vel.z) / MAX_SPEED);
  rot2_.set_speed((0.000 * out_vel.x - 1.000 * out_vel.y + Robo::RADIUS * out_vel.z) / MAX_SPEED);
  rot3_.set_speed((-0.866 * out_vel.x + 0.500 * out_vel.y + Robo::RADIUS * out_vel.z) / MAX_SPEED);

  // キック動作
  if (kicking_)
  {
    if (kick_count_ < 17)
    {
      servo_.set_angle(-45);
    }
    else if (kick_count_ < 22)
    {
      servo_.set_angle(-90);
    }
    else
    {
      servo_.stop();
      kicking_ = false;
    }
    kick_count_++;
  }
}

void Robo::kick()
{
  if (!kicking_) {
    kick_count_ = 0;
    kicking_ = true;
  }
}

void Robo::stop() {
  servo_.stop();
  rot1_.stop();
  rot2_.stop();
  rot3_.stop();
}

void Robo::set_target_vel(xyz_t target_vel) {
  target_vel_ = target_vel;
}

void Robo::set_target_angle(float angle) {
  target_angle_ = angle;
}