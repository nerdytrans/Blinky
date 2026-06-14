#include <Arduino.h>
#include <U8g2lib.h>

// SW_I2C on PA2 (SCL) / PA1 (SDA) — identical to the original working config.
// HW Wire is NOT used on the controller; a software I2C master below handles
// target enrollment and animation commands on the same bus.
U8G2_SSD1306_128X64_NONAME_1_SW_I2C display(U8G2_R0, PIN_PA2, PIN_PA1, U8X8_PIN_NONE);

// ── Pins ──────────────────────────────────────────────────────────────────────
// TOKEN_OUT: driven HIGH to grant the first target node permission to enrol.
//            Wire this pin to PA3 of the first target node.
#define TOKEN_OUT       PIN_PA3

// ── I2C address-assignment protocol ──────────────────────────────────────────
#define ENROLL_ADDR         0x08
#define CMD_ASSIGN          0x01   // [CMD_ASSIGN, newAddr]
#define CMD_SET_ANIM        0x02   // [CMD_SET_ANIM, type, r, g, b, speed]

#define MAX_TARGETS         16
#define FIRST_TARGET_ADDR   0x10   // 0x10–0x1F; well clear of 0x3C (SSD1306)
#define ENROLL_TIMEOUT_MS   2000UL // How long to wait for a new target to appear

uint8_t targetAddrs[MAX_TARGETS];
uint8_t numTargets = 0;

// ── Software I2C master ───────────────────────────────────────────────────────
// Uses the same PA1/PA2 pins as U8g2 SW_I2C (open-drain, same convention).
// Requires external pull-up resistors on SDA/SCL (already present for display).
// Never call while a display firstPage/nextPage loop is active.
#define SWI2C_SDA      PIN_PA1
#define SWI2C_SCL      PIN_PA2
#define SWI2C_DLY      5   // µs — ~100 kHz

static inline void sda_hi() { pinMode(SWI2C_SDA, INPUT); }
static inline void sda_lo() { pinMode(SWI2C_SDA, OUTPUT); digitalWrite(SWI2C_SDA, LOW); }
static inline void scl_hi() { pinMode(SWI2C_SCL, INPUT); }
static inline void scl_lo() { pinMode(SWI2C_SCL, OUTPUT); digitalWrite(SWI2C_SCL, LOW); }

static void swi2c_start() {
    sda_hi(); scl_hi(); delayMicroseconds(SWI2C_DLY);
    sda_lo();           delayMicroseconds(SWI2C_DLY);
    scl_lo();           delayMicroseconds(SWI2C_DLY);
}

static void swi2c_stop() {
    sda_lo(); scl_lo(); delayMicroseconds(SWI2C_DLY);
    scl_hi();           delayMicroseconds(SWI2C_DLY);
    sda_hi();           delayMicroseconds(SWI2C_DLY);
}

// Sends one byte; returns true if slave ACKed.
static bool swi2c_write_byte(uint8_t b) {
    for (int8_t i = 7; i >= 0; i--) {
        if (b & (1 << i)) sda_hi(); else sda_lo();
        delayMicroseconds(SWI2C_DLY);
        scl_hi(); delayMicroseconds(SWI2C_DLY);
        scl_lo(); delayMicroseconds(SWI2C_DLY);
    }
    sda_hi();                            // release SDA — slave drives ACK
    delayMicroseconds(SWI2C_DLY);
    scl_hi(); delayMicroseconds(SWI2C_DLY);
    bool ack = (digitalRead(SWI2C_SDA) == LOW);
    scl_lo(); delayMicroseconds(SWI2C_DLY);
    return ack;
}

// Returns true if a device ACKs at addr (7-bit).
static bool swi2c_probe(uint8_t addr) {
    swi2c_start();
    bool ack = swi2c_write_byte(addr << 1);
    swi2c_stop();
    return ack;
}

// Sends len bytes to addr; returns true if address ACKed.
static bool swi2c_send(uint8_t addr, const uint8_t* data, uint8_t len) {
    swi2c_start();
    if (!swi2c_write_byte(addr << 1)) { swi2c_stop(); return false; }
    for (uint8_t i = 0; i < len; i++) swi2c_write_byte(data[i]);
    swi2c_stop();
    return true;
}

// ── Target enumeration ────────────────────────────────────────────────────────
// Drives TOKEN_OUT HIGH, then repeatedly polls ENROLL_ADDR.  Each time a
// target appears it is assigned the next sequential address.  The target then
// drives its TOKEN_OUT HIGH, passing the token to the next node.  Enumeration
// ends when no device responds within ENROLL_TIMEOUT_MS.
void enumerateTargets() {
    pinMode(TOKEN_OUT, OUTPUT);
    digitalWrite(TOKEN_OUT, LOW);   // Hold low while targets boot
    delay(500);                     // Allow all nodes time to initialise
    digitalWrite(TOKEN_OUT, HIGH);  // Release token to first node

    uint8_t nextAddr = FIRST_TARGET_ADDR;

    while (numTargets < MAX_TARGETS) {
        uint32_t start = millis();
        bool found = false;
        while ((millis() - start) < ENROLL_TIMEOUT_MS) {
            if (swi2c_probe(ENROLL_ADDR)) { found = true; break; }
            delay(50);
        }
        if (!found) break;

        uint8_t msg[] = { CMD_ASSIGN, nextAddr };
        swi2c_send(ENROLL_ADDR, msg, 2);
        delay(20);  // Give target time to switch addresses and pass token

        targetAddrs[numTargets++] = nextAddr++;
    }
}

// ── Animation commands ────────────────────────────────────────────────────────
void sendAnimation(uint8_t addr, uint8_t animType,
                   uint8_t r, uint8_t g, uint8_t b, uint8_t speed) {
    uint8_t msg[] = { CMD_SET_ANIM, animType, r, g, b, speed };
    swi2c_send(addr, msg, 6);
}

// ── Setup ─────────────────────────────────────────────────────────────────────
void setup() {
    delay(500);
    display.begin();
    display.setFont(u8g2_font_7x13B_tf);

    display.firstPage();
    do { display.drawStr(10, 20, "Enrolling..."); } while (display.nextPage());

    enumerateTargets();

    // Show the result briefly
    char buf[20];
    snprintf(buf, sizeof(buf), "Found: %u", (unsigned)numTargets);
    display.firstPage();
    do { display.drawStr(10, 20, buf); } while (display.nextPage());
    delay(2000);
}

// ── Loop ──────────────────────────────────────────────────────────────────────
void loop() {
    char buf[20];
    snprintf(buf, sizeof(buf), "Targets: %u", (unsigned)numTargets);
    display.firstPage();
    do { display.drawStr(10, 20, buf); } while (display.nextPage());

    // Send a different animation to each target node.
    for (uint8_t i = 0; i < numTargets; i++) {
        sendAnimation(targetAddrs[i], i % 5, 255, 0, 128, 50);
    }

    delay(5000);
}
