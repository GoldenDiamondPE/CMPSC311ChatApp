# include <stdio.h>
# include <stdlib.h>
# include <unistd.h> //brings the close() command
# include <sys/types.h> //helps initiate the following two packages
# include <sys/socket.h> //1 creates socket
# include <netinet/in.h> //2 contains structure, call def to know address and the port of remote socket
# include <pthread.h>
# include <errno.h> 
# include <string.h>
# include <arpa/inet.h>

/*
At this time, the client is only able to recieve messages

Set up for min requirments for server to be tested -> 
feel free to add more, such as the GUI functions
*/

//I think when the client types a message, they will see the message twice in the terminal

static void * threadFuncRecv(void *arg){
    int server_socket = *((int *)arg);
    free(arg); //frees space for pointer
    char buffer[268];// Your "bucket" - used to hold the data that is recieved from network
   
    while(1){
            ssize_t n = recv(server_socket, buffer, sizeof(buffer) -1, 0);

            //n = bytes recieved
            if (n == 0){
                printf("Server closed connection.\n");
                _exit(1);
            }else if (n > 0){
                buffer[n] = '\0'; //resets reponse 
                printf("%s\n",  buffer);

            }else{
                perror("recv failed");
                break;
            }
        }
}


static void * threadFuncSend(void *arg){
    int server_socket = *((int *)arg);
    free(arg); //frees space for pointer
    char client_message[256]; //client's message to send back

    while(1){
        // OLD: breaks messages with spaces: scanf("%255s",client_message);
		//New: allows spaces */
		fgets(client_message, sizeof(client_message), stdin);
		//strip out newline char
		client_message[strcspn(client_message, "\n")] = '\0';
		//close socket and quit if logoff message is received.
		if (strcmp(client_message, "LOGOFF") == 0){
			close(server_socket);
			_exit(0);
		}
		send(server_socket, client_message, strlen(client_message), 0);
       
    }
}


int main(){

    int serverFD  = socket(AF_INET, SOCK_STREAM, 0);
    //server chooses (IPV4 addressing, TCP, default protocol(0))
    //bind() bind to port - specifies the address for the socket

    struct sockaddr_in server_address; // sockaddr_in is designed for IPv4

    //defines address remotely
    server_address.sin_family = AF_INET; // tells kernel to use IPv4 protocol
    server_address.sin_port = htons(8080); //80 = HTTP - htons() converts address to appropriate data format for TCP
   // server_address.sin_addr.s_addr = INADDR_ANY; //This accesses the address itself .s_addr

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("SERVER: Invalid address\n");
        return (-1);
    }

    int connection_status;
    connection_status = connect(serverFD, (struct sockaddr *) &server_address, sizeof(server_address));
    // connect() is a testing method, it returns an integer, -1,0

    if (connection_status < 0){
        
        perror("connection was not successful\n");
        return(-1);
    }
    else{
        printf("arrived!\n");
    
        char preNode[268] = "0_input";
        int s; //pointer call
        void *res;
        int N = 1;

        int *server_arg = malloc(sizeof(int)); //allocates heap memory, meaning the seperate socket value copy for the new thread
        if (server_arg == NULL) {
            perror("Client: malloc failed");
            N = 0;
        }

        int *server_arg2 = malloc(sizeof(int)); //allocates heap memory, meaning the seperate socket value copy for the new thread
        if (server_arg2 == NULL) {
            perror("Client: malloc failed");
            N = 0;
        }

        if (N == 1){
            *server_arg = serverFD;
            *server_arg2 = serverFD;
            pthread_t t1;
            pthread_t t2;
            if (pthread_create(&t1, NULL, threadFuncRecv, server_arg) == 0){ //checks to see if thread creation works
                    printf("\nt1 created\n");
                    //pthread_detach(t1); // used for cleanup
                
            }else{
                perror("pthread_create failed");
                free(server_arg);
            }



            if (pthread_create(&t2, NULL, threadFuncSend, server_arg2) == 0){ //checks to see if thread creation works
                  printf("\nt2 created\n");
                  pthread_detach(t2); // used for cleanup
                
            }else{
                perror("pthread_create failed");
                 free(server_arg2);
            }

        	printf("Type 'LOGOFF' at any time to exit.\n\n");
            s =  pthread_join(t1, &res);
            //printf("\n%d\n",s);


    }else{
        // close the socket
        close(serverFD);
    }

    }
    
    return 0;
}
