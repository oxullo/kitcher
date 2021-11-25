#include <Arduino.h>
#include <RotaryEncoder.h>

const uint8_t BUTTON_PIN = 5;
const uint8_t ENCODER_A_PIN = A5;   // PB02
const uint8_t ENCODER_B_PIN = A4;   // PA05

volatile int encoder_position;
volatile uint8_t encoder_state = 0;
volatile const int8_t offsets_lut[16] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};


void encoder_isr()
{
    encoder_state &= 0x03;
    encoder_state |= REG_PORT_IN1 & (1 << 2);
    encoder_state |= (REG_PORT_IN0 & (1 << 5)) >> 2;
    encoder_position += offsets_lut[encoder_state];
    encoder_state >>= 2;
}

void setup()
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(ENCODER_A_PIN, INPUT_PULLUP);
    pinMode(ENCODER_B_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), encoder_isr, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_B_PIN), encoder_isr, CHANGE);
}

void loop()
{
    static int position = 0;

    digitalWrite(LED_BUILTIN, digitalRead(BUTTON_PIN));

    if (encoder_position != position) {
        Serial.println(encoder_position);
        position = encoder_position;
    }
}
