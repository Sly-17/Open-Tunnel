# Open-Tunnel

Open-Tunnel provides a pseudo remote login experience wherein one can access a peer host without\
any kind of key-based login (as in SSH).

To get started run ```sh buid.sh```

**USAGE** :\
If you want to tunnel into a host's machine, get the host to run ```./tunnel-client <PORT>```   or\
```./tunnel-client <PORT> &```  to let it run in the background\
 In order to gain access to their session, run ```./tunnel <PORT>```

Keep in mind that this is just a pseudo remote login and hence there is no way to access
text editors on the host machine. Also complex commands may fail in some occasions

