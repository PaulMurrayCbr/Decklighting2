#!/bin/bash

mkdir -p out/test
OUT=out/test/$(basename "$0" .sh).txt

status=$(curl -s -o $OUT -w "%{http_code}" http://localhost:8080/api)
if [ "$status" -eq 200 ]; then
    echo "$(basename "$0" .sh) http://localhost:8080/api PASS"
else
    echo -n "$(basename "$0" .sh) http://localhost:8080/api FAIL $status: "
    cat $OUT
    echo
    exit 1
fi
