#!/bin/sh

# exit when a command fails
set -e
export TERM=xterm-256color

runner () {
	echo "$@"
	eval "$@"
	clear
}

start=$(date +%s)

make fclean
make -j $(grep -c ^processor /proc/cpuinfo)

./webserv testing/config_file.conf > /tmp/webserv.out &

# START TESTS
runner curl -v localhost:8081
runner curl -v localhost:8081/cgi/input
runner curl -v -X POST -F "upfile=@webserv" localhost:8081
runner curl -v -H transfer-encoding:chunked -X POST -F "upfile=@webserv" -v localhost:8081
# END TESTS

clear
pkill webserv

end=$(date +%s)
seconds=$(echo "$end - $start" | bc)
echo
echo
echo "All tests successful in $(awk -v t=$seconds 'BEGIN{t=int(t*1000); printf "%02d hours, %02d minutes, %02d seconds\n", t/3600000, t/60000%60, t/1000%60}')"

exit 0
