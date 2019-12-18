#include "common.h"
#include "server.h"

// SoftwareSerial g_soft_serial(PIN_SOFTWARE_SERIAL_RX, PIN_SOFTWARE_SERIAL_TX);
// Server server(&g_soft_serial, 9600);
Server server(&Serial);

void pedalInterrupt() {
    server.pedalInterrupt();
}

void setup() {
    Serial.begin(115200);
    pinMode(PIN_PEDAL, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_PEDAL), pedalInterrupt, RISING);
    server.start();
}

void loop() {
    // Serial.println("Loop");
    server.loop();
    // g_soft_serial.print("HelloWorld");
    // if (g_soft_serial.available())
    // {
    //     Serial.print("text=");
    //     Serial.println(g_soft_serial.readString());
    //     // screen_print(buffer.c_str());
    // }
    // else
    // {
    //     Serial.println("unavailable");
    // }
}
