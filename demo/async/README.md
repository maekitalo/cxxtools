Server
======

This demo implements a server, which can handle multiple simultaneous clients.
It handles all input and output asynchronously in a single thread.

This server just echos all input back to the client.

You can test it using the nc tool like this:

    nc localhost 7000 <server.cpp

This sends the file `server.cpp` to the server. Since our server just echos the
data back, it is printed by nc to stdout.
