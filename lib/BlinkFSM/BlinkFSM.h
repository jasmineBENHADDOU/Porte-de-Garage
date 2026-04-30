#pragma once
#include "FSM.h"

enum BlinkState
{
    BLINK_OFF,
    BLINK_ON
};

class BlinkFSM
{
private:
    uint8_t pin;
    unsigned long interval;
    FSM fsm;
    bool active = false;

public:
    BlinkFSM(uint8_t pin, unsigned long intervalMs = 500)
        : pin(pin), interval(intervalMs)
    {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);

        fsm.ADD_STATE(BLINK_OFF)
            .ADD_STATE(BLINK_ON)

            .addTransition(BLINK_OFF, BLINK_ON, [this]()
                           { return fsm.timeInState() >= interval; })
            .addTransition(BLINK_ON, BLINK_OFF, [this]()
                           { return fsm.timeInState() >= interval; })

            .onEnter(BLINK_OFF, [this]()
                     {
                         digitalWrite(this->pin, LOW);
                         // Serial.print("Entering state: BLINK_OFF on pin ");
                         // Serial.println(this->pin);
                     })
            .onEnter(BLINK_ON, [this]()
                     {
                         digitalWrite(this->pin, HIGH);
                         // Serial.print("Entering state: BLINK_ON on pin ");
                         // Serial.println(this->pin);
                     });
    }

    void start()
    {
        // Serial.println("Starting BlinkFSM...");
        fsm.start(BLINK_OFF);
        active = true;
    }

    void stop()
    {
        // Serial.println("Stopping BlinkFSM...");
        active = false;
        digitalWrite(pin, LOW);
    }

    void update()
    {
        if (active)
        {
            fsm.update();
        }
    }
};
