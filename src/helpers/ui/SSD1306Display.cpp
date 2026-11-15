#include "SSD1306Display.h"
#ifdef UI_CYRILLIC
  #include "Cyrillic.h"
  #include "CyrillicFont.h"
#endif

bool SSD1306Display::i2c_probe(TwoWire& wire, uint8_t addr) {
  wire.beginTransmission(addr);
  uint8_t error = wire.endTransmission();
  return (error == 0);
}

// Color scheme
ColorVal UIColor::window_bkg = SSD1306_BLACK;
ColorVal UIColor::title_bkg = SSD1306_BLACK;
ColorVal UIColor::title_txt = SSD1306_WHITE;
ColorVal UIColor::primary_txt = SSD1306_WHITE;
ColorVal UIColor::secondary_txt = SSD1306_WHITE;
ColorVal UIColor::warning_txt = SSD1306_WHITE;
ColorVal UIColor::popup_bkg = SSD1306_BLACK;
ColorVal UIColor::popup_txt = SSD1306_WHITE;
ColorVal UIColor::corp_blue = SSD1306_WHITE;

bool SSD1306Display::begin() {
  if (!_isOn) {
    if (_peripher_power) _peripher_power->claim();
    _isOn = true;
  }
  #ifdef DISPLAY_ROTATION
  display.setRotation(DISPLAY_ROTATION);
  #endif
  bool success = display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS, true, false);
  #if defined(PIN_OLED_CS) && defined(PIN_OLED_DC)
  return success;
  #else
  return success && i2c_probe(Wire, DISPLAY_ADDRESS);
  #endif
}

void SSD1306Display::turnOn() {
  if (!_isOn) {
    if (_peripher_power) _peripher_power->claim();
    _isOn = true;  // set before begin() to prevent double claim
    if (_peripher_power) begin();  // re-init display after power was cut
  }
  display.ssd1306_command(SSD1306_DISPLAYON);
}

void SSD1306Display::turnOff() {
  display.ssd1306_command(SSD1306_DISPLAYOFF);
  if (_isOn) {
    if (_peripher_power) {
#if PIN_OLED_RESET >= 0
      digitalWrite(PIN_OLED_RESET, LOW);
#endif
      _peripher_power->release();
    }
    _isOn = false;
  }
}

void SSD1306Display::clear() {
  display.clearDisplay();
  display.display();
}

void SSD1306Display::startFrame(ColorVal bkg) {
  display.clearDisplay();  // TODO: apply 'bkg'
  _color = SSD1306_WHITE;
  display.setTextColor(_color);
  display.setTextSize(1);
#ifdef UI_CYRILLIC
  _text_size = 1;
#endif
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
}

void SSD1306Display::setTextSize(int sz) {
  display.setTextSize(sz);
#ifdef UI_CYRILLIC
  _text_size = sz;
#endif
}

void SSD1306Display::setColor(ColorVal c) {
  _color = c;
  display.setTextColor(_color);
}

void SSD1306Display::setCursor(int x, int y) {
  display.setCursor(x, y);
}

#ifdef UI_CYRILLIC
// bytes 0x80..0xFF are Windows-1251: Cyrillic letters from CYR_FONT_5X8, anything else as a block
void SSD1306Display::print(const char* str) {
  for (const uint8_t* p = (const uint8_t*)str; *p; p++) {
    if (*p < 0x80) {
      display.write(*p);
      continue;
    }
    const uint8_t* g = CYR_FONT_5X8[*p - 0x80];
    if ((g[0] | g[1] | g[2] | g[3] | g[4]) == 0) {
      display.write(0xDB);   // CP437 full block
      continue;
    }
    display.write(' ');   // moves the cursor, wrapping like any other character
    const int sz = _text_size;
    const int x = display.getCursorX() - 6 * sz, y = display.getCursorY();
    for (int col = 0; col < 5; col++) {
      for (int row = 0; row < 8; row++) {
        if ((g[col] >> row) & 1) {
          if (sz == 1) display.drawPixel(x + col, y + row, _color);
          else display.fillRect(x + col * sz, y + row * sz, sz, sz, _color);
        }
      }
    }
  }
}

void SSD1306Display::translateUTF8ToBlocks(char* dest, const char* src, size_t dest_size) {
  Cyrillic::utf8ToDisplay(dest, src, dest_size);
}
#else
void SSD1306Display::print(const char* str) {
  display.print(str);
}
#endif

void SSD1306Display::fillRect(int x, int y, int w, int h) {
  display.fillRect(x, y, w, h, _color);
}

void SSD1306Display::drawRect(int x, int y, int w, int h) {
  display.drawRect(x, y, w, h, _color);
}

void SSD1306Display::drawXbm(int x, int y, const uint8_t* bits, int w, int h) {
  display.drawBitmap(x, y, bits, w, h, _color);
}

uint16_t SSD1306Display::getTextWidth(const char* str) {
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(str, 0, 0, &x1, &y1, &w, &h);
  return w;
}

void SSD1306Display::endFrame() {
  display.display();
}
