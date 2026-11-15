# Generates src/helpers/ui/CyrillicFont.h: the 5x8 Cyrillic glyph table (Adafruit GFX classic font format:
# 5 column bytes, bit 0 = top row) indexed by Windows-1251 code 0x80..0xFF.
# usage: python gen_cyrillic_font.py <Adafruit GFX Library/glcdfont.c> src/helpers/ui/CyrillicFont.h
# (glcdfont.c is in .pio/libdeps/<env>/; look-alike letters are copied from it)
import re, sys

glcd_path, out_path = sys.argv[1], sys.argv[2]
src = open(glcd_path).read()
body = src[src.index('{') + 1:src.rindex('}')]
GL = [int(x, 16) for x in re.findall(r'0x[0-9A-Fa-f]{2}', body)]

def latin(c):
    return GL[ord(c) * 5:ord(c) * 5 + 5]

def from_art(rows, top=0):
    rows = [r for r in rows.split()]
    assert all(len(r) == 5 for r in rows), rows
    assert top + len(rows) <= 8, rows
    cols = [0] * 5
    for y, r in enumerate(rows):
        for x, ch in enumerate(r):
            if ch == '#':
                cols[x] |= 1 << (top + y)
    return cols

U = {}   # uppercase: rows from 0
L = {}   # lowercase: rows from top 2 unless given as (top, art)

same = {'А': 'A', 'В': 'B', 'Е': 'E', 'І': 'I', 'К': 'K', 'М': 'M', 'Н': 'H', 'О': 'O', 'Р': 'P',
        'С': 'C', 'Т': 'T', 'Х': 'X',
        'а': 'a', 'е': 'e', 'і': 'i', 'о': 'o', 'р': 'p', 'с': 'c', 'у': 'y', 'х': 'x'}

U['Б'] = "##### #.... #.... ####. #...# #...# ####."
U['Г'] = "##### #.... #.... #.... #.... #.... #...."
U['Ґ'] = "....# ##### #.... #.... #.... #.... #...."
U['Д'] = "..##. .#.#. .#.#. .#.#. .#.#. ##### #...#"
U['Є'] = ".###. #...# #.... ####. #.... #...# .###."
U['Ж'] = "#.#.# #.#.# .###. ..#.. .###. #.#.# #.#.#"
U['З'] = ".###. #...# ....# ..##. ....# #...# .###."
U['И'] = "#...# #...# #..## #.#.# ##..# #...# #...#"
U['Ї'] = ".#.#. ..... .###. ..#.. ..#.. ..#.. .###."
U['Й'] = ".#.#. ..#.. #...# #..## #.#.# ##..# #...#"
U['Л'] = "..### .#..# .#..# .#..# .#..# .#..# #...#"
U['П'] = "##### #...# #...# #...# #...# #...# #...#"
U['У'] = "#...# #...# #...# .#### ....# #...# .###."
U['Ф'] = "..#.. .###. #.#.# #.#.# #.#.# .###. ..#.."
U['Ц'] = "#..#. #..#. #..#. #..#. #..#. ##### ....#"
U['Ч'] = "#...# #...# #...# .#### ....# ....# ....#"
U['Ш'] = "#.#.# #.#.# #.#.# #.#.# #.#.# #.#.# #####"
U['Щ'] = "#.#.# #.#.# #.#.# #.#.# #.#.# ##### ....#"
U['Ь'] = "#.... #.... #.... ####. #...# #...# ####."
U['Ю'] = "#..#. #.#.# #.#.# ###.# #.#.# #.#.# #..#."
U['Я'] = ".#### #...# #...# .#### ..#.# .#..# #...#"
U['Ё'] = ".#.#. ..... ##### #.... ####. #.... #####"
U['Ъ'] = "##... .#... .#... .###. .#..# .#..# .###."
U['Ы'] = "#...# #...# #...# ##..# #.#.# #.#.# ##..#"
U['Э'] = ".###. #...# ....# .#### ....# #...# .###."

