#!/bin/sh

# exit when a command fails
set -e
export TERM=xterm-256color

runner () {
	echo "$@"
	eval "$@"
	clear
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
