// RemoteXY_Receiver.h
#ifndef REMOTEXY_RECEIVER_H
#define REMOTEXY_RECEIVER_H

#include "Arduino.h"

#include "Receiver.h"
#include <HardwareSerial.h>

class RemoteXY_Receiver : public Receiver {
    public:
        RemoteXY_Receiver();

        void setup() override;

        void update() override;

        void set_values(xyz_t vel,  bool kick);

    private:

};

#endif // RemoteXY_Receiver