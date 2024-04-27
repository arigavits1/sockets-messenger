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
    int max_clients;
    size_t client_size;
    int* client_array;
    int sockfd;
    int max_fd;
} SocketData;

SocketData sockData = {};

void quit(SocketData* sockData, int exit_code)
{
    for (int i = 0; i < sockData->max_clients - 1; i++)
    {
        close(sockData->client_array[i]);
    }
    close(sockData->sockfd);
    free(sockData->client_array);
    printf("Quitting...\n");
    exit(exit_code);
}

void SetupSockets(SocketData* sockData)
{
    sockData->max_clients = 1;
    sockData->client_size = sizeof(int) * sockData->max_clients;
    sockData->client_array = (int*)malloc(sockData->client_size);
    sockData->client_array[0] = accept(sockData->sockfd, NULL, NULL);
    sockData->max_fd = sockData->client_array[0];
    if (sockData->sockfd > sockData->max_fd)
    {
        sockData->max_fd = sockData->sockfd;
    }
}

int handleClientMessage(SocketData* sockData, int index)
{
    Data sendData;
    char buffer[sizeof(sendData)];
    ssize_t bytes_received = recv(sockData->client_array[index], buffer, sizeof(buffer) - 1, 0);
    memcpy(&sendData, buffer, sizeof(sendData));
    if (bytes_received < 0)
    {
        perror("Recv failed");
        return 1;
    }
    sendData.buffer[strlen(sendData.buffer)] = '\0';
    memset(buffer, 0, sizeof(buffer));

    memcpy(buffer, &sendData, sizeof(sendData));

    if (strcmp(sendData.buffer, "") == 0)
    {
        return 0;
    }

    printf("%s\n", sendData.buffer);

    if (strcmp(sendData.buffer, "bye") == 0)
    {
        int temp_sock;
        temp_sock = sockData->client_array[index];
        sockData->client_array[index] = sockData->client_array[sockData->max_clients - 1];
        sockData->client_array[sockData->max_clients - 1] = temp_sock;
        send(sockData->client_array[sockData->max_clients - 1], buffer, sizeof(buffer), 0);
        close(sockData->client_array[sockData->max_clients - 1]);
        if (sockData->max_clients == 1)
        {
            return -1;
        }

        sockData->client_size -= sizeof(int);
        void *temp = realloc(sockData->client_array, sockData->client_size);
        if (temp == NULL) 
        {
            perror("realloc");
            return 1;
        }
        sockData->max_clients--;
        return 0;
    }

    for (int i = 0; i < sockData->max_clients; i++)
    {
        send(sockData->client_array[i], buffer, sizeof(buffer), 0);
    }

    memset(&buffer, 0, sizeof(buffer));
    
    return 0;
}