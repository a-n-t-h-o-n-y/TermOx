"""Check Phase's scenes, then exercise tabs and controls through a POSIX PTY."""
import csv
import io
import json
from pathlib import Path
import subprocess
import sys

from smoke_examples import smoke


def main():
    executable = Path(sys.argv[1]) / 'examples' / 'TermOx.examples.phase'
    subprocess.run([str(executable), '--check'], check=True)
    for scene in range(4):
        frame = json.loads(subprocess.check_output(
            [str(executable), '--json', '--scene', str(scene), '--frames', '90',
             '--width', '80', '--height', '28'], text=True))
        assert frame['width'] == 80 and frame['height'] == 28
        content = frame['rows'][1:-3]
        colors = {cell['fg'] for row in content for cell in row if cell['text'].strip()}
        assert len(colors) > 12, f'scene {scene} has no rendered simulation'
    for scene in range(4):
        output = subprocess.check_output(
            [str(executable), '--benchmark', '--scene', str(scene), '--frames', '2',
             '--width', '800', '--height', '240'], text=True)
        row, = csv.DictReader(io.StringIO(output))
        assert int(row['frames']) == 2 and int(row['bytes_per_frame']) > 0
        assert all(float(row[key]) >= 0 for key in ('simulation_ms', 'paint_ms', 'encode_ms'))
    # SGR mouse clicks select each tab; controls keep working after pointer capture,
    # focus changes, wheel input, keyboard tab switching, and terminal resizes.
    output = smoke(executable, [
        b' ', b'.', b'r', b' ',
        b'\x1b[<2;60;15M', b'\x1b[<34;68;18M', b'\x1b[<2;68;18m',
        b'\x1b[<64;68;18M', b'\x1b[<65;68;18M', b'\x1b[H',
        b'\x1b[<0;12;12M', b'\x1b[<32;24;16M', b'\x1b[<0;24;16m',
        b'\x1b[<0;11;1M', b'\x1b[<0;11;1m',  # Bloom
        b'\x1b[<0;30;12M', b'\x1b[<32;40;15M', b'\x1b[<0;40;15m',
        b'\x1b[<64;40;15M',
        b'\x1b[<0;19;1M', b'\x1b[<0;19;1m',  # Loom
        b'\x1b[<0;50;12M', b'\x1b[<32;55;14M', b'\x1b[<0;55;14m',
        b'\x1b[6;5~',  # Ctrl-PageDown: Flock
        b'\x1b[<35;40;12M', b'\x1b[<0;40;12M', b'\x1b[<0;40;12m',
        b'\x1b[<2;40;12M', b'\x1b[<2;40;12m',
        b'\t', b'\r', b'\t', b'\r',
        b'\t', b'\r', b'\t', b'\r', b'\x1b[B', b'\r',
        b'\t', b'\x1b[C',
    ])
    for text in (b'brush', b'ratio', b'birds', b'Current'):
        assert text in output, f'missing scene status: {text!r}'
    print('Phase: rendered scenes, mouse gestures, tab switching, and controls passed')


if __name__ == '__main__':
    main()
