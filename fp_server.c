# include <stdio.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <unistd.h>
# include <netinet/in.h>
#include <arpa/inet.h>
# include <pthread.h>
# include <string.h>




#define MAX_CLIENTS 100 //defines the limit of clients
#define USER_NAME 32 //max length of username of 32

char users[MAX_CLIENTS][USER_NAME]; //array to store usernames

int g_client_sockets[MAX_CLIENTS];
int g_client_count = 0;

pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER; 


static void * threadrec(void *arg){
    int client_socket = *((int *)arg);
    free(arg); //frees space for pointer
    char buffer[234];
	char whole_message[268];
   
    printf("SERVER: Client connected on socket %d\n", client_socket);

	//first message received from client is entry of user name
    ssize_t bytes_received = recv(client_socket, users[client_socket], sizeof(users[client_socket]) - 1, 0);

	users[client_socket][bytes_received]='\0';
	
	//diagnostic printing of username and socket. Can be deleted before final version
	printf("%s connect on socket %d.\n",users[client_socket],client_socket);
		
	//Added by Matt - send join message to other clients
	char join_message[268];
		
	pthread_mutex_lock(&client_mutex);
		
	/*Build the join message*/
	snprintf(join_message, sizeof(join_message), "*** %s has join the chat ***", users[client_socket]);
		
	/*Send to other clients*/
	for(int i = 0; i < g_client_count; i++){
		if(g_client_sockets[i] != client_socket){
            send(g_client_sockets[i], join_message, strlen(join_message), 0);
		}
	}
	pthread_mutex_unlock(&client_mutex);
	
		
    if (bytes_received > 0 ){
		/* Broadcast message to all connected clients */
		while((bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0 ){
			pthread_mutex_lock(&client_mutex);

            snprintf(whole_message, sizeof(whole_message), "%s: %s", users[client_socket], buffer);
            for (int i = 0; i < g_client_count; i++) {
                /* Don't send message back to sender*/
                if ((g_client_sockets[i] != client_socket) && (buffer[0] != '\0')){
                    send(g_client_sockets[i], whole_message, strlen(whole_message), 0);
                }
             }
				
             //clear buffers
			memset(&buffer, 0, sizeof(buffer));
			memset(&whole_message, 0, sizeof(whole_message));
            pthread_mutex_unlock(&client_mutex);
        }
          
	}
	if (bytes_received == 0) {
        printf("SERVER: Client on socket %d disconnected\n", client_socket);
                
        //Added by Matt - send disconnect message to other clients
        char leave_message[268];
        pthread_mutex_lock(&client_mutex);
                
        snprintf(leave_message, sizeof(leave_message), "*** %s has left the chat ***", users[client_socket]);
                
        for(int i = 0; i < g_client_count; i++){
			if(g_client_sockets[i] != client_socket){
				send(g_client_sockets[i], leave_message, strlen(leave_message), 0);
			}
		}
				
		pthread_mutex_unlock(&client_mutex);
    }
    else{ //if (bytes_received < 0){
        perror("SERVER: recv failed");
	}
                
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

    //binding section
    int binds = bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
   
    if (binds< 0 ){
        perror("SERVER: Bind failed");
        return -1;
    }
	else {
        printf("SERVER: Bind successful\n");
   	}

    if (listen(server_socket, 5) == -1){
       printf("SERVER: Listen failed\n");
        return -1;
    }
	else {
        printf("SERVER: Listen successful\n"); 
    }
    
    //listens at this socket, any # besides 1 for small traffic levels,
    int client_socket;

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

        pthread_t t;
       
        if (pthread_create(&t, NULL, threadrec, client_arg) != 0){ //checks to see if thread creation works
                perror("pthread_create failed");
                free(client_arg); //frees the space because creation failed
                close(client_socket);
                continue;
        }
       

        pthread_detach(t); // used for cleanup
    }
    

    close(server_socket);
    return 0;
}
//bulbasaur
