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

struct Data
{
    char buffer[256];
} typedef Data;
int max_sockets;
size_t clientfd_size;

void quit(int clientfd[], int sockfd, int exit_code)
{
    for (int i = 0; i < max_sockets - 1; i++)
    {
        close(clientfd[i]);
    }
    close(sockfd);
    free(clientfd);
    printf("Quitting...\n");
    exit(exit_code);
}

int handleClientMessage(int clientfd[], int index)
{
    Data sendData;
    char buffer[sizeof(sendData)];
    ssize_t bytes_received = recv(clientfd[index], buffer, sizeof(buffer) - 1, 0);
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
        temp_sock = clientfd[index];
        clientfd[index] = clientfd[max_sockets - 1];
        clientfd[max_sockets - 1] = temp_sock;
        send(clientfd[max_sockets - 1], buffer, sizeof(buffer), 0);
        close(clientfd[max_sockets - 1]);
        if (max_sockets == 1)
        {
            return -1;
        }
        void *temp = realloc(clientfd, clientfd_size - sizeof(int));
        if (temp == NULL) 
        {
            perror("realloc");
            return 1;
        }
        clientfd_size -= sizeof(int);
        max_sockets--;
        return 0;
    }

    for (int i = 0; i < max_sockets; i++)
    {
        send(clientfd[i], buffer, sizeof(buffer), 0);
    }

    memset(&buffer, 0, sizeof(buffer));
    
    return 0;
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

    clientfd_size = sizeof(int) * max_sockets - 1;
    int* clientfd = (int*)malloc(clientfd_size);
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
                int exit_code = handleClientMessage(clientfd, i);
                if (exit_code == 1)
                {
                    fprintf(stdout, "Error while handling client message\n");
                    quit(clientfd, sockfd, exit_code);
                }
                if (exit_code == -1)
                {
                    quit(clientfd, sockfd, 0);
                }
            }
        }
    }
}