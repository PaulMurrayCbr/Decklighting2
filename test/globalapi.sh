#!/bin/bash

mkdir -p out/test
TEST=$(basename "$0" .sh)
OUT=out/test/$TEST.txt

check_equals() {
    local actual="$1"
    local expected="$2"
    local message="$3"

    if [[ "$actual" != "$expected" ]]; then
        echo "$TEST FAIL: $message (got '$actual', expected '$expected')" >&2
        exit 1
    else
        echo "$TEST PASS: $message = '$actual' "
    fi
}

check_q() {
    local q="$1"
    local expected="$2"
	check_equals $(jq "$q" "$OUT") "$expected" "$q"
}

status=$(curl -s -o $OUT -w "%{http_code}" http://localhost:8080/api)


if [ "$status" -eq 200 ]; then
    echo "$TEST PASS http://localhost:8080/api PASS"
else
    echo -n "$TEST FAIL http://localhost:8080/api STATUS $status: "
    cat $OUT
    echo
    exit 1
fi

check_q '.result.brightness' '0'
check_q '.result.on' 'false'
curl -s -o $OUT 'http://localhost:8080/api/on?brightness=3'
check_q '.result.brightness' '3'
check_q '.result.on' 'true'
curl -s -o $OUT 'http://localhost:8080/api/off'
check_q '.result.brightness' '3'
check_q '.result.on' 'false'

