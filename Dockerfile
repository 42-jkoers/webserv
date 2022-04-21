FROM debian:buster

RUN apt-get update && \
	apt-get install -y -qq\
	make\
	clang\
	libc++-dev

COPY . .
RUN make fclean
RUN make -j 4

ENTRYPOINT [ "sh", "testing/send_many_requests.sh"  ]
