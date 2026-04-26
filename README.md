# CMPSC311 Chat App

This is a program in C that uses a client, server, and display component to make a functional chat app with sockets.

## Server
Standard socket creation and binding.

Uses thread for Receiving client messages.
Upon receipt of first message from client, sets that as the Username on that socket, then broadcasts "join message" to all other clients.
Subsequent messages take the stored Username, concatenates the received message, and rebroadcasts the whole string to all other clients. (Client program echoes the message locally.)
When 0 bytes are received from the client, disconnects the client from that socket and broadcasts "leave message" to all other clients.

## Client


## Display

