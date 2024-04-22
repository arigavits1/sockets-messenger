#pragma once
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

#define PORT 62

typedef struct
{
    char buffer[256];
} Data;

typedef struct
{
    int max;
    size_t size;
    int* list;
} ClientData;

ClientData clientfd = {};

void quit(ClientData* clientfd, int sockfd, int exit_code)
{
    for (int i = 0; i < clientfd->max - 1; i++)
    {
        close(clientfd->list[i]);
    }
    close(sockfd);
    free(clientfd->list);
    printf("Quitting...\n");
    exit(exit_code);
}

int handleClientMessage(ClientData* clientfd, int index)
{
    Data sendData;
    char buffer[sizeof(sendData)];
    ssize_t bytes_received = recv(clientfd->list[index], buffer, sizeof(buffer) - 1, 0);
    memcpy(&sendData, buffer, sizeof(sendData));
    if (bytes_received < 0)
    {
        perror("Recv failed");
        return 1;
    }
    sendData.buffer[bytes_received - sizeof(sendData) + sizeof(sendData.buffer) - 1] = '\0';
    memset(buffer, 0, sizeof(buffer));
        
    printf("%s\n", sendData.buffer);
    memcpy(buffer, &sendData, sizeof(sendData));

    if (strcmp(sendData.buffer, "bye") == 0)
    {
        int temp_sock;
        temp_sock = clientfd->list[index];
        clientfd->list[index] = clientfd->list[clientfd->max - 1];
        clientfd->list[clientfd->max - 1] = temp_sock;
        send(clientfd->list[clientfd->max - 1], buffer, sizeof(buffer), 0);
        close(clientfd->list[clientfd->max - 1]);
        if (clientfd->max == 1)
        {
            return -1;
        }
        void *temp = realloc(clientfd->list, clientfd->size - sizeof(int));
        if (temp == NULL) 
        {
            perror("realloc");
            return 1;
        }
        clientfd->size -= sizeof(int);
        clientfd->max--;
        return 0;
    }

    for (int i = 0; i < clientfd->max; i++)
    {
        send(clientfd->list[i], buffer, sizeof(buffer), 0);
    }

    memset(&buffer, 0, sizeof(buffer));
    
    return 0;
}