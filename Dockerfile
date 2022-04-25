FROM debian:stretch-20220418-slim

RUN apt-get update && \
	apt-get install -y -qq\
	build-essential\
	procps\
	curl=7.52.1-5+deb9u16\
	make\
	clang\
	libc++-dev\
	bc

COPY . .

ENTRYPOINT [ "sh", "./testing/send_many_requests.sh"  ]
