#!/bin/sh

# exit when a command fails
set -e
export TERM=xterm-256color

runner () {
	echo
	echo "$@"
	eval "$@"
	clear
}

start=$(date +%s)

make fclean

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
	make -j $(grep -c ^processor /proc/cpuinfo)
else
	make -j $(sysctl -n hw.ncpu)
fi

# kill proceess listening on port
P=$(lsof -ti:8081) && kill -9 $P

./webserv testing/config_file.conf > /tmp/webserv.out &

# START TESTS
runner curl -v localhost:8081
runner curl -v 'localhost:8081/aaa?ad=3&a=c'
runner curl -v localhost:8081/cgi/input --request POST --data 'aa'
runner curl -v localhost:8081/cgi/input --request POST --data $(head -c   50 < /dev/zero | tr '\0' 'a')
runner curl -v localhost:8081/cgi/input --request POST --data $(head -c 1000 < /dev/zero | tr '\0' 'a')

runner curl -v -X POST -F "upfile=@webserv" localhost:8081
runner curl -v -H transfer-encoding:chunked -X POST -F "upfile=@webserv" -v localhost:8081
# END TESTS

pkill webserv

end=$(date +%s)
seconds=$(echo "$end - $start" | bc)
echo
echo
echo "All tests successful in $(awk -v t=$seconds 'BEGIN{t=int(t*1000); printf "%02d hours, %02d minutes, %02d seconds\n", t/3600000, t/60000%60, t/1000%60}')"

exit 0
