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
#include <stdbool.h>

#define PORT 8080
#define NAMESIZE 11

typedef struct
{
    char buffer[256];
    char name[256];
} Data;

typedef struct
{
    int max_clients;
    size_t client_size;
    int* client_array;
    int sockfd;
    int max_fd;
} SocketData;

typedef struct
{
    char** array;
    int next;
    size_t size;
} CharArray;

SocketData sockData = {};
CharArray names = {};

void sendCode(int clientfd, int code)
{
    char buffer[4];
    sprintf(buffer, "%d", code);
    send(clientfd, buffer, sizeof(buffer), 0);
}

int RemoveFromIntArray(int* array, size_t* size, int index)
{
    int temp_sock;
    size_t array_size = *size / sizeof(int);
    temp_sock = array[index];
    array[index] = array[array_size - 1];
    array[array_size - 1] = temp_sock;
    
    if (array_size == 1)
    {
        return -1;
    }

    *size -= sizeof(int);
    void *temp = realloc(array, *size);
    if (temp == NULL) 
    {
        perror("realloc");
        return 1;
    }
    return 0;
}

int RemoveFromCharArray(char** array, size_t* size, int index)
{
    char* temp_sock;
    size_t array_size = *size / sizeof(char*);
    temp_sock = array[index];
    array[index] = array[array_size - 1];
    array[array_size - 1] = temp_sock;
    
    if (array_size == 1)
    {
        return -1;
    }

    *size -= sizeof(char*);
    void *temp = realloc(array, *size);
    if (temp == NULL) 
    {
        perror("realloc");
        return 1;
    }
    return 0;
}

bool CheckIfInCharArray(char** array, size_t arrayCount, char* item)
{
    for(int i = 0; i < arrayCount; i++)
    {
        if (strcmp(array[i], item) == 0)
        {
            return true;
        }
    }
    return false;
}

void quit(SocketData* sockData, CharArray names, int exit_code)
{
    for (int i = 0; i < sockData->max_clients - 1; i++)
    {
        close(sockData->client_array[i]);
    }
    close(sockData->sockfd);
    free(sockData->client_array);
    if (names.array !=  NULL)
    {
        for(int i = 0; i < names.next; i++)
        {
            free(names.array[i]);
        }
        free(names.array);
    }
    printf("Quitting...\n");
    exit(exit_code);
}

void SetupSockets(SocketData* sockData)
{
    sockData->max_clients = 1;
    sockData->client_size = sizeof(int) * sockData->max_clients;
    sockData->client_array = (int*)malloc(sockData->client_size);
    sockData->client_array[0] = accept(sockData->sockfd, NULL, NULL);
    if (sockData->client_array[0] < 0)
    {
        fprintf(stderr, "Connection to new client failed");
        close(sockData->client_array[0]);
        free(sockData->client_array);
        SetupSockets(sockData);
    }

    names.size = sizeof(char*);
    names.array = (char**)malloc(names.size);
    names.array[0] = (char*)malloc(sizeof(char) * NAMESIZE);
    ssize_t bytes_received = recv(sockData->client_array[0], names.array[0], NAMESIZE, 0);
    names.array[0][bytes_received] = '\0';
    if (bytes_received < 0)
    {
        fprintf(stderr, "Connection to new client failed");
        free(sockData->client_array);
        free(names.array[0]);
        free(names.array);
        SetupSockets(sockData);
    }
    names.next++;
    printf("User %s has connected!\n", names.array[0]);
    sendCode(sockData->client_array[0], 200);
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

    if (strcmp(sendData.buffer, "bye") == 0)
    {
        printf("User %s has disconnected\n", sendData.name);
        fflush(stdout);
        int code = RemoveFromIntArray(sockData->client_array, &sockData->client_size, index);
        
        sockData->max_clients--;
        //send(sockData->client_array[sockData->max_clients], buffer, sizeof(buffer), 0);
        close(sockData->client_array[sockData->max_clients]);
        return code;
    }

    printf("%s\n", sendData.buffer);

    for (int i = 0; i < sockData->max_clients; i++)
    {
        send(sockData->client_array[i], buffer, sizeof(buffer), 0);
    }

    memset(&buffer, 0, sizeof(buffer));
    
    return 0;
}
