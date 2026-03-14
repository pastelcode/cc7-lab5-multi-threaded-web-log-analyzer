#!/bin/bash
set -e

make

echo "--- Running with access_short.log ---"
./main access_short.log

echo ""
echo "--- Running with access.log ---"
./main access.log
