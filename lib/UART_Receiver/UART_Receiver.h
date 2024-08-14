// UART_Receiver.h
#ifndef UART_RECEIVER_H
#define UART_RECEIVER_H

#include "Arduino.h"

#include "Receiver.h"
#include <HardwareSerial.h>

class UART_Receiver : public Receiver {
    public:
        UART_Receiver();

        void setup() override;

        void update() override;

    private:

};

#endif // UART_Receiver