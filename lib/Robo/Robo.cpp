#include "Robo.h"

Robo::Robo(Rot_Servo& rot1, Rot_Servo& rot2, Rot_Servo& rot3, Servo& servo, Gyro& gyro)
   :
    target_vel_{0},
    last_vel_{0},
    last_omega_error_(0),
    omega_error_integral_(0),
    vel_{0},
    rot1_{rot1},
    rot2_{rot2},
    rot3_{rot3},
    servo_{servo},
    gyro_{gyro},
    kicking_(false),
    kick_count_(0),
    default_speed_(100),
    default_omega_(3),
    use_gyro_(false)
{}

void Robo::setup() {
  servo_.set_angle(90);
  delay(500);
  servo_.stop();
  rot1_.set_speed(0);
  rot2_.set_speed(0);
  rot3_.set_speed(0);
  gyro_.setup();
}

void Robo::execute(float cycle) {

  auto clamp = [](float a, float min, float max) {
    if (a > max) {
      a = max;
    } else if (a < min) {
      a = min;
    }
    return a;
  };
   
  // 出力速度
  xyz_t out_vel = target_vel_;

  if (use_gyro_)
  {
    // 目標角速度エラー
    float omega_error = target_vel_.z - (-gyro_.gyro().z);

    // 積分
    omega_error_integral_ += omega_error * cycle;
    // 微分
    float omega_error_diff = (omega_error - last_omega_error_) / cycle;

    //Serial.printf(">omega_error:%f\n", (float)omega_error);

    // 角速度をフィードバック（PID）
    const float k_p = 0.1;
    const float k_i = 20;
    const float k_d = 0.0001;
    out_vel.z += k_p * omega_error + k_i * omega_error_integral_ + k_d * omega_error_diff;

    last_omega_error_ = omega_error;
  }

  // 速度調整用係数
  constexpr float c = 1.0;

  // 加速度制限
  const float d_speed = MAX_ACC * cycle;
  out_vel.x = clamp(c * out_vel.x, last_vel_.x - d_speed, last_vel_.x + d_speed);
  out_vel.y = clamp(c * out_vel.y, last_vel_.y - d_speed, last_vel_.y + d_speed);
 
  // 速度調整
  out_vel.x = clamp(c * out_vel.x, -Robo::MAX_SPEED, Robo::MAX_SPEED);
  out_vel.y = clamp(c * out_vel.y, -Robo::MAX_SPEED, Robo::MAX_SPEED);
  out_vel.z = clamp(c * out_vel.z, -Robo::MAX_OMEGA, Robo::MAX_SPEED);

  last_vel_ = out_vel;

  // サーボへの出力に変換
  rot1_.set_speed((0.866 * out_vel.x + 0.500 * out_vel.y + Robo::RADIUS * out_vel.z) / Robo::MAX_SPEED);
  rot2_.set_speed(-(0.000 * out_vel.x - 1.000 * out_vel.y + Robo::RADIUS * out_vel.z) / Robo::MAX_SPEED);
  rot3_.set_speed((-0.866 * out_vel.x + 0.500 * out_vel.y + Robo::RADIUS * out_vel.z) / Robo::MAX_SPEED);

  // キック動作
  if (kicking_)
  {
    if (kick_count_ < 200)
    {
      // 打つ
      servo_.set_angle(90);
    }
    else if (kick_count_ < 600) {
      // ひっかける
      servo_.set_angle(-25);
    }
    else if (kick_count_ < 800) {
      // 戻す
      servo_.set_angle(-25 + (25 + 65) * (kick_count_ - 600) / 200);
      //servo_.set_angle(30);
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
  target_vel_ = {0, 0, 0};
  servo_.stop();
  rot1_.stop();
  rot2_.stop();
  rot3_.stop();
}

void Robo::move_mm(float x, float y) {
  target_vel_.z = 0;
  float distance = sqrt(x * x + y * y);
  target_vel_.x = default_speed_ * x / distance;
  target_vel_.y = default_speed_ * y / distance;
  delay(distance / default_speed_ * 1000);
  target_vel_.x = 0;
  target_vel_.y = 0;
}

void Robo::turn_degree(float deg) {
  float rad = deg * PI / 180.0;
  target_vel_.z = rad > 0 ? default_omega_: -default_omega_;
  delay(abs(rad) / default_omega_ * 1000);
  target_vel_.z = 0;
}

void Robo::set_target_vel(xyz_t target_vel) {
  target_vel_ = target_vel;
}

void Robo::set_target_vel(float vx, float vy, float omega) {
  target_vel_ = xyz_t{vx, vy, omega};
}

void Robo::set_default_speed(float speed) {
  default_speed_ = min(speed, Robo::MAX_SPEED);
}

void Robo::set_default_omega(float omega) {
  default_omega_ = omega;
}

void Robo::set_use_gyro(boolean flag) {
  use_gyro_ = flag;
}

bool Robo::kicking() {
  return kicking_;
}