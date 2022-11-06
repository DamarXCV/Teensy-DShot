/***************************************************************************
 * Example sketch for the Teensy-DShot library
 *
 * This example initiates the ESCs, increases the throttle to maximum speed,
 * and then decreases the throttle to zero.
 *
 ***************************************************************************/
#include <Arduino.h>

#include "DShot.h"

constexpr uint16_t LOOP_HZ = 2000;

DShot motor0(&Serial2, DShotType::DShot600); // Teensy4.X Pin 8
DShot motor1(&Serial3, DShotType::DShot600); // Teensy4.X Pin 14
DShot motor2(&Serial4, DShotType::DShot600); // Teensy4.X Pin 17
DShot motor3(&Serial5, DShotType::DShot600); // Teensy4.X Pin 20

void setup()
{
    // Setup blink led
    pinMode(LED_BUILTIN, OUTPUT);

    // Wait for motor init
    for (size_t i = 0; i < 4000; i++) {
        motor0.sendCommand(0, false);
        motor1.sendCommand(0, false);
        motor2.sendCommand(0, false);
        motor3.sendCommand(0, false);
        delayMicroseconds(1'000);
    }
}

uint64_t counter = 0;
int16_t throttle = 0;
int8_t throttleChange = 1;
uint8_t ledState = false;

void loop()
{
    uint32_t loopCycleStart = micros();

    // Set throttle
    motor0.sendThrottle(throttle, false);
    motor1.sendThrottle(throttle, false);
    motor2.sendThrottle(throttle, false);
    motor3.sendThrottle(throttle, false);

    // Decrease throttle if max reached
    if (throttle >= 1999) {
        throttleChange = -1;
    }

    // Increase or decrease throttle
    if (counter % 20 == 0 && !(throttle == 0 && throttleChange == -1)) {
        throttle += throttleChange;
    }

    // Blink the LED
    if (counter % LOOP_HZ == 0) {
        digitalWrite(LED_BUILTIN, ledState ^= 1);
    }

    ++counter;
    delayMicroseconds((1'000'000 / LOOP_HZ) - (micros() - loopCycleStart));
}
