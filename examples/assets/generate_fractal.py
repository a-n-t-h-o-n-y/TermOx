"""Generate the bundled 640 x 400, 24-bit BMP using only the Python standard library.

Original procedural artwork for TermOx, distributed under the repository's MIT license.
Run from any directory: python3 examples/assets/generate_fractal.py
"""
import colorsys
import math
from pathlib import Path
import struct

width, height = 640, 400
pixels = bytearray()
for y in range(height - 1, -1, -1):
    for x in range(width):
        # A Julia set with spiralling branches; smooth escape-time coloring.
        z = complex((x / (width - 1) - .5) * 3.6,
                    (y / (height - 1) - .5) * 2.25)
        n = 0
        while abs(z) < 256 and n < 240:
            z = z * z + complex(-.74543, .11301)
            n += 1
        if n == 240:
            r, g, b = 5, 10, 24
        else:
            smooth = n + 1 - math.log2(math.log2(max(abs(z), 1.001)))
            hue = (.60 + smooth * .018) % 1
            value = .25 + .75 * (1 - math.exp(-max(0, smooth) / 12))
            r, g, b = (round(v * 255) for v in colorsys.hsv_to_rgb(hue, .78, value))
        pixels.extend((b, g, r))
header = struct.pack('<2sIHHI', b'BM', 54 + len(pixels), 0, 0, 54)
header += struct.pack('<IiiHHIIiiII', 40, width, height, 1, 24, 0, len(pixels), 2835, 2835, 0, 0)
Path(__file__).with_name('julia.bmp').write_bytes(header + pixels)
