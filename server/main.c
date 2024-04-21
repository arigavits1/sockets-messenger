#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/time.h>
#include <stdbool.h>

#define PORT 62
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

struct Data
{
    char buffer[256];
} typedef Data;
int max_sockets;
bool should_quit = false;

void quit(int clientfd[], int sockfd, int exit_code)
{
    for (int i = 0; i < 2; i++)
    {
        close(clientfd[i]);
    }
    close(sockfd);
    printf("Quitting...\n");
    exit(exit_code);
}

void handleClientMessage(int clientfd[], int index)
{
    Data sendData;
    char buffer[sizeof(sendData)];
    ssize_t bytes_received = recv(clientfd[index], buffer, sizeof(buffer) - 1, 0);
    memcpy(&sendData, buffer, sizeof(sendData));
    if (bytes_received < 0)
    {
        perror("Recv failed");
        return;
    }
    sendData.buffer[bytes_received - sizeof(sendData) + sizeof(sendData.buffer) - 1] = '\0';
    memset(buffer, 0, sizeof(buffer));
        
    printf("%s\n", sendData.buffer);
    memcpy(buffer, &sendData, sizeof(sendData));
    for (int i = 0; i < max_sockets; i++)
    {
        send(clientfd[i], buffer, sizeof(buffer), 0);
    }
        
    memcpy(&sendData, buffer, sizeof(sendData));
    if (strcmp(buffer, "bye") == 0)
    {
        should_quit = true;
        return;
    }
    memset(&buffer, 0, sizeof(buffer));
    
    return;
}

int main(int argc, char* argv[])
{
    char* argument = argv[1];
    if (argument == NULL)
    {
        fprintf(stderr, "Invalid argument! Breaking...\n");
        exit(1);
    }
    max_sockets = (int)*argument - '0';
    printf("%d\n", max_sockets);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Socket error");
        exit(1);
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &(server_address.sin_addr));

    if (bind(sockfd, (const struct sockaddr*)&server_address, sizeof(server_address)) == -1)
    {
        perror("Bind error");
        close(sockfd);
        exit(1);
    }
    
    if (listen(sockfd, 2) == -1)
    {
        perror("Listen error");
        close(sockfd);
        exit(1);
    }

    int clientfd[max_sockets];
    for (int i = 0; i < max_sockets; i++)
    {
        clientfd[i] = accept(sockfd, NULL, NULL);
    }

    fd_set readfds;
    struct timeval tv;
    int max_fd = clientfd[0];
    for (int i = 0; i < max_sockets; i++)
    {
        if (clientfd[i] > max_fd)
        {
            max_fd = clientfd[i];
        }
    }

    printf("Past setup\n");
    for(;;)
    {
        FD_ZERO(&readfds);
        for (int i = 0; i < max_sockets; i++)
        {
            FD_SET(clientfd[i], &readfds);
        }
        
        tv.tv_sec = 0;
        tv.tv_usec = 500000;

        int select_result = select(max_fd + 1, &readfds, NULL, NULL, &tv);

        if (select_result == -1)
        {
            perror("Select error");
            break;
        }
        else if (select_result == 0)
        {
            continue;
        }

        for (int i = 0; i < max_sockets; i++)
        {
            if (FD_ISSET(clientfd[i], &readfds))
            {
                handleClientMessage(clientfd, i);
            }
        }
        if (should_quit)
        {
            quit(clientfd, sockfd, 0);
        }
    }
}