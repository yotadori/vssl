// Udp_Receiver.h

#include "Arduino.h"

#include "WiFi.h"
#include "AsyncUDP.h"

class Udp_Receiver {
    public:
        Udp_Receiver(char* ssid, char* password);

        void setup();

    private:
        char * ssid_;
        char * password_;

        AsyncUDP udp_;
};
