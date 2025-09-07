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

status=$(curl -s -o $OUT -w "%{http_code}" http://localhost:8080/api/Door)


if [ "$status" -eq 200 ]; then
    echo "$TEST PASS http://localhost:8080/api PASS"
else
    echo -n "$TEST FAIL http://localhost:8080/api STATUS $status: "
    cat $OUT
    echo
    exit 1
fi

check_q '.result.density' '1'
check_q '.result.effect' '"GRADIENT"'
check_q '.result.mode' '"on"'

curl -s -o $OUT 'http://localhost:8080/api/Door/off'
check_q '.result.mode' '"off"'

curl -s -o $OUT 'http://localhost:8080/api/Door/out'
check_q '.result.mode' '"off"'

curl -s -o $OUT 'http://localhost:8080/api/Door/on'
check_q '.result.mode' '"on"'

curl -s -o $OUT 'http://localhost:8080/api/Deck/off'
check_q '.result.mode' '"off"'

curl -s -o $OUT 'http://localhost:8080/api/Deck/out'
check_q '.result.mode' '"out"'

curl -s -o $OUT 'http://localhost:8080/api/Deck/on'
check_q '.result.mode' '"on"'

curl -s -o $OUT 'http://localhost:8080/api/Door/on?density=3'
check_q '.result.density' '3'

curl -s -o $OUT 'http://localhost:8080/api/Door/off?density=5'
check_q '.result.density' '3'

curl -s -o $OUT 'http://localhost:8080/api/Door/set?density=5'
check_q '.result.density' '5'

curl -s -o $OUT 'http://localhost:8080/api/Door/set?density=-1'
check_q '.status' '400'

curl -s -o $OUT 'http://localhost:8080/api/Door/set?density=999'
check_q '.status' '400'


curl -s -o $OUT 'http://localhost:8080/api/Door/set?effect=THEATRE'
check_q '.result.effect' '"THEATRE"'

curl -s -o $OUT 'http://localhost:8080/api/Door/set?effect=ZZZ'
check_q '.status' '400'
