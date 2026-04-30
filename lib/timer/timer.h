#pragma once
#include <Arduino.h>

/**
 * @class Timer
 * @brief A simple timer utility class for managing time-based operations.
 *
 * The Timer class provides functionality to start, reset, and check the status
 * of a timer. It uses the `millis()` function to track elapsed time and determine
 * whether the timer has expired or is still running. Additionally, it provides
 * methods to retrieve the elapsed and remaining time.
 *
 * Example usage:
 * @code
 * Timer myTimer;
 * myTimer.start(5000); // Start a timer for 5 seconds
 *
 * while (!myTimer.isExpired()) {
 *     // Perform some operations while the timer is running
 *     Serial.println("Timer is running...");
 * }
 *
 * Serial.println("Timer expired!");
 * @endcode
 *
 */
class Timer
{
private:
    unsigned long startTime;
    unsigned long duration;

public:
    Timer() : startTime(0), duration(0) {}

    /**
     * @brief Starts the timer with a specified duration.
     *
     * This function initializes the timer by setting the duration and recording
     * the current time as the start time.
     *
     * @param duration The duration for the timer in milliseconds.
     */
    void start(unsigned long _duration)
    {
        duration = _duration;
        startTime = millis();
    }

    void stop()
    {
        duration = 0;
    }

    /**
     * @brief Resets the timer by updating the start time to the current time.
     *
     * This function sets the `startTime` variable to the value returned by `millis()`,
     * effectively restarting the timer from the current moment.
     */
    void reset()
    {
        startTime = millis();
    }

    /**
     * @brief Checks if the timer has expired.
     *
     * This function determines whether the specified duration has elapsed
     * since the timer was started. It compares the current time (retrieved
     * using `millis()`) with the start time and the duration.
     *
     * @return true if the timer has expired (i.e., the elapsed time is
     *         greater than or equal to the duration), false otherwise.
     */
    bool isExpired()
    {
        return (millis() - startTime) >= duration;
    }

    /**
     * @brief Checks if the timer is still running.
     *
     * This function determines whether the timer is currently active by comparing
     * the elapsed time since the timer started with the specified duration.
     *
     * @return true if the timer is still running (elapsed time is less than the duration),
     *         false otherwise.
     */
    bool isRunning()
    {
        return (millis() - startTime) < duration;
    }

    /**
     * @brief Calculates the elapsed time since the timer was started.
     *
     * @return The elapsed time in milliseconds as an unsigned long.
     */
    unsigned long elapsed()
    {
        if (duration == 0)
            return 0; // Timer is not running, return 0 elapsed time
        return millis() - startTime;
    }

    /**
     * @brief Calculates the remaining time for the timer.
     *
     * This function computes the remaining time by subtracting the elapsed time
     * from the total duration of the timer.
     *
     * @return The remaining time in milliseconds as an unsigned long.
     */
    unsigned long remaining()
    {
        return duration - elapsed();
    }
};
