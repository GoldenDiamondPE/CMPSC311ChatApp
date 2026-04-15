# include <stdio.h>
//# include "display.h"
# include <stdlib.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <unistd.h>
# include <netinet/in.h>
#include <arpa/inet.h>
# include <pthread.h>
# include <string.h>
#define _GNU_SOURCE  

//At this time the server is only able to send messages

#define MAX_CLIENTS 100 //defines the limit of clients

int g_client_sockets[MAX_CLIENTS];
int g_client_count = 0;

pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER; //Added by Matt

void listOClients(const char *client){

// client data would be stored in this list

}

//the void is the int? 

static void * threadrec(void *arg){
    int client_socket = *((int *)arg);
    free(arg); //frees space for pointer
    char buffer[256];
   
    printf("SERVER: Client connected on socket %d\n", client_socket);

    while (1) {
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        
            if (bytes_received == 0) {
                printf("SERVER: Client on socket %d disconnected\n", client_socket);
                break;
            
            }else if (bytes_received > 0){
                buffer[bytes_received] = '\0'; //resets reponse 
                printf("CLIENT[%d]: %s\n", client_socket, buffer);
                /* OLD: sends message back only to same client
				send(client_socket, server_message, strlen(server_message), 0);
				*/

				/* NEW: Broadcast message to all connected clients */

				pthread_mutex_lock(&client_mutex);

			for (int i = 0; i < g_client_count; i++) {

    			int sock = g_client_sockets[i];

    			/* Don't send message back to sender */
    			if (sock != client_socket) {

        		send(sock, buffer, strlen(buffer), 0);

    			}
			}

pthread_mutex_unlock(&client_mutex);

            }else if (bytes_received < 0){
                 perror("SERVER: recv failed");
                break;
            }else{
                /*
                perror("recv failed");
                break;
                */
               continue;
            }
    }

    /*close(client_socket);
    return NULL;*/
    
    //Matt: Remove disconnected client from list
    pthread_mutex_lock(&client_mutex);

	for (int i = 0; i < g_client_count; i++) {

    	if (g_client_sockets[i] == client_socket) {

        	for (int j = i; j < g_client_count - 1; j++) {

            	g_client_sockets[j] = g_client_sockets[j + 1];

        	}

        	g_client_count--;
        	break;
    	}
	}

	pthread_mutex_unlock(&client_mutex);
    //^^Matt^^
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
        perror("SERVER: Bind failed");
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


    while(1){
        client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0) { //checks to see if client connects
            perror("SERVER: Accept failed");
            continue;
        }

        int *client_arg = malloc(sizeof(int)); //allocates heap memory, meaning the seperate socket value copy for the new thread
        if (client_arg == NULL) {
            perror("SERVER: malloc failed");
            close(client_socket);
            continue;
        }
       

        *client_arg = client_socket;

        
        //Matt: Adds client socket to global list
        pthread_mutex_lock(&client_mutex);

        if (g_client_count < MAX_CLIENTS) {
            g_client_sockets[g_client_count] = client_socket;
            g_client_count++;
        }

		pthread_mutex_unlock(&client_mutex);
        //^^Added by Matt^^

        pthread_t t;
       
        if (pthread_create(&t, NULL, threadrec, client_arg) != 0){ //checks to see if thread creation works
                perror("pthread_create failed");
                free(client_arg); //frees the space because creation failed
                close(client_socket);
                continue;
        }
       

        pthread_detach(t); // used for cleanup
        g_client_count++;
    }
    

    close(server_socket);
    return 0;
}
//bulbasaur
