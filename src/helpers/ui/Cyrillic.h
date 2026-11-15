#pragma once

// Cyrillic on 8-bit displays: text is kept in Windows-1251 (one byte per letter),
// the display driver draws codes 0x80..0xFF with the glyphs from CyrillicFont.h.
// Also the Ukrainian keyboard layout (ЙЦУКЕН) for ASCII keyboards such as the CardKB.

#include <stdint.h>
#include <stddef.h>

#define CYR_NO_GLYPH  0x98   // unused in Windows-1251: shown as a block

namespace Cyrillic {

// Unicode -> Windows-1251; 0 if not representable
inline uint8_t fromUnicode(uint32_t u) {
  if (u >= 0x0410 && u <= 0x044F) return (uint8_t)(u - 0x0410 + 0xC0);   // А..я
  switch (u) {
    case 0x0401: return 0xA8;   // Ё
    case 0x0451: return 0xB8;   // ё
    case 0x0404: return 0xAA;   // Є
    case 0x0454: return 0xBA;   // є
    case 0x0406: return 0xB2;   // І
    case 0x0456: return 0xB3;   // і
    case 0x0407: return 0xAF;   // Ї
    case 0x0457: return 0xBF;   // ї
    case 0x0490: return 0xA5;   // Ґ
    case 0x0491: return 0xB4;   // ґ
  }
  return 0;
}

// Windows-1251 -> Unicode; 0 if not a letter from fromUnicode()
inline uint32_t toUnicode(uint8_t b) {
  if (b >= 0xC0) return 0x0410 + (b - 0xC0);
  switch (b) {
    case 0xA8: return 0x0401;
    case 0xB8: return 0x0451;
    case 0xAA: return 0x0404;
    case 0xBA: return 0x0454;
    case 0xB2: return 0x0406;
    case 0xB3: return 0x0456;
    case 0xAF: return 0x0407;
    case 0xBF: return 0x0457;
    case 0xA5: return 0x0490;
    case 0xB4: return 0x0491;
  }
  return 0;
}

// ASCII look-alikes for common typographic characters
inline char asciiFallback(uint32_t u) {
  switch (u) {
    case 0x02BC: case 0x2018: case 0x2019: case 0x201B: case 0x2032: return '\'';   // apostrophes
    case 0x00AB: case 0x00BB: case 0x201C: case 0x201D: case 0x201E: return '"';
    case 0x2010: case 0x2011: case 0x2012: case 0x2013: case 0x2014: case 0x2212: return '-';
    case 0x00A0: case 0x2009: case 0x202F: return ' ';
    case 0x2026: return '.';
  }
  return 0;
}

// decodes one UTF-8 character at s[*i], advances *i past it
inline uint32_t nextUtf8(const char* s, size_t* i) {
  uint8_t c = (uint8_t)s[*i];
  (*i)++;
  if (c < 0x80) return c;
  int extra = c >= 0xF0 ? 3 : c >= 0xE0 ? 2 : c >= 0xC0 ? 1 : 0;
  uint32_t u = c & (0x3F >> extra);
  for (int k = 0; k < extra; k++) {
    uint8_t n = (uint8_t)s[*i];
    if ((n & 0xC0) != 0x80) return 0xFFFD;   // truncated sequence
    u = (u << 6) | (n & 0x3F);
    (*i)++;
  }
  return extra == 0 ? 0xFFFD : u;   // a stray continuation byte
}

// UTF-8 -> Windows-1251 for display; other characters become CYR_NO_GLYPH, control characters are dropped
inline void utf8ToDisplay(char* dest, const char* src, size_t dest_size) {
  size_t i = 0, j = 0;
  while (src[i] != 0 && j < dest_size - 1) {
    uint32_t u = nextUtf8(src, &i);
    char a;
    if (u >= 32 && u <= 126) {
      dest[j++] = (char)u;
    } else if (u >= 0x80) {
      uint8_t b = fromUnicode(u);
      if (b != 0) dest[j++] = (char)b;
      else if ((a = asciiFallback(u)) != 0) dest[j++] = a;
      else dest[j++] = (char)CYR_NO_GLYPH;
    }
  }
  dest[j] = 0;
}

// number of UTF-8 bytes of a Windows-1251 string (all letters from fromUnicode() take 2)
inline size_t utf8Length(const char* s, size_t len) {
  size_t n = len;
  for (size_t i = 0; i < len; i++) {
    if ((uint8_t)s[i] >= 0x80) n++;
  }
  return n;
}

// Windows-1251 -> UTF-8; dest must hold utf8Length(src, len) + 1 bytes
inline void toUtf8(char* dest, const char* src, size_t len) {
  size_t j = 0;
  for (size_t i = 0; i < len; i++) {
    uint8_t b = (uint8_t)src[i];
    uint32_t u = b < 0x80 ? b : toUnicode(b);
    if (u < 0x80) {
      dest[j++] = (char)u;
    } else if (u != 0) {
      dest[j++] = (char)(0xC0 | (u >> 6));
      dest[j++] = (char)(0x80 | (u & 0x3F));
    }
  }
  dest[j] = 0;
}

// Ukrainian layout (ЙЦУКЕН, as on Windows): an ASCII key -> Windows-1251 code (or the key itself).
// Letters on [ ] ; ' , . ` \ and Shift; '/' gives '.', '?' gives ','.
inline uint8_t ukrainianKey(uint8_t key) {
  static const char LAT[] = "qwertyuiop[]asdfghjkl;'zxcvbnm,./`\\"
                            "QWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>?|";
  static const char UKR[] = "йцукенгшщзхїфівапролджєячсмитьбю.'ґ"
                            "ЙЦУКЕНГШЩЗХЇФІВАПРОЛДЖЄЯЧСМИТЬБЮ,Ґ";
  size_t u = 0;
  for (size_t i = 0; LAT[i] != 0; i++) {
    uint32_t cp = nextUtf8(UKR, &u);
    if ((uint8_t)LAT[i] == key) {
      return cp < 0x80 ? (uint8_t)cp : fromUnicode(cp);
    }
  }
  return key;
}

}
