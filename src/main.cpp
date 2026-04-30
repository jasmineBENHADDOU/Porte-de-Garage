#include <Arduino.h>
#include <FSM.h>
#include "config.h"

// =====================================================
// TEMPORISATIONS
// =====================================================
const unsigned long DEBOUNCE_MS = 50;
const unsigned long APPUI_LONG_MS = 10000;
const unsigned long TEMPS_MOUVEMENT_MAX = 10000;
const unsigned long LED_LOCK_PULSE_MS = 200;

// =====================================================
// ETATS
// =====================================================
enum State
{
    INIT,
    ARRET_BAS,
    ARRET_HAUT,
    ARRET_INTERMEDIAIRE,
    MONTEE,
    DESCENTE,
    VERROUILLE,

    STATE_COUNT
};

FSM fsm;

// =====================================================
// EVENEMENTS
// =====================================================
bool evtAppuiCourt = false;
bool evtAppuiLong = false;

// true  = dernier mouvement montée
// false = dernier mouvement descente
bool dernierMouvementMontee = false;

// =====================================================
// STRUCTURE BOUTON
// =====================================================
struct BoutonPrincipal
{
    int lastReading = HIGH;
    int stableState = HIGH;
    unsigned long lastChange = 0;

    bool pressed = false;
    bool longAlreadyDetected = false;
    bool releasedSinceLong = true;
    unsigned long pressStart = 0;
};

BoutonPrincipal bouton;

// =====================================================
// LED INDICATEUR D'APPUI
// =====================================================
bool ledPulseActive = false;
unsigned long ledPulseStart = 0;

void pulseLedLock()
{
    digitalWrite(LED_LOCK, HIGH);
    ledPulseStart = millis();
    ledPulseActive = true;
}

void updateLedLock()
{
    if (!ledPulseActive)
        return;

    if (millis() - ledPulseStart < LED_LOCK_PULSE_MS)
        return;

    digitalWrite(LED_LOCK, LOW);
    ledPulseActive = false;
}

// =====================================================
// MOTEUR / LEDS
// =====================================================
void moteurStop()
{
    digitalWrite(LED_MONTEE, LOW);
    digitalWrite(LED_DESCENTE, LOW);
}

void moteurMontee()
{
    digitalWrite(LED_MONTEE, HIGH);
    digitalWrite(LED_DESCENTE, LOW);
}

void moteurDescente()
{
    digitalWrite(LED_MONTEE, LOW);
    digitalWrite(LED_DESCENTE, HIGH);
}

// =====================================================
// FINS DE COURSE
// =====================================================
bool finCourseHaut()
{
    return digitalRead(BTN_FDC_HAUT) == LOW;
}

bool finCourseBas()
{
    return digitalRead(BTN_FDC_BAS) == LOW;
}

// =====================================================
// LECTURE BOUTON PRINCIPAL
// =====================================================
void traiterBoutonAppuye()
{
    bouton.pressed = true;
    bouton.longAlreadyDetected = false;
    bouton.pressStart = millis();

    pulseLedLock();
}

void traiterBoutonRelache()
{
    if (!bouton.pressed)
        return;

    if (!bouton.longAlreadyDetected)
    {
        evtAppuiCourt = true;
        Serial.println("Evenement : appui court");
    }

    bouton.pressed = false;
    bouton.releasedSinceLong = true;
}

void detecterAppuiLong()
{
    if (!bouton.pressed)
        return;

    if (!bouton.releasedSinceLong)
        return;

    if (millis() - bouton.pressStart < APPUI_LONG_MS)
        return;

    evtAppuiLong = true;
    bouton.longAlreadyDetected = true;
    bouton.releasedSinceLong = false;

    Serial.println("Evenement : appui long 10s");
}

void lireBoutonPrincipal()
{
    int reading = digitalRead(BTN_MAIN);

    if (reading != bouton.lastReading)
    {
        bouton.lastChange = millis();
        bouton.lastReading = reading;
    }

    if (millis() - bouton.lastChange <= DEBOUNCE_MS)
    {
        detecterAppuiLong();
        return;
    }

    if (reading == bouton.stableState)
    {
        detecterAppuiLong();
        return;
    }

    bouton.stableState = reading;

    if (bouton.stableState == LOW)
        traiterBoutonAppuye();
    else
        traiterBoutonRelache();

    detecterAppuiLong();
}

// =====================================================
// CONDITIONS FSM
// =====================================================
bool mouvementMaxAtteint()
{
    return fsm.timeInState() >= TEMPS_MOUVEMENT_MAX;
}

bool demandeMonteeDepuisIntermediaire()
{
    return evtAppuiCourt && !dernierMouvementMontee;
}

