// Udp.cpp

#include "Udp_Receiver.h"

Udp_Receiver::Udp_Receiver(char* ssid, char* password) : ssid_{ssid}, password_{password}, udp_{} {}

void Udp_Receiver::setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid_, password_);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
        while(1) {
            delay(1000);
        }
    }
    if(udp_.listenMulticast(IPAddress(224, 4, 23, 4), 10004)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        udp_.onPacket([](AsyncUDPPacket packet) {
            Serial.print("UDP Packet Type: ");
            Serial.print(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
            Serial.print(", From: ");
            Serial.print(packet.remoteIP());
            Serial.print(":");
            Serial.print(packet.remotePort());
            Serial.print(", To: ");
            Serial.print(packet.localIP());
            Serial.print(":");
            Serial.print(packet.localPort());
            Serial.print(", Length: ");
            Serial.print(packet.length());
            Serial.print(", Data: ");
            Serial.write(packet.data(), packet.length());
            Serial.println();
            //reply to the client
            packet.printf("Got %u bytes of data", packet.length());
        });
        //Send multicast
        udp_.print("Hello!");
    }
}