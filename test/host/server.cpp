#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write

#define BUFFSIZE 512

/** Declare prototypes */ 
int createSocket(int port);
void acceptConnection(void);
void closeSocket(int sock);
void sendMsg(int sock, void* msg, uint32_t msgsize);

int ssock, csock;
struct sockaddr_in client;

int main(int argc, char const* argv[])
{
    int PORT = 2300;
    char buff[BUFFSIZE];

    bzero(buff, BUFFSIZE);

    ssock = createSocket(PORT);
    printf("Server listening on port %d\n", PORT);

    acceptConnection();
    printf("----------------------------\n");
    printf("Opening connection with client\n"); 
    printf("Accepted connection from %s\n", inet_ntoa(client.sin_addr));

    while(1)
    {
        if (read(csock, buff, BUFFSIZE) > 0)
        {
            printf("%s", buff);
        }
    }
}


int createSocket(int port)
{
    int sock, err;
    struct sockaddr_in server;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("ERROR: Socket creation failed\n");
        exit(1);
    }
    printf("Socket created\n");

    bzero((char *) &server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        printf("ERROR: Bind failed\n");
        exit(1);
    }
    printf("Bind done\n");

    listen(sock , 3);

    return sock;
}

void acceptConnection(void)
{
    int clilen = sizeof(client);

    csock = accept(ssock, (struct sockaddr *)&client, (socklen_t *)&clilen);
    if (csock < 0)
    {
        printf("Error: accept() failed\n");
    }
}

void closeSocket(int sock)
{
    close(sock);
    return;
}


void sendMsg(int sock, void* msg, uint32_t msgsize)
{
    if (write(sock, msg, msgsize) < 0)
    {
        printf("Can't send message.\n");
        closeSocket(sock);
        exit(1);
    }
    printf("Message sent (%d bytes).\n", msgsize);
    return;
}