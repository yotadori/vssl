// Speaker.cpp

#include "Speaker.h"

Speaker::Speaker (int channel, int pin) :
channel_(channel), melody_{0}, counter_(0)
{
    ledcSetup(channel, 12000, 8);    
    ledcAttachPin(pin, channel);

}

void Speaker::update() {
    if (melody_[melody_step_].tone == Speaker::STOP) {
        stop();
    } else if (melody_[melody_step_].tone == Speaker::REPEAT) {
        counter_ = 0;
        melody_step_ = 0;
        beep(melody_[melody_step_].tone);
    } else if (counter_ < melody_[melody_step_].time) {
        counter_++;
    } else {
        counter_ = 0;
        melody_step_++;
        beep(melody_[melody_step_].tone);
    }
}

void Speaker::beep(int tone) {
    static const int tones[] = {0, 262, 294, 330, 349, 392, 440, 494, 523, 587};

    ledcWriteTone(channel_, tones[tone]);
}

void Speaker::stop() {
    ledcWriteTone(channel_, 0);
}

void Speaker::set_melody(tone_type melody[]) {
    counter_ = 0;
    melody_step_ = 0;

    for (int i = 0; i < 99; i++) {
        melody_[i] = melody[i];
        if (melody[i].tone == -1) {
            break;
        }
    }
    
    beep(melody_[melody_step_].tone);
}
