#!/bin/bash
set -euo pipefail
SCRIPT_DIR="$(dirname "$(realpath "$0")")"
BIN="$SCRIPT_DIR/../sys-info"


GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

pass() {
    echo -e "${GREEN}[PASS] $1${NC}"
}

fail() {
    echo -e "${RED}[FAIL] $1${NC}"
}

if [ ! -f "$BIN" ]; then
    fail "File $BIN not found!"
    exit 1
fi

test_help_output() {
    output=$($BIN --help)
    echo "$output" | grep -q "Usage:" && pass "Help output shows usage" || fail "Help output missing usage"
}

test_table_output_nonempty() {
    output=$($BIN)

    echo "$output" | grep -q "PID" && pass "Default table output is non-empty" || fail "Table output is empty"
}


test_json_output_valid() {
    output=$($BIN --json)
    echo "$output" | jq empty 2>&1 && pass "Valid JSON output" || fail "Invalid JSON output"
}

test_output_file_write() {
    tmpfile="output_test.txt"
    $BIN > "$tmpfile"
    [[ -s "$tmpfile" ]] && pass "Output written to file" || fail "Output file is empty"
    rm "$tmpfile"
}

# Run tests
test_help_output
test_table_output_nonempty
test_json_output_valid
test_output_file_write
