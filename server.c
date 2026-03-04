#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(){
    /*
    SOCKET
    */
    // Socket File Desc
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); //IPV4 TCP Protocol

    // Sees if sockfd ran properly
    if(sockfd ==-1){
        printf("SERVER: Error With Socket Creation\n");
        return -1;
    } else {
        printf("SERVER: Socket Creation Success\n");
    }

    /*
    BIND
    */
    struct sockaddr_in server_addr; // Creates server_addr struct
    server_addr.sin_family = AF_INET; // Uses IPV4
    server_addr.sin_port = htons(8080); // Sets SERVER Port

    // Uses 127.0.0.1 as an IP Addr
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        printf("SERVER: Invalid address\n");
        return -1;
    }

    // Calls Bind to see if it worked
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("SERVER: Bind failed");
        return -1;
    } else {
        printf("SERVER: Bind successful\n");
    }

    /*
    LISTEN
    */    
    if (listen(sockfd, 10)== -1){
        printf("SERVER: Listen failed\n");
        return -1;
    } else {
        printf("SERVER: Listen successful\n");
    }

    /*
    ACCEPT
    */
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int new_socket = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);

    if (new_socket < 0) {
        perror("SERVER: Accept failed");
        return -1;
    } else {
        printf("SERVER: Accept successful! Client connected.\n");
    }

    /*
    RECV()
    */
    char buffer[1024]; // Your "bucket"
    ssize_t bytes_received = recv(new_socket, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received > 0) {
    buffer[bytes_received] = '\0'; // Add null terminator so printf knows where to stop
    printf("Client says: %s\n", buffer);
    } else if (bytes_received == 0) {
        printf("Client disconnected.\n");
    } else {
        perror("recv failed");
    }
    
    while (1) {
        ssize_t bytes_received = recv(new_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) break; // Exit loop if error or disconnect

        buffer[bytes_received] = '\0';
        printf("Client: %s", buffer);

        // Echo the message back to the client
        send(new_socket, buffer, bytes_received, 0);
    }
    

    close(new_socket);
    close(sockfd);  
    printf("END\n");
}