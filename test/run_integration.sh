#!/usr/bin/env bash
set -euo pipefail

# Run blossom headless for a short duration and verify it starts
NO_PLOT=1 timeout 5 ./blossom | tee /tmp/blossom.integ.out || true
if grep -q "Species-Blossom version" /tmp/blossom.integ.out; then
  echo "Integration: version output present"
  exit 0
else
  echo "Integration test failed: missing version"
  cat /tmp/blossom.integ.out
  exit 2
fi
