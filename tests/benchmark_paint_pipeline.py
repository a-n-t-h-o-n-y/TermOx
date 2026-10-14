"""Compare two Release benchmark binaries without running builds concurrently."""

import argparse
import csv
import io
from pathlib import Path
import statistics
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("before", type=Path)
    parser.add_argument("after", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--modes", nargs="+", choices=("renderer", "paint", "application"),
                        default=("renderer", "paint", "application"))
    args = parser.parse_args()
    samples = []
    grouped = {}
    for mode in args.modes:
        for repetition in range(3):
            # Alternate order to reduce systematic warmup and scheduling bias.
            order = ("before", "after") if repetition % 2 == 0 else ("after", "before")
            for version in order:
                binary = getattr(args, version).resolve()
                result = subprocess.run(
                    [str(binary), "--" + mode], check=True, capture_output=True, text=True
                )
                rows = list(csv.DictReader(io.StringIO(result.stdout)))
                grouped.setdefault((mode, version), []).append(rows)
                samples.extend(
                    {"version": version, "repetition": repetition + 1, **row}
                    for row in rows
                )
                print(f"{mode}: {version} sample {repetition + 1} complete", flush=True)

    # Every process starts from the same state and must perform identical work.
    for mode in args.modes:
        reference = grouped[mode, "before"][0]
        expected = [(row["workload"], row["iterations"], row["checksum"]) for row in reference]
        for version in ("before", "after"):
            for rows in grouped[mode, version]:
                actual = [(row["workload"], row["iterations"], row["checksum"]) for row in rows]
                if actual != expected:
                    raise RuntimeError(f"Workload/checksum mismatch: {mode} {version}")

    args.output.mkdir(parents=True, exist_ok=True)
    with (args.output / "paint-pipeline-samples.csv").open("w", newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=list(samples[0]), lineterminator="\n")
        writer.writeheader()
        writer.writerows(samples)
    for (mode, version), runs in grouped.items():
        rows = [dict(row) for row in runs[0]]
        for index, row in enumerate(rows):
            for column in ("median_us_per_operation", "allocations_per_operation"):
                row[column] = f"{statistics.median(float(run[index][column]) for run in runs):.3f}"
        path = args.output / f"paint-pipeline-{mode}-{version}.csv"
        with path.open("w", newline="") as stream:
            writer = csv.DictWriter(stream, fieldnames=list(rows[0]), lineterminator="\n")
            writer.writeheader()
            writer.writerows(rows)


if __name__ == "__main__":
    main()
