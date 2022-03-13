#!/bin/bash
set -e

while [ true ]; do
	curl -v -X POST -F "upfile=@test.txt" -H transfer-encoding:chunked localhost:8081/upload &>/dev/null
	[ $? -ne 0 ] && exit 1
	echo "success"
done;
