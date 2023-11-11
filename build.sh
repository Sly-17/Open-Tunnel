#!/bin/sh

touch out.txt logs.txt
gcc -o tunnel .src/tunneller.c
gcc -o tunnel-client .src/tunnel-client.c

echo "Open-Tunnel provides a pseudo remote login experience wherein one can access a peer host without any kind of key-based login (as in SSH)\n"
echo "USAGE : If you want to tunnel into a host's machine, get the host to run `./tunnel-client <PORT>`   or   `./tunnel-client <PORT>` &  to let it run in the background"
echo "        In order to gain access to their session, run `./tunnel <PORT>`\n"
echo "Keep in mind that this is just a pseudo remote login and hence there is no way to access text editors on the host machine. Also complex commands may fail in some occasions"
