#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define PORT 1234              
#define MAXDATALEN 1024

int sockfd;
int n;                        
struct sockaddr_in serv_addr; 
char buffer[MAXDATALEN];
char buf[10];

void *quit();
void *chat_write(int);
void *chat_read(int);

int main(int argc, char *argv[])
{

    pthread_t t1, t2; 
    // use t1 and t2 for reading and writing
    if (argc != 4){
        printf("Server ip + port + username \n");
        exit(1);
    }

    // socket creating
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        printf("socket error\n");
    else
        printf("socket created\n");

    // set info
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // username
    bzero(buf, 10);
    strcpy(buf, argv[3]);
    buf[strlen(buf)] = ':';

    // client connect to server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        printf("client connect error\n");
        exit(0);
    }
    else
        printf("joined\n");

    send(sockfd, buf, strlen(buf), 0);
    pthread_create(&t2, NULL, (void *)chat_write, (void *) (intptr_t) sockfd); 
    pthread_create(&t1, NULL, (void *)chat_read, (void *) (intptr_t) sockfd);  
    pthread_join(t2, NULL);
    pthread_join(t1, NULL);
    return 0;

}

void *chat_read(int sockfd){
    while (1){
        n = recv(sockfd, buffer, MAXDATALEN - 1, 0);
        if (n == 0)
        {
            printf("server closed !\n");
            exit(0);
        }

        if (n > 0)
        {
            printf("- %s", buffer);
            bzero(buffer, MAXDATALEN);
        }
    }
}

void *chat_write(int sockfd){
    while (1){
        fgets(buffer, MAXDATALEN - 1, stdin);
        if (strlen(buffer) - 1 > sizeof(buffer)){
            printf("buffer full ! \n");
            bzero(buffer, MAXDATALEN);
            __fpurge(stdin);
        }

        n = send(sockfd, buffer, strlen(buffer), 0);
        if (strncmp(buffer, "/quit", 5) == 0){
            exit(0);
        }
        bzero(buffer, MAXDATALEN);
    }
}
