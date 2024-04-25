#include "header.h"

int main(int argc, char* argv[])
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

    clientfd.max = 1;
    clientfd.size = sizeof(int) * clientfd.max - 1;
    clientfd.list = (int*)malloc(clientfd.size);
    int max_fd = clientfd.list[0];
    for (int i = 0; i < clientfd.max; i++)
    {
        clientfd.list[i] = accept(sockfd, NULL, NULL);
        if (clientfd.list[i] > max_fd)
        {
            max_fd = clientfd.list[i];
        }
    }
    if (sockfd > max_fd)
    {
        max_fd = sockfd;
    }

    fd_set readfds;
    struct timeval tv;        
    tv.tv_sec = 0;
    tv.tv_usec = 50000;

    for(;;)
    {
        FD_ZERO(&readfds);
        for (int i = 0; i < clientfd.max; i++)
        {
            FD_SET(clientfd.list[i], &readfds);
        }
        FD_SET(sockfd, &readfds);

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

        for (int i = 0; i < clientfd.max; i++)
        {
            if (FD_ISSET(clientfd.list[i], &readfds))
            {
                int exit_code = handleClientMessage(&clientfd, i);
                if (exit_code == 1)
                {
                    fprintf(stdout, "Error while handling client message\n");
                    quit(&clientfd, sockfd, exit_code);
                }
                if (exit_code == -1)
                {
                    quit(&clientfd, sockfd, 0);
                }
            }
        }
        if (FD_ISSET(sockfd, &readfds))
        {
            clientfd.size += sizeof(int);
            void *temp = realloc(clientfd.list, clientfd.size);
            if (temp == NULL) 
            {
                perror("realloc");
                return 1;
            }
            clientfd.list[clientfd.max] = accept(sockfd, NULL, NULL);

            if (clientfd.list[clientfd.max] > max_fd)
            {
                max_fd = clientfd.list[clientfd.max];
            }

            clientfd.max++;
        }
    }
}