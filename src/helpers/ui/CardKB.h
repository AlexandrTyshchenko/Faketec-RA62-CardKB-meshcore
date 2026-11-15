#pragma once

// M5Stack CardKB: I2C keyboard, sends one byte per key press (0 = no key).
// Arrows, Enter and Esc use the same codes as UIScreen.h (KEY_LEFT .. KEY_RIGHT, KEY_ENTER, KEY_CANCEL).

#include <Arduino.h>
#include <Wire.h>

#ifndef CARDKB_I2C_ADDR
  #define CARDKB_I2C_ADDR  0x5F
#endif

#define CARDKB_POLL_MILLIS    20
#define CARDKB_PROBE_MILLIS   5000   // how often to look for a keyboard that is not connected

class CardKB {
  TwoWire* _wire;
  bool _present = false;
  unsigned long _next_poll = 0, _next_probe = 0;

public:
  CardKB(TwoWire& wire = Wire) : _wire(&wire) { }

  bool isPresent() const { return _present; }

  // returns the code of the next key, or 0 if none (the keyboard can be plugged in at any time)
  uint8_t poll() {
    unsigned long now = millis();
    if (!_present) {
      if ((long)(now - _next_probe) < 0) return 0;
      _next_probe = now + CARDKB_PROBE_MILLIS;
      // probe with a 1-byte read, as in polling: a zero-length write (address only) is not
      // reliable on the nRF52 TWIM and missed the keyboard depending on timing
      if (_wire->requestFrom((uint8_t)CARDKB_I2C_ADDR, (uint8_t)1) != 1) return 0;
      _present = true;
      _next_poll = now + CARDKB_POLL_MILLIS;
      return (uint8_t)_wire->read();   // a key pressed meanwhile, or 0
    }
    if ((long)(now - _next_poll) < 0) return 0;
    _next_poll = now + CARDKB_POLL_MILLIS;

    if (_wire->requestFrom((uint8_t)CARDKB_I2C_ADDR, (uint8_t)1) != 1) {   // unplugged
      _present = false;
      _next_probe = now + CARDKB_PROBE_MILLIS;
      return 0;
    }
    return (uint8_t)_wire->read();
  }
};