L['б'] = (0, ".###. #.... ####. #...# #...# #...# .###.")
L['в'] = "####. #...# ####. #...# ####."
L['г'] = "##### #.... #.... #.... #...."
L['ґ'] = (1, "....# ##### #.... #.... #.... #....")
L['д'] = "..##. .#.#. .#.#. ##### #...#"
L['є'] = ".###. #...# ###.. #...# .###."
L['ж'] = "#.#.# #.#.# .###. #.#.# #.#.#"
L['з'] = ".###. ....# ..##. ....# .###."
L['и'] = "#...# #..## #.#.# ##..# #...#"
L['ї'] = (0, ".#.#. ..... .##.. ..#.. ..#.. ..#.. .###.")
L['й'] = (0, ".#.#. ..#.. #...# #..## #.#.# ##..# #...#")
L['к'] = "#..#. #.#.. ##... #.#.. #..#."
L['л'] = "..### .#..# .#..# .#..# #...#"
L['м'] = "#...# ##.## #.#.# #...# #...#"
L['н'] = "#...# #...# ##### #...# #...#"
L['п'] = "##### #...# #...# #...# #...#"
L['т'] = "##### ..#.. ..#.. ..#.. ..#.."
L['ф'] = (1, "..#.. .###. #.#.# #.#.# #.#.# .###. ..#..")
L['ц'] = "#..#. #..#. #..#. #..#. ##### ....#"
L['ч'] = "#...# #...# .#### ....# ....#"
L['ш'] = "#.#.# #.#.# #.#.# #.#.# #####"
L['щ'] = "#.#.# #.#.# #.#.# #.#.# ##### ....#"
L['ь'] = "#.... #.... ####. #...# ####."
L['ю'] = "#..#. #.#.# ###.# #.#.# #..#."
L['я'] = ".#### #...# .#### .#..# #...#"
L['ё'] = (0, ".#.#. ..... .###. #...# ##### #.... .###.")
L['ъ'] = "##... .#... .###. .#..# .###."
L['ы'] = "#...# #...# ##..# #.#.# ##..#"
L['э'] = ".###. #...# ..### #...# .###."

glyphs = {}
for k, v in same.items():
    glyphs[k] = latin(v)
for k, v in U.items():
    glyphs[k] = from_art(v, 0)
for k, v in L.items():
    top, art = v if isinstance(v, tuple) else (2, v)
    glyphs[k] = from_art(art, top)

table = [[0] * 5 for _ in range(128)]
for ch, g in glyphs.items():
    b = ch.encode('cp1251')[0]
    assert b >= 0x80, ch
    table[b - 0x80] = g

missing = [bytes([b]).decode('cp1251') for b in range(0xC0, 0x100) if not any(table[b - 0x80])]
assert not missing, missing

with open(out_path, 'w', encoding='utf-8', newline='\n') as f:
    f.write('#pragma once\n\n')
    f.write('// Generated: Cyrillic glyphs for the classic 5x7 Adafruit GFX font (5 column bytes, bit 0 = top row),\n')
    f.write('// indexed by Windows-1251 code - 0x80. All zero = no glyph.\n\n')
    f.write('#include <stdint.h>\n\n')
    f.write('static const uint8_t CYR_FONT_5X8[128][5] = {\n')
    for i, g in enumerate(table):
        if any(g):
            name = bytes([i + 0x80]).decode('cp1251')
            f.write('  {%s},  // 0x%02X %s\n' % (', '.join('0x%02X' % x for x in g), i + 0x80, name))
        else:
            f.write('  {0},\n')
    f.write('};\n')

# preview
def show(chars):
    rows = []
    for c in chars:
        g = glyphs[c]
        rows.append([''.join('#' if g[x] >> y & 1 else '.' for x in range(5)) for y in range(8)])
    print('  '.join(f'{c:5}' for c in chars))
    for y in range(8):
        print('  '.join(r[y] for r in rows))
    print()

up = 'АБВГҐДЕЄЖЗИІЇЙКЛМНОПРСТУФХЦЧШЩЬЮЯЁЪЫЭ'
lo = up.lower()
for s in (up[:19], up[19:], lo[:19], lo[19:]):
    show(s)
