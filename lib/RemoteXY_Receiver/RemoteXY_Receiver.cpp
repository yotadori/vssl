// RemoteXY_Receiver.cpp

#include "RemoteXY_Receiver.h"

RemoteXY_Receiver::RemoteXY_Receiver() : Receiver()
{
}

void RemoteXY_Receiver::setup() {
}

void RemoteXY_Receiver::update()
{
    last_updated_time_ = millis();
}

void RemoteXY_Receiver::set_values(xyz_t vel, bool kick) {
    vel_ = vel;
    kick_flag_ = kick;
}