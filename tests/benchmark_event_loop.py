"""Measure the former polling pump and readiness pump on an idle POSIX PTY."""

import csv
import fcntl
import os
import pty
import statistics
import select
import time
import struct
import subprocess
import sys
import termios


def sample(executable, polling):
    master, slave = pty.openpty()
    fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack("HHHH", 24, 80, 0, 0))
    original = termios.tcgetattr(slave)
    env = dict(os.environ, TERM="xterm-256color", COLORTERM="truecolor")
    env.pop("NO_COLOR", None)
    process = None
    try:
        process = subprocess.Popen([str(executable)] + (["--polling"] if polling else []),
                                   stdin=slave, stdout=slave, stderr=subprocess.PIPE, env=env)
        deadline = time.monotonic() + 5
        while process.poll() is None and time.monotonic() < deadline:
            if select.select([master], [], [], 0.05)[0]:
                os.read(master, 65536)
        _, error = process.communicate(timeout=1)
        assert process.returncode == 0, error.decode(errors="replace")
        assert termios.tcgetattr(slave) == original, "terminal modes were not restored"
        iterations, latency = error.decode().strip().split(",")
        return int(iterations), float(latency)
    finally:
        if process is not None and process.poll() is None:
            process.kill()
            process.wait()
        os.close(master)
        os.close(slave)


def main():
    executable = sys.argv[1]
    samples = {mode: [] for mode in ("polling", "readiness")}
    # Alternate modes to reduce bias from machine scheduling or temperature.
    for _ in range(5):
        for mode in samples:
            samples[mode].append(sample(executable, mode == "polling"))
    writer = csv.writer(sys.stdout)
    writer.writerow(["mode", "sample", "iterations_per_idle_second", "post_to_dispatch_us"])
    for mode, values in samples.items():
        for index, (iterations, latency) in enumerate(values):
            writer.writerow([mode, index + 1, iterations, latency])
        print(f"{mode}: median {statistics.median(x[0] for x in values)} iterations, "
              f"{statistics.median(x[1] for x in values):.3f} us delivery", file=sys.stderr)


if __name__ == "__main__":
    main()
