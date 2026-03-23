# include <stdio.h>

# include <stdlib.h>
# include <unistd.h> //brings the close() command
# include <sys/types.h> //helps initiate the following two packages
# include <sys/socket.h> //1 creates socket
# include <netinet/in.h> //2 contains structure, call def to know address and the port of remote socket

# include <errno.h> 
# include <string.h>

/*
At this time, the client is only able to recieve messages

Set up for min requirments for server to be tested -> 
feel free to add more, such as the GUI functions
*/


void functions(const char *messg1){ //testing if the server message can initiate functions
  
    //ideally the GUI function, or update function
}

int update(int response){
// how to check if only a signal is received?
    if ( response > -1){
             // print out data we got back
        printf("server data: %d\n", response); //prints the server_response - the char message.
    }
    return (-1);
}

int main(){

    int serverFD  = socket(AF_INET, SOCK_STREAM, 0);
    // server chooses (IPV4 addressing, TCP, default protocol(0))


    //bind() bind to port - specifies the address for the socket

    struct sockaddr_in server_address; // sockaddr_in is designed for IPv4

    //defines address remotely
    server_address.sin_family = AF_INET; // tells kernel to use IPv4 protocol
    server_address.sin_port = htons(8080); //80 = HTTP - htons() converts address to appropriate data format for TCP
    server_address.sin_addr.s_addr = INADDR_ANY; //This accesses the address itself .s_addr



    int connection_status;
    connection_status = connect(serverFD, (struct sockaddr *) &server_address, sizeof(server_address));
    // connect() is a testing method, it returns an integer, -1,0

    if (connection_status < 0){
        perror("connection was not successful\n");
        return(-1);
    }
    else{
        printf("arrived!\n");
    

        
        //recieves data from the server - this is usually after the listen portion
        char server_response[268];// Your "bucket" - used to hold the data that is recieved from network
       // char client_message[256]; //client's message to send back
        char preNode[268] = "0_input";
        int flag = 0;

        while(1){
        //only updates when client recieves data
        // after each update, flag should be checked if it is the same as last output's
        // the flag is indicated by server_response[0], ex: "1: Hello!""
            ssize_t n = recv(serverFD, &server_response, sizeof(server_response) -1, 0);

            //n = bytes recieved
            if (n > 0){
                server_response[n] = '\0'; //resets reponse 
                printf("Server %d: %s\n", flag + 1, server_response);
                flag++;

            }else if (n == 0){
                printf("Server closed connection.\n");
                break;
            }else{
                perror("recv failed");
                break;
            }
            /*
            old code

            // after the read, preNode should or should'nt equal message. 
            if (server_response[0] != preNode[0] ){
                printf("server data: %s\n", server_response);
                // maybe call GUI update here
                flag ++;
                preNode[0] = server_response[0];

            }
                */


        }

        // close the socket
        close(serverFD);
    
    }
    //functions(server_response);
    
    return 0;
}

