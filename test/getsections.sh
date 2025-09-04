#!/bin/bash

mkdir -p out/test
OUT=out/test/$(basename "$0" .sh).txt

for section in Door Game Theatre Deck Back
do
	status=$(curl -s -o $OUT -w "%{http_code}" http://localhost:8080/api/$section)
	if [ "$status" -eq 200 ]; then
	    echo "$(basename "$0" .sh) http://localhost:8080/api/$section PASS"
	else
	    echo -n "$(basename "$0" .sh) http://localhost:8080/api/$section FAIL $status: "
	    cat $OUT
	    echo
	    exit 1
	fi
done