bool demandeDescenteDepuisIntermediaire()
{
    return evtAppuiCourt && dernierMouvementMontee;
}

// =====================================================
// SETUP
// =====================================================
void setup()
{
    Serial.begin(115200);
    Serial.println("--- Maquette porte/store avec FSM ---");

    pinMode(BTN_MAIN, INPUT_PULLUP);
    pinMode(BTN_FDC_HAUT, INPUT_PULLUP);
    pinMode(BTN_FDC_BAS, INPUT_PULLUP);

    pinMode(LED_MONTEE, OUTPUT);
    pinMode(LED_DESCENTE, OUTPUT);
    pinMode(LED_LOCK, OUTPUT);

    moteurStop();
    digitalWrite(LED_LOCK, LOW);

    fsm.setDebugTransition(true);

    fsm.ADD_STATE(INIT)
       .ADD_STATE(ARRET_BAS)
       .ADD_STATE(ARRET_HAUT)
       .ADD_STATE(ARRET_INTERMEDIAIRE)
       .ADD_STATE(MONTEE)
       .ADD_STATE(DESCENTE)
       .ADD_STATE(VERROUILLE);

    // =================================================
    // TRANSITIONS
    // =================================================

    fsm.addTransition(INIT, ARRET_BAS, []()
    {
        return true;
    });

    // Verrouillage uniquement quand la porte est en bas
    fsm.addTransition(ARRET_BAS, VERROUILLE, []()
    {
        return evtAppuiLong;
    });

    // En verrouillé : seul un appui long déverrouille
    fsm.addTransition(VERROUILLE, ARRET_BAS, []()
    {
        return evtAppuiLong;
    });

    // Commande normale
    fsm.addTransition(ARRET_BAS, MONTEE, []()
    {
        return evtAppuiCourt;
    });

    fsm.addTransition(ARRET_HAUT, DESCENTE, []()
    {
        return evtAppuiCourt;
    });

    fsm.addTransition(ARRET_INTERMEDIAIRE, MONTEE, []()
    {
        return demandeMonteeDepuisIntermediaire();
    });

    fsm.addTransition(ARRET_INTERMEDIAIRE, DESCENTE, []()
    {
        return demandeDescenteDepuisIntermediaire();
    });

    // Appui court pendant mouvement = arrêt intermédiaire
    fsm.addTransition(MONTEE, ARRET_INTERMEDIAIRE, []()
    {
        return evtAppuiCourt;
    });

    fsm.addTransition(DESCENTE, ARRET_INTERMEDIAIRE, []()
    {
        return evtAppuiCourt;
    });

    // Fin de course ou arrêt automatique après 10s
    fsm.addTransition(MONTEE, ARRET_HAUT, []()
    {
        return finCourseHaut() || mouvementMaxAtteint();
    });

    fsm.addTransition(DESCENTE, ARRET_BAS, []()
    {
        return finCourseBas() || mouvementMaxAtteint();
    });

    // =================================================
    // ACTIONS EN ENTREE D'ETAT
    // =================================================

    fsm.onEnter(INIT, []()
    {
        Serial.println("INIT");
        moteurStop();
    });

    fsm.onEnter(ARRET_BAS, []()
    {
        Serial.println("ARRET BAS / DEVERROUILLE");
        moteurStop();

        dernierMouvementMontee = false;

        evtAppuiCourt = false;
        evtAppuiLong = false;
    });

    fsm.onEnter(ARRET_HAUT, []()
    {
        Serial.println("ARRET HAUT");
        moteurStop();

        dernierMouvementMontee = true;

        evtAppuiCourt = false;
        evtAppuiLong = false;
    });

    fsm.onEnter(ARRET_INTERMEDIAIRE, []()
    {
        Serial.println("ARRET INTERMEDIAIRE");
        moteurStop();

        evtAppuiCourt = false;
        evtAppuiLong = false;
    });

    fsm.onEnter(MONTEE, []()
    {
        Serial.println("MONTEE : LED 22 allumee");
        moteurMontee();

        dernierMouvementMontee = true;

        evtAppuiCourt = false;
        evtAppuiLong = false;
    });

    fsm.onEnter(DESCENTE, []()
    {
        Serial.println("DESCENTE : LED 23 allumee");
        moteurDescente();

        dernierMouvementMontee = false;

        evtAppuiCourt = false;
        evtAppuiLong = false;
    });

    fsm.onEnter(VERROUILLE, []()
    {
        Serial.println("VERROUILLE : appuis courts bloques");
        moteurStop();

        evtAppuiCourt = false;
        evtAppuiLong = false;
    });

    fsm.start(INIT);
}

// =====================================================
// LOOP
// =====================================================
void loop()
{
    lireBoutonPrincipal();
    updateLedLock();
    fsm.update();
}