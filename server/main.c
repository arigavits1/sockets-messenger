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

#define PORT 62

struct Data
{
    char buffer[256];
} typedef Data;

void* recvFromClient(void* arg)
{
    int* clientfd = (int*)arg;
    Data sendData;
    char buffer[sizeof(sendData)];
    for (;;)
    {
        ssize_t bytes_received = recv(clientfd[0], buffer, sizeof(buffer) - 1, 0);
        memcpy(&sendData, buffer, sizeof(sendData));
        if (bytes_received < 0)
        {
            perror("Recv failed");
            return NULL;
        }
        else if (strcmp(sendData.buffer, "") == 0)
        {
            continue;
        }
        sendData.buffer[bytes_received - sizeof(sendData) + sizeof(sendData.buffer) - 1] = '\0';
        memset(buffer, 0, sizeof(buffer));
        
        printf("%s\n", sendData.buffer);
        memcpy(buffer, &sendData, sizeof(sendData));
        for (int i = 0; i < 2; i++)
        {
            send(clientfd[i], buffer, sizeof(buffer), 0);
        }
        
        memcpy(&sendData, buffer, sizeof(sendData));
        if (strcmp(buffer, "bye") == 0)
        {
            printf("Breaking...\n");
            break;
        }
        memset(&buffer, 0, sizeof(buffer));
    }
    return NULL;
}

int main()
{
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

    int clientfd[2] = {};
    int clientfd2[2] = {};
    for (int i = 0; i < 2; i++)
    {
        clientfd[i] = accept(sockfd, NULL, NULL);
    }
    clientfd2[0] = clientfd[1];
    clientfd2[1] = clientfd[0];

    pthread_t thread[2] = {};
    pthread_create(&thread[0], NULL, recvFromClient, (void*)clientfd);
    pthread_create(&thread[1], NULL, recvFromClient, (void*)clientfd2);

    for (int i = 0; i < 2; i++)
    {
        pthread_join(thread[i], NULL);
    }

    for (int i = 0; i < 2; i++)
    {
        close(clientfd[i]);
    }
    close(sockfd);
}
