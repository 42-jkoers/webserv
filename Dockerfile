FROM debian:stretch-20220418-slim

RUN apt-get update && \
	apt-get install -y -qq\
	build-essential\
	procps\
	curl=7.52.1-5+deb9u16\
	make\
	clang\
	libc++-dev

COPY . .
RUN make fclean
RUN make -j $(grep -c ^processor /proc/cpuinfo)

ENTRYPOINT [ "sh", "./testing/send_many_requests.sh"  ]