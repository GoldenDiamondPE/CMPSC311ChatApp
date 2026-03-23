# include <stdio.h>
//# include "display.h"
# include <stdlib.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <unistd.h>
# include <netinet/in.h>
#include <arpa/inet.h>


//At this time the server is only able to send messages

#define MAX_CLIENTS 100 //defines the limit of clients
int g_client_sockets[MAX_CLIENTS];
int g_client_count = 0;

void listOClients(const char *client){

// client data would be stored in this list

}

int main(){

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    //defines server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);

    // Uses 127.0.0.1 as an IP Addr
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        printf("SERVER: Invalid address\n");
        return -1;
    }
    //server_address.sin_addr.s_addr = INADDR_ANY;//resolves to any IP address on local machine

    //binding section
    int binds = bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
   
    if (binds< 0 ){
        printf("SERVER: Bind failed");
        return -1;
    } else {
        printf("SERVER: Bind successful\n");
   }
        

    //listen()
    //listen (server_socket, 5);

    if (listen(server_socket, 5) == -1){
       printf("SERVER: Listen failed\n");
        return -1;
    } else {
        printf("SERVER: Listen successful\n"); 
    };
    

    //listens at this socket, any # besides 1 for small traffic levels, 
    int client_socket;

    //accept()

    //define a client list of addresses
    
   /* if (client_socket < 0){ //testing
        perror("SERVER: Accept failed");
        return -1;
    } else {
        printf("SERVER: Accept successful! Client connected.\n");
        //add the clients info to the list
        //addClient(client_socket);
       // printClientList();
    }
       
*/
/*
    char buffer[268]; // Your "bucket"
    ssize_t bytes_received = recv(new_socket, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received > 0) {
        buffer[bytes_received] = '\0'; // Add null terminator so printf knows where to stop
        printf("Client says: %s\n", buffer);
    } else if (bytes_received == 0) {
        printf("Client disconnected.\n");
    } else {
        perror("recv failed");
    }
    
    */
    client_socket = accept(server_socket, NULL,NULL);
    //send the message
    int numbFlag = 1;
    char server_message[256]; // message
    char finalM[268]; // numbFlag + message


    while(1){
        scanf("%255s",server_message );//max char to be taken in
        //snprintf(finalM, sizeof(finalM), "%d: %s", numbFlag, server_message);

        send(client_socket, server_message, sizeof(server_message), 0);
       
       
    }
    

    close(server_socket);
    close(client_socket);
    return 0;
}
//bulbasaur