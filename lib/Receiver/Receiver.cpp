// Receiver.cpp

#include "Receiver.h"

#include <Arduino.h>

Receiver::Receiver() : kick_flag_(false),
                       last_updated_time_(0) {}

xyz_t Receiver::vel() {
    return vel_;
} 

bool Receiver::kick_flag() {
    return kick_flag_;
}

float Receiver::updated_time() {
    return last_updated_time_;
}

void Receiver::setup() {}

void Receiver::update() {}