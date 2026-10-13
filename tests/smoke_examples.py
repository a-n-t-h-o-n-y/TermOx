"""Exercise example input and terminal restoration through a POSIX pseudo-terminal."""

import fcntl
import os
from pathlib import Path
import pty
import select
import struct
import subprocess
import sys
import termios
import time


def smoke(executable, inputs):
    master, slave = pty.openpty()
    fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack("HHHH", 35, 120, 0, 0))
    original = termios.tcgetattr(slave)
    env = dict(os.environ, TERM="xterm-256color", COLORTERM="truecolor")
    process = subprocess.Popen([str(executable)], stdin=slave, stdout=slave, stderr=slave, env=env)
    output = bytearray()

    def drain(duration):
        deadline = time.monotonic() + duration
        while time.monotonic() < deadline:
            ready, _, _ = select.select([master], [], [], min(0.05, max(0, deadline - time.monotonic())))
            if ready:
                output.extend(os.read(master, 65536))

    try:
        deadline = time.monotonic() + 5
        while b"\x1b[?1049h" not in output and process.poll() is None and time.monotonic() < deadline:
            drain(0.05)
        drain(0.1)
        assert process.poll() is None, output.decode(errors="replace")
        assert b"\x1b[?1049h" in output, f"alternate screen was not entered: {bytes(output)!r}"
        before = len(output)
        for data in inputs:
            os.write(master, data)
            drain(0.2)
        assert len(output) > before, "input did not trigger a repaint"
        # Resize in both directions while input and painting are active.
        fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack("HHHH", 12, 25, 0, 0))
        drain(0.2)
        fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack("HHHH", 35, 120, 0, 0))
        drain(0.2)
        os.write(master, b"\x03")
        deadline = time.monotonic() + 5
        while process.poll() is None and time.monotonic() < deadline:
            drain(0.05)
        assert process.poll() == 1, f"Ctrl-C did not exit cleanly: {process.poll()}"
        drain(0.1)
        assert termios.tcgetattr(slave) == original, "terminal modes were not restored"
        assert b"\x1b[?1049l" in output, "alternate screen was not restored"
        assert b"\x1b[?25h" in output, "cursor was not restored"
        assert b"\x1b[?2004l" in output, "bracketed paste was not disabled"
        print(f"{executable.name}: input, Ctrl-C, and terminal restoration passed")
    finally:
        if process.poll() is None:
            process.kill()
        process.wait()
        os.close(master)
        os.close(slave)


def main():
    examples = Path(sys.argv[1]) / "examples"
    smoke(examples / "TermOx.examples.pong", [b"\r", b"\x1b[B", b"\x1b[A"])
    smoke(examples / "TermOx.examples.pinbox", [b"\x1b[<0;10;5M", b"\x1b[<32;11;5M", b"\x1b[<0;11;5m"])
    smoke(examples / "TermOx.examples.snake", [b"s", b"j", b"l"])
    smoke(examples / "TermOx.examples.accumulator", [b"\x1b[<0;10;5M", b"\x1b[<0;10;5m", b"\t"])
    smoke(examples / "TermOx.examples.scratch", [b"\x1b[<0;80;4M", "é日e\u0301👩\u200d💻".encode(), b"\x1b[200~paste\ntext\x1b[201~"])


if __name__ == "__main__":
    main()
