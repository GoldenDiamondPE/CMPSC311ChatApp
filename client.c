#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(){
    /*
    SOCKET
    */
    // Socket File Desc
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); //IPV4 TCP Protocol

    // Sees if sockfd ran properly
    if(sockfd ==-1){
        printf("CLIENT: Error With Socket Creation\n");
        return -1;
    } else {
        printf("CLIENT: Socket Creation Success\n");
    }    
}