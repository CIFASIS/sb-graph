#!/usr/bin/env python3
"""Runs sbg-partitioner over a fixed set of models/sizes/partition counts,
checks that each run succeeds, and writes a customSmallerIsBetter JSON file
for github-action-benchmark.

Usage:
    run_benchmark.py --binary build/algorithms/partitioner/sbg-partitioner \
                      --output bench.json [--repetitions 3] [--timeout 900]
"""

import argparse
import json
import re
import statistics
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[3]
AIR_CONDITIONERS_DATA_DIR = REPO_ROOT / "test" / "partitioner" / "external_tools" / "data"

SIZES_PARTS = [
    (1000, 4),
    (1000, 8),
    (1000, 16),
    (10000, 4),
    (100000, 4),
]

TIME_BUILD_RE = re.compile(r"time_to_build_graph\s*=\s*([\d.]+)\s*ms")
TIME_PARTITION_RE = re.compile(r"time_to_partitionate\s*=\s*([\d.]+)\s*ms")


def advection2d_filename(size: int) -> str:
    # Not read from disk: get_sbg() only parses the size out of the name.
    return f"advection2D_{size}.json"


def air_conditioners_filename(size: int) -> str:
    # Read from disk: build_sb_graph() parses the real json file.
    path = AIR_CONDITIONERS_DATA_DIR / f"air_conditioners_{size}.json"
    if not path.exists():
        raise FileNotFoundError(f"missing fixture: {path}")
    return str(path)


def air_conditioners_cont_filename(size: int) -> str:
    # Not read from disk: get_sbg() only parses the size out of the name.
    return f"air_conditioners_cont_{size}.json"


MODELS = [
    ("advection2D", advection2d_filename),
    ("air_conditioners", air_conditioners_filename),
    ("air_conditioners_cont", air_conditioners_cont_filename),
]

# (model, size, parts) combinations known not to converge in a reasonable time.
# air_conditioners_cont/1000/16: the KL refinement does not converge within 5+
# minutes for this tiny model with this many partitions. Tracked separately;
# not a benchmark issue.
EXCLUDED_CASES = {
    ("air_conditioners_cont", 1000, 16),
}


def build_cases():
    cases = []
    for model_name, filename_fn in MODELS:
        for size, parts in SIZES_PARTS:
            if (model_name, size, parts) in EXCLUDED_CASES:
                continue
            cases.append(
                {
                    "model": model_name,
                    "size": size,
                    "parts": parts,
                    "filename_fn": filename_fn,
                }
            )
    return cases


def run_once(binary: str, filename: str, parts: int, timeout: int):
    cmd = [binary, "-f", filename, "-p", str(parts)]
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
    except subprocess.TimeoutExpired:
        return None, f"timed out after {timeout}s"

    if result.returncode != 0:
        return None, f"exit code {result.returncode}\nstderr:\n{result.stderr[-4000:]}"

    build_match = TIME_BUILD_RE.search(result.stdout)
    partition_match = TIME_PARTITION_RE.search(result.stdout)
    if not build_match or not partition_match:
        return None, "missing timing output in stdout"

    build_ms = float(build_match.group(1))
    partition_ms = float(partition_match.group(1))
    return (build_ms, partition_ms), None


def run_case(binary: str, case: dict, repetitions: int, timeout: int):
    name = f"{case['model']} size={case['size']} parts={case['parts']}"
    filename = case["filename_fn"](case["size"])

    runs = []
    for i in range(repetitions):
        timings, error = run_once(binary, filename, case["parts"], timeout)
        if timings is None:
            return {
                "name": name,
                "ok": False,
                "error": f"run {i + 1}/{repetitions} failed: {error}",
            }
        runs.append(timings)

    totals = [build_ms + partition_ms for build_ms, partition_ms in runs]
    median_total = statistics.median(totals)
    median_build = statistics.median(b for b, _ in runs)
    median_partition = statistics.median(p for _, p in runs)

    return {
        "name": name,
        "ok": True,
        "value": median_total,
        "build_ms": median_build,
        "partition_ms": median_partition,
        "runs": totals,
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--binary", required=True, help="path to the sbg-partitioner executable")
    parser.add_argument("--output", required=True, help="path to write the benchmark JSON to")
    parser.add_argument("--repetitions", type=int, default=3, help="runs per case (median is reported)")
    parser.add_argument("--timeout", type=int, default=900, help="timeout in seconds for a single run")
    args = parser.parse_args()

    binary = str(Path(args.binary).resolve())
    if not Path(binary).exists():
        print(f"error: binary not found: {binary}", file=sys.stderr)
        return 1

    cases = build_cases()
    results = []
    had_failure = False

    for case in cases:
        try:
            result = run_case(binary, case, args.repetitions, args.timeout)
        except FileNotFoundError as e:
            result = {"name": f"{case['model']} size={case['size']} parts={case['parts']}", "ok": False, "error": str(e)}

        results.append(result)
        if result["ok"]:
            print(f"[OK]   {result['name']}: {result['value']:.1f} ms (median of {args.repetitions})")
        else:
            had_failure = True
            print(f"[FAIL] {result['name']}: {result['error']}")

    benchmark_entries = [
        {
            "name": r["name"],
            "unit": "ms",
            "value": r["value"],
            "extra": f"build={r['build_ms']:.1f}ms partition={r['partition_ms']:.1f}ms runs={r['runs']}",
        }
        for r in results
        if r["ok"]
    ]

    output_path = Path(args.output)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(json.dumps(benchmark_entries, indent=2))

    print()
    print(f"Wrote {len(benchmark_entries)}/{len(cases)} successful results to {output_path}")

    if had_failure:
        print("error: one or more benchmark cases failed", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
