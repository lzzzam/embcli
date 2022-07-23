#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1)

typedef struct payload_t {
    uint8_t length;
    uint8_t group;
    uint8_t id;
    uint8_t buf[10];
} payload;

#pragma pack()

int PORT = 2300;
#define BUFFSIZE 512
char buff[BUFFSIZE];
int ssock, csock;
int nread;
struct sockaddr_in client;
int clilen = sizeof(client);


void executeClient(void)
{
    std::string command = "python client.py";
    popen(command.c_str(), "r");   
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

// printf("Sending it back.. ");
// sendMsg(csock, p, sizeof(payload));
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


TEST_CASE( "onChip Terminal Simulation") 
{
    
    bzero(buff, BUFFSIZE);

    ssock = createSocket(PORT);
    printf("Server listening on port %d\n", PORT);

    //executeClient();

    acceptConnection();
    printf("----------------------------\n");
    printf("Opening connection with client\n"); 
    printf("Accepted connection from %s\n", inet_ntoa(client.sin_addr));

    SECTION("Receive a command")
    {
        // uint8_t length = read(csock, &buff[0], 1);
        // printf("Received length=%d\n", length);

        // for (uint8_t i=1; i<length; i++)
        // {
        //     read(csock, &buff[i], 1);
        // }

        while ((nread=read(csock, buff, BUFFSIZE)) > 0)
        {
            printf("Received %d bytes\n", nread);
            payload *p = (payload*) buff;
        }
        
        payload *p = (payload*) buff;
        printf("Received contents: length=%d, group=%d, id=%d\n", p->length, p->group, p->id);

        // while ((nread=read(csock, buff, BUFFSIZE)) > 0)
        // {
        //     printf("Received %d bytes\n", nread);
        //     payload *p = (payload*) buff;
        //     printf("Received contents: length=%d, group=%d, id=%d\n",
        //             p->length, p->group, p->id);
        // }
    }
    
    printf("Closing connection to client\n");
    printf("----------------------------\n");
    closeSocket(csock);

}