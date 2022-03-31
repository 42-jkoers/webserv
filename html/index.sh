#!/bin/bash
echo -ne "Content-type: text/plain\r\n\r\n"
echo -ne "hello from bash"
echo -ne "$SERVER_PROTOCOL"
echo -ne "aaa"
