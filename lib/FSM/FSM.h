#pragma once
#include <Arduino.h>
#include <functional>
#include <map>
#include <vector>

#define ADD_STATE(state) addState(state, #state)

using namespace std;

class FSM
{
private:
    using State = int;
    using ConditionFunc = function<bool(void)>;
    using CallbackFunc = function<void(void)>;

    struct Transition
    {
        State from;
        State to;
        ConditionFunc condition;
    };

    /// Debug flags
    bool debugEnter = false;
    bool debugExit = false;
    bool debugTransition = false;

    vector<State> states;
    std::map<State, const char *> stateNames;
    vector<Transition> transitions;
    std::map<State, CallbackFunc> enterCallbacks;
    std::map<State, CallbackFunc> exitCallbacks;

    State currentState;
    State previousState;
    unsigned long stateStartTime;

public:
    // Constructor
    FSM() : currentState(-1), previousState(-1), stateStartTime(0) {}

    FSM &addState(State s, const char *name)
    {
        states.push_back(s);
        stateNames[s] = name;
        return *this;
    }

    FSM &addTransition(State from, State to, ConditionFunc condition)
    {
        transitions.push_back({from, to, condition});
        return *this;
    }

    // Callbacks

    FSM &onEnter(State s, CallbackFunc func)
    {
        enterCallbacks[s] = func;
        return *this;
    }

    FSM &onExit(State s, CallbackFunc func)
    {
        exitCallbacks[s] = func;
        return *this;
    }

    // Start the FSM with an initial state
    void start(State initial)
    {
        currentState = initial;
        stateStartTime = millis();
        if (enterCallbacks.count(currentState))
            enterCallbacks[currentState]();
    }

    // update the FSM
    void update()
    {
        for (auto &t : transitions)
        {
            if (t.from == currentState && t.condition())
            {

                if (debugTransition)
                {
                    Serial.print("[T] ");
                    Serial.print(stateNames[currentState]);
                    Serial.print(" -> ");
                    Serial.println(stateNames[t.to]);
                }

                // Exit current state
                if (exitCallbacks.count(currentState))
                {
                    if (debugExit)
                    {
                        Serial.print("[OUT] ");
                        Serial.println(stateNames[currentState]);
                    }
                    exitCallbacks[currentState]();
                }

                previousState = currentState;
                currentState = t.to;
                stateStartTime = millis();

                // Enter new state
                if (enterCallbacks.count(currentState))
                {
                    if (debugEnter)
                    {
                        Serial.print("[IN] ");
                        Serial.println(stateNames[currentState]);
                    }
                    enterCallbacks[currentState]();
                }

                return;
            }
        }
    }

    // debug functions
    FSM &setDebugEnter(bool enable)
    {
        debugEnter = enable;
        return *this;
    }

    FSM &setDebugExit(bool enable)
    {
        debugExit = enable;
        return *this;
    }

    FSM &setDebugTransition(bool enable)
    {
        debugTransition = enable;
        return *this;
    }

    FSM &setDebugAll(bool enable)
    {
        debugEnter = debugExit = debugTransition = enable;
        return *this;
    }

    /// Getters
    State getState() const { return currentState; }
    State getPrevious() const { return previousState; }
    unsigned long timeInState() const { return millis() - stateStartTime; }
};
