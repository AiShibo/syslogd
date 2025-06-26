#!/bin/bash

# Simple parallel AFL fuzzing script
# Usage: ./parallel_fuzz.sh

# Start primary fuzzer
afl-fuzz -M primary -i seeds -o state -- ../syslogd -P 1234 1>primary.log 2>primary.error &

# Start secondary fuzzers
afl-fuzz -S secondary01 -i seeds -o state -- ../syslogd -P 1234 1>secondary01.log 2>secondary01.error &
afl-fuzz -S secondary02 -i seeds -o state -- ../syslogd -P 1234 1>secondary02.log 2>secondary02.error &

echo "Started parallel fuzzing:"
echo "- Primary fuzzer running"
echo "- Secondary01 fuzzer running" 
echo "- Secondary02 fuzzer running"
echo ""
echo "Monitor with: afl-whatsup state/"
echo "Press Ctrl+C to stop"

# Monitor fuzzing progress
while true; do
    clear
    afl-whatsup state/
    sleep 1
done
