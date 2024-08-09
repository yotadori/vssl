// Dabble_Receiver.h
#ifndef DABBLE_RECEIVER_H
#define DABBLE_RECEIVER_H

#include "Arduino.h"

#include "Receiver.h"

class Dabble_Receiver : public Receiver {
    public:
        Dabble_Receiver();

        void setup() override;

        void update() override;

    private:

};

#endif // DABBLE_RECEIVER_H