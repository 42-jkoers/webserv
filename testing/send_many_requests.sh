#!/bin/sh

# exit when a command fails
set -e
export TERM=xterm-256color

test_upload () {
	curl -X POST -F "upfile=@$1" localhost:8081 &> /dev/null
	curl -H transfer-encoding:chunked -X POST -F "upfile=@$1" -v localhost:8081 &> /dev/null
}

runner () {
	clear
	echo "$@"
	eval "$@"
}

./webserv config_file.conf > /tmp/webserv.out &

# START TESTS
runner curl -v localhost:8081
runner curl -v localhost:8081/cgi/input
runner curl -v -X POST -F "upfile=@webserv" localhost:8081
runner curl -v -H transfer-encoding:chunked -X POST -F "upfile=@webserv" -v localhost:8081
# END TESTS

clear
pkill webserv
exit 0
