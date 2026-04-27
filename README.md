# CMPSC311 Chat App
This is a program in C that uses a client, server, and display component to make a functional chat app with sockets.

Capable of handling 100 concurrent clients. Can be increased by changing definition in server file.


## Server
Standard socket creation and binding.

Uses thread for Receiving client messages.

Upon receipt of first message from client, sets that as the Username on that socket, then broadcasts "join message" to all other clients.

Subsequent messages take the stored Username, concatenates the received message, and rebroadcasts the whole string to all other clients. (Client program echoes the message locally.)

When 0 bytes are received from the client, disconnects the client from that socket and broadcasts "leave message" to all other clients.



## Client/GUI
Uses GTK library for GUI, which is uses the XML and CSS files to construct its appearance.

Standard connection methods and error checks for successful connection.


Creates thread for receiving messages from server.

When receiving 0 bytes, the server closed the connection, and client will exit.
Otherwise a valid message is received, and will be printed.

The message received window will auto-scroll down when enough messages are recieved to keep the most recently received message visible.


For sending messages, the 'Send' button will ignore empty strings in text box.

The first string typed in the textbox will be sent to the server as the Username.

Subsequent strings will be sent to the server and echoed locally, concatenated with previously provided Username.


## Build Commands

### Server (server.c)
gcc server.c -lpthread -o server

### Client (cligui.c + chatapp.ui + style.css)
DEPENDENCES
  Ubuntu/Debian:
    bashsudo apt install gcc pkg-config libgtk-4-dev
  Fedora/RHEL:
    bashsudo dnf install gcc pkg-config gtk4-devel
  Arch:
    bashsudo pacman -S gcc pkgconf gtk4

BUILD COMMAND 
gcc $(pkg-config --cflags gtk4) -o cligui cligui.c $(pkg-config --libs gtk4) -lpthread
