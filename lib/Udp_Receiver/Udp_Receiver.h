// Udp_Receiver.h
#ifndef UDP_RECEIVER_H
#define UDP_RECEIVER_H 

#include "Arduino.h"

#include "WiFi.h"
#include "AsyncUDP.h"

#include "Context.h"

class Udp_Receiver {
    public:
        Udp_Receiver(char* ssid, char* password);

        void setup();

        xyz_t vel();

        bool kick_flag();

        float updated_time();

    private:
        char * ssid_;
        char * password_;

        AsyncUDP udp_;

        xyz_t vel_;
        bool kick_flag_;        

        float last_updated_time_;

        /**
         * @brief 値を更新
        */
        void update(uint8_t* data);
};

#endif // UDP_RECEIVER_H