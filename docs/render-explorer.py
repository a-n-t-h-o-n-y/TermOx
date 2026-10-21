"""Render Explorer with a temporary sample directory as SVG."""

import argparse
from html import escape
import json
from pathlib import Path
import subprocess
import shutil
import tempfile
import os


def render(frame, title):
    cell_width, cell_height, margin = 9, 18, 18
    width = frame['width'] * cell_width + 2 * margin
    height = frame['height'] * cell_height + 2 * margin
    parts = [f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" '
             f'viewBox="0 0 {width} {height}" role="img" aria-labelledby="title desc">',
             f'<title id="title">{escape(title)}</title>',
             '<desc id="desc">Rendered from the current Explorer frame with motion disabled.</desc>',
             f'<rect width="{width}" height="{height}" fill="#141917"/>']
    text = []
    for y, row in enumerate(frame['rows']):
        # Join adjacent equal backgrounds to keep the SVG small, including pixel images.
        start = 0
        while start < len(row):
            color = row[start]['bg'] or '20,25,23'
            end = start + 1
            while end < len(row) and (row[end]['bg'] or '20,25,23') == color:
                end += 1
            if color != '20,25,23':
                parts.append(f'<rect x="{margin + start * cell_width}" y="{margin + y * cell_height}" '
                             f'width="{(end - start) * cell_width}" height="{cell_height}" fill="rgb({color})"/>')
            start = end
        for x, cell in enumerate(row):
            if cell['columns'] and cell['text'].strip():
                text.append(f'<text x="{margin + x * cell_width}" y="{margin + y * cell_height + 14}" '
                            f'fill="rgb({cell["fg"] or "232,230,219"})" '
                            f'textLength="{cell["columns"] * cell_width}" lengthAdjust="spacingAndGlyphs">'
                            f'{escape(cell["text"])}</text>')
    parts.append('<g font-family="Menlo,Consolas,monospace" font-size="14" xml:space="preserve">')
    parts.extend(text)
    parts.extend(['</g>', '</svg>'])
    return '\n'.join(parts) + '\n'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output', type=Path)
    parser.add_argument('--executable', default='build/examples/TermOx.examples.explorer')
    parser.add_argument('--width', type=int, default=110)
    parser.add_argument('--height', type=int, default=30)
    parser.add_argument('--title', default='TermOx Explorer')
    args = parser.parse_args()
    # A small, disposable dataset keeps the README independent of the developer's files.
    with tempfile.TemporaryDirectory(prefix='termox-explorer-', dir='/tmp') as temporary:
        folder = Path(temporary)
        (folder / 'documents').mkdir()
        (folder / 'images').mkdir()
        (folder / 'notes.txt').write_text('Explorer\n\nBrowse folders and preview files.\n')
        (folder / 'project.json').write_text('{"name": "TermOx", "language": "C++20"}\n')
        (folder / 'readme.md').write_text('# Project notes\n\nKeyboard and mouse navigation.\n')
        shutil.copy(Path(__file__).resolve().parent.parent / 'examples/assets/julia.bmp',
                    folder / 'julia.bmp')
        for item in folder.iterdir():
            os.utime(item, (1704067200, 1704067200))
        frame = json.loads(subprocess.check_output([
            args.executable, str(folder), '--json', '--select', 'julia.bmp', '--theme', 'dark',
            '--width', str(args.width), '--height', str(args.height)], text=True))
    args.output.write_text(render(frame, args.title), encoding='utf-8')


if __name__ == '__main__':
    main()
