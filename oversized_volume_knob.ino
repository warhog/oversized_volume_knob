#include <ClickEncoder.h>
#include <HID-Project.h>
#include <TimerOne.h>

// definition of the used pins
#define ENCODER_CLK A0
#define ENCODER_DT A1
#define ENCODER_SW A2

// the 4th parameter sets the number of steps per notch
ClickEncoder encoder(ENCODER_DT, ENCODER_CLK, ENCODER_SW, 4);
int16_t oldValue = -1;
int16_t value = -1;

// timer 1 ISR
void timer1Isr() {
    encoder.service();
}

void setup() {
    // start media key consumer
    Consumer.begin();

    // start TimerOne with 1000 microseconds
    // according to the clickencoder spec the service routine should be called every millisecond for correct acceleration detection
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timer1Isr);
}

void loop() {
    // get the current value from the encoder
    value += encoder.getValue();

    // if the value changed the knob was turned
    if (value != oldValue) {
        if (oldValue < value) {
            Consumer.write(MEDIA_VOLUME_UP);
        } else {
            Consumer.write(MEDIA_VOLUME_DOWN);
        }
        oldValue = value;
    }

    // check the button state
    ClickEncoder::Button buttonState = encoder.getButton();
    if (buttonState != ClickEncoder::Open) {
        switch (buttonState) {
        case ClickEncoder::Clicked:
            Consumer.write(MEDIA_VOLUME_MUTE);
            break;
        case ClickEncoder::DoubleClicked:
            Consumer.write(MEDIA_PLAY_PAUSE);
            break;
        }
    }
}