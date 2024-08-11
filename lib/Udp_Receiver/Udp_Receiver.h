// Udp_Receiver.h
#ifndef UDP_RECEIVER_H
#define UDP_RECEIVER_H 

#include "Arduino.h"

#include "WiFi.h"
#include "AsyncUDP.h"

#include "Context.h"

#include "Receiver.h"

class Udp_Receiver : public Receiver {
    public:
        Udp_Receiver(char* ssid, char* password);

        void setup() override;

    private:
        char * ssid_;
        char * password_;

        AsyncUDP udp_;

        /**
         * @brief 値を更新
        */
        void update_data(uint8_t* data);
};

#endif // UDP_RECEIVER_H