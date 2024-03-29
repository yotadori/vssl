// Speaker.cpp

#include "Speaker.h"

Speaker::Speaker (int channel, int pin) :
channel_(channel)
{
    ledcSetup(channel, 12000, 8);    
    ledcAttachPin(pin, channel);
}

void Speaker::beep(int tone) {
    static const int tones[] = {262, 294, 330, 349, 392, 440, 494, 523, 587};

    ledcWriteTone(channel_, tones[tone]);
}

void Speaker::stop() {
    ledcWriteTone(channel_, 0);
}


