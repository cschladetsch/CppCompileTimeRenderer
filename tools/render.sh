#!/usr/bin/env bash
set -euo pipefail

raw=${1:-raw.txt}
img=${2:-image.txt}

clang++ -std=c++20 -fconstexpr-steps=10000000 -ferror-limit=0 raytracer.cpp 2>"$raw" || true
./tools/clean_errors.py <"$raw" >"$img"

echo "Wrote $raw and $img"
