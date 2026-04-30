
/**
 * @file leds.h
 * @brief A lightweight library for controlling multiple LEDs using an array of GPIO pins.
 *
 * This header file defines the `Leds` class, which provides an easy-to-use interface
 * for managing multiple LEDs connected to an Arduino or compatible microcontroller.
 * The class allows for configuring the LED pins, blinking individual LEDs, and blinking
 * all LEDs sequentially.
 *
 * ## Example Usage
 * ```cpp
 * #include "leds.h"
 *
 * const int ledPins[] = {32, 23};
 * Leds leds(ledPins, sizeof(ledPins) / sizeof(ledPins[0]));
 *
 * void setup() {
 *     leds.setup(); // Configure the LED pins as output
 * }
 *
 * void loop() {
 *     leds.blinkAll(1000); // Blink all LEDs with a 1-second duration
 * }
 * ```
 *
 * ## Features
 * - Configure multiple GPIO pins as outputs for LEDs.
 * - Blink individual LEDs with a specified duration.
 * - Blink all LEDs sequentially with a uniform duration.
 *
 * ## Notes
 * - Ensure that the `ledPins` array is properly defined and passed to the `Leds` constructor.
 * - The `setup()` method must be called before using other methods to ensure the pins are configured correctly.
 *
 * @author bzctoons
 * @date 2023
 */

#pragma once
#include <Arduino.h>

class Leds
{
private:
    const int *ledPins = nullptr;
    int numLeds;

public:
    /**
     * @brief Constructor for the Leds class.
     *
     * @param pins Pointer to an array of integers representing the GPIO pins connected to the LEDs.
     * @param count The number of LEDs (or pins) in the array.
     */
    Leds(const int *pins, int count) : ledPins(pins), numLeds(count) {}

    /**
     * @brief Configures the pins connected to the LEDs as output pins.
     *
     * This function iterates through the array of LED pins (`ledPins`) and sets
     * each pin's mode to OUTPUT using the `pinMode` function. The number of LEDs
     * is determined by the `numLeds` variable.
     *
     * @note Ensure that `ledPins` and `numLeds` are properly defined and initialized
     *       before calling this function.
     */
    void setup()
    {
        if (ledPins == nullptr)
            return;

        for (int i = 0; i < numLeds; i++)
        {
            pinMode(ledPins[i], OUTPUT);
        }
    }

    /**
     * @brief Blinks an LED connected to the specified pin synchronously.
     *
     * This function sets the specified pin to HIGH for the given duration,
     * then sets it back to LOW, effectively creating a single blink.
     *
     * @param pin The GPIO pin number where the LED is connected.
     * @param duration The duration (in milliseconds) for which the LED stays ON.
     */
    void blinkSync(int pin, int duration)
    {
        digitalWrite(pin, HIGH);
        delay(duration);
        digitalWrite(pin, LOW);
    }

    /**
     * @brief Blinks all LEDs connected to the pins specified in the ledPins array.
     *
     * This function iterates through all the LED pins defined in the ledPins array
     * and calls the blinkSync function for each pin, causing all LEDs to blink
     * sequentially with the specified duration.
     *
     * @param duration The duration (in milliseconds) for which each LED should blink.
     */
    void blinkAll(int duration, int times = 1)
    {
        if (ledPins == nullptr)
            return;

        for (int i = 0; i < times; i++)
        {
            for (int i = 0; i < numLeds; i++)
            {
                blinkSync(ledPins[i], duration);
            }
        }
    }
};
