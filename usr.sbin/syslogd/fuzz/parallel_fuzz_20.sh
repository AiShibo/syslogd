#!/bin/bash

# Parallel AFL fuzzing script with 20 fuzzers
# Usage: ./parallel_fuzz_20.sh

# Start primary fuzzer
afl-fuzz -M fuzzer00 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer00.log 2>fuzzer00.error &

# Start secondary fuzzers
afl-fuzz -S fuzzer01 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer01.log 2>fuzzer01.error &
afl-fuzz -S fuzzer02 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer02.log 2>fuzzer02.error &
afl-fuzz -S fuzzer03 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer03.log 2>fuzzer03.error &
afl-fuzz -S fuzzer04 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer04.log 2>fuzzer04.error &
afl-fuzz -S fuzzer05 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer05.log 2>fuzzer05.error &
afl-fuzz -S fuzzer06 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer06.log 2>fuzzer06.error &
afl-fuzz -S fuzzer07 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer07.log 2>fuzzer07.error &
afl-fuzz -S fuzzer08 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer08.log 2>fuzzer08.error &
afl-fuzz -S fuzzer09 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer09.log 2>fuzzer09.error &
afl-fuzz -S fuzzer10 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer10.log 2>fuzzer10.error &
afl-fuzz -S fuzzer11 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer11.log 2>fuzzer11.error &
afl-fuzz -S fuzzer12 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer12.log 2>fuzzer12.error &
afl-fuzz -S fuzzer13 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer13.log 2>fuzzer13.error &
afl-fuzz -S fuzzer14 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer14.log 2>fuzzer14.error &
afl-fuzz -S fuzzer15 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer15.log 2>fuzzer15.error &
afl-fuzz -S fuzzer16 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer16.log 2>fuzzer16.error &
afl-fuzz -S fuzzer17 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer17.log 2>fuzzer17.error &
afl-fuzz -S fuzzer18 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer18.log 2>fuzzer18.error &
afl-fuzz -S fuzzer19 -i seeds -o state_20 -- ../syslogd -P 1234 1>fuzzer19.log 2>fuzzer19.error &

echo "Started parallel fuzzing with 20 instances:"
echo "- Primary fuzzer (fuzzer00) running"
echo "- 19 secondary fuzzers (fuzzer01-fuzzer19) running"
echo ""
echo "Monitor with: afl-whatsup state_20/"
echo "Press Ctrl+C to stop"

# Monitor fuzzing progress
while true; do
    clear
    afl-whatsup state_20/
    sleep 1
done