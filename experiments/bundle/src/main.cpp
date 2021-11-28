#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include <Fonts/FreeSansBold12pt7b.h>

const uint8_t BUTTON_PIN = 5;
const uint8_t ENCODER_A_PIN = A5;   // PB02
const uint8_t ENCODER_B_PIN = A4;   // PA05

const uint8_t DISPLAY_SCK_PIN = 13;
const uint8_t DISPLAY_MOSI_PIN = 11;
const uint8_t DISPLAY_SS_PIN = 10;

const uint8_t COLOR_BLACK = 0;
const uint8_t COLOR_WHITE = 1;


Adafruit_SharpMem display(DISPLAY_SCK_PIN, DISPLAY_MOSI_PIN, DISPLAY_SS_PIN, 144, 168);

volatile int encoder_position;
volatile uint8_t encoder_state = 0;
volatile const int8_t offsets_lut[16] = {0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0};

void encoder_isr()
{
    encoder_state &= 0x03;
    encoder_state |= (REG_PORT_IN1 & (1 << 2)) | ((REG_PORT_IN0 & (1 << 5)) >> 2);
    encoder_position += offsets_lut[encoder_state];
    if (encoder_position < 0) {
        encoder_position = 0;
    }
    encoder_state >>= 2;
}

void print_duration(int seconds)
{
    display.setCursor(10, 50);

    if (seconds >= 3600) {
        display.print(seconds / 3600);
        display.print("h");
    }
    
    if (seconds >= 60) {
        display.print((seconds % 3600) / 60);
        display.print("m");
    }
    
    display.print(seconds % 60);
    display.print("s");
}

void setup()
{
    Serial.begin(9600);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(ENCODER_A_PIN, INPUT_PULLUP);
    pinMode(ENCODER_B_PIN, INPUT_PULLUP);

    display.begin();
    display.clearDisplay();

    display.setFont(&FreeSansBold12pt7b);
    // display.setTextSize(2);
    display.cp437(true);
    display.setTextColor(COLOR_BLACK);

    attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), encoder_isr, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_B_PIN), encoder_isr, CHANGE);
}

void loop()
{
    static int position = -1;
    static uint32_t ts_last_refresh = 0;

    if (encoder_position != position) {
        Serial.println(encoder_position);
        position = encoder_position;
        display.clearDisplay();
        print_duration(position * 30);
        display.refresh();
    }

    if (millis() - ts_last_refresh > 1500) {
        display.refresh();
        ts_last_refresh = millis();
    }
}
