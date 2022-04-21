#!/bin/sh

# log all commands exececuted
set -x

# exit when a command fails
set -e
./webserv config_file.conf &
sleep 3

curl -v localhost:8081
# curl -v localhost:8081/cgi/input

killall webserv
exit 0
