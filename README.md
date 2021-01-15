# TCP-Server
A server written in C that facilitates file transfer over a TCP connection, logs user actions and keeps track of current users.

gcc testServer.c -o testServer
./testServer

After compiling agent.c,
./agent zeus.cs.txstae.edu 9678 '#JOIN'

to connect the server.
All requests must be put in '' and be preceded by a #. e.g.: '#JOIN'
Use above commands to compile and run.

Default port is set to 9678. You can change it in the top of the file at "#define PORT 9678".
Log file is saved to "log.txt" and active agents are saved to "active.txt". Please email me at bwb63@txstate.edu
if you have any trouble compiling.