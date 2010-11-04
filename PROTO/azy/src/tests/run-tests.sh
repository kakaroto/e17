#!/bin/sh

export CHARSET=UTF-8

./server  &>server.log &
SERVER_PID=$!
sleep 1

./client &>client.log

kill -9 $SERVER_PID
