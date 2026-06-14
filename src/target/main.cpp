#include <Arduino.h>
#include <Wire.h>
#include <tinyNeoPixel.h>
#include "ColorWipe.h"
#include "TheaterChase.h"
#include "Rainbow.h"
#include "RainbowCycle.h"
#include "TheaterChaseRainbow.h"

// ── Pins ──────────────────────────────────────────────────────────────────────
#define NEOPIXEL_PIN    PIN_PB0
// TOKEN_IN:  driven HIGH by the left neighbour (or controller) to grant this
//            node permission to request an I2C address.
#define TOKEN_IN        PIN_PA3
// TOKEN_OUT: driven HIGH by this node once it is enrolled, granting the right
//            neighbour its turn.
#define TOKEN_OUT       PIN_PA4

// ── I2C address-assignment protocol ──────────────────────────────────────────
// All un-enrolled targets listen at this address simultaneously.  Bus collisions
// are avoided because only the token-holder accepts and acts on the assignment.
#define ENROLL_ADDR     0x08

// Command bytes
#define CMD_ASSIGN      0x01   // controller → 0x08 : [CMD_ASSIGN, newAddr]
#define CMD_SET_ANIM    0x02   // controller → myAddr: [CMD_SET_ANIM, type, r, g, b, speed]

// Animation type IDs
#define ANIM_COLOR_WIPE             0
#define ANIM_RAINBOW                1
#define ANIM_RAINBOW_CYCLE          2
#define ANIM_THEATER_CHASE          3
#define ANIM_THEATER_CHASE_RAINBOW  4

// ── NeoPixel strip ────────────────────────────────────────────────────────────
tinyNeoPixel strip(20, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// ── Shared state (ISR ↔ main) ─────────────────────────────────────────────────
volatile bool    enrolled   = false;
volatile uint8_t myAddr     = 0;

volatile bool    newCommand = false;
volatile uint8_t cmdAnimType = ANIM_RAINBOW;
volatile uint8_t cmdR = 0, cmdG = 0, cmdB = 255;
volatile uint8_t cmdSpeed   = 50;

// ── I2C callbacks ─────────────────────────────────────────────────────────────

// Called while listening at ENROLL_ADDR.  All un-enrolled nodes receive the
// same bytes; only the token-holder acts on them.
void onReceiveEnroll(int numBytes) {
    if (enrolled || numBytes < 2 || digitalRead(TOKEN_IN) == LOW) {
        while (Wire.available()) Wire.read();
        return;
    }
    uint8_t cmd  = Wire.read();
    uint8_t addr = Wire.read();
    while (Wire.available()) Wire.read();

    if (cmd == CMD_ASSIGN) {
        myAddr   = addr;
        enrolled = true;
    }
}

// Called after enrollment while listening at myAddr.
void onReceiveCommand(int numBytes) {
    if (numBytes < 1) return;
    uint8_t cmd = Wire.read();

    if (cmd == CMD_SET_ANIM && numBytes >= 6) {
        cmdAnimType = Wire.read();
        cmdR        = Wire.read();
        cmdG        = Wire.read();
        cmdB        = Wire.read();
        cmdSpeed    = Wire.read();
        newCommand  = true;
    } else {
        while (Wire.available()) Wire.read();
    }
}

// ── Setup ─────────────────────────────────────────────────────────────────────
void setup() {
    pinMode(TOKEN_IN,  INPUT);
    pinMode(TOKEN_OUT, OUTPUT);
    digitalWrite(TOKEN_OUT, LOW);

    strip.begin();
    strip.show();

    // Listen at the shared enrollment address until the controller assigns us
    // a unique address.
    Wire.begin(ENROLL_ADDR);
    Wire.onReceive(onReceiveEnroll);

    while (!enrolled) { /* spin — ISR will set enrolled */ }

    // Re-initialise Wire with our assigned address (safe here; we are not in
    // an ISR context).
    Wire.begin(myAddr);
    Wire.onReceive(onReceiveCommand);

    // Pass the token to the next node in the chain.
    digitalWrite(TOKEN_OUT, HIGH);
}

// ── Loop ──────────────────────────────────────────────────────────────────────
void loop() {
    // Snapshot volatile parameters so they stay consistent within one cycle.
    uint8_t  anim  = cmdAnimType;
    uint32_t color = strip.Color(cmdR, cmdG, cmdB);
    uint8_t  spd   = cmdSpeed;
    newCommand = false;

    switch (anim) {
        case ANIM_COLOR_WIPE:
            ColorWipe(strip, color, spd).run();
            break;
        case ANIM_RAINBOW_CYCLE:
            RainbowCycle(strip, spd).run();
            break;
        case ANIM_THEATER_CHASE:
            TheaterChase(strip, color, spd).run();
            break;
        case ANIM_THEATER_CHASE_RAINBOW:
            TheaterChaseRainbow(strip, spd).run();
            break;
        case ANIM_RAINBOW:
        default:
            Rainbow(strip, spd).run();
            break;
    }
}
