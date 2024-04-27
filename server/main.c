#include "header.h"

int main(int argc, char* argv[])
{
    sockData.sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockData.sockfd == -1)
    {
        perror("Socket error");
        exit(1);
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, "192.168.1.6", &(server_address.sin_addr));

    if (bind(sockData.sockfd, (const struct sockaddr*)&server_address, sizeof(server_address)) == -1)
    {
        perror("Bind error");
        close(sockData.sockfd);
        exit(1);
    }
    
    if (listen(sockData.sockfd, 2) == -1)
    {
        perror("Listen error");
        close(sockData.sockfd);
        exit(1);
    }

    SetupSockets(&sockData);

    fd_set readfds;
    struct timeval tv;        
    tv.tv_sec = 0;
    tv.tv_usec = 50000;

    for(;;)
    {
        FD_ZERO(&readfds);
        for (int i = 0; i < sockData.max_clients; i++)
        {
            FD_SET(sockData.client_array[i], &readfds);
        }
        FD_SET(sockData.sockfd, &readfds);

        int select_result = select(sockData.max_fd + 1, &readfds, NULL, NULL, &tv);

        if (select_result == -1)
        {
            perror("Select error");
            break;
        }
        else if (select_result == 0)
        {
            continue;
        }
        
        for (int i = 0; i < sockData.max_clients; i++)
        {
            if (FD_ISSET(sockData.client_array[i], &readfds))
            {
                int exit_code = handleClientMessage(&sockData, i);
                if (exit_code == 1)
                {
                    fprintf(stdout, "Error while handling client message\n");
                    quit(&sockData, exit_code);
                }
                if (exit_code == -1)
                {
                    free(sockData.client_array);
                    SetupSockets(&sockData);
                }
            }
        }
        if (FD_ISSET(sockData.sockfd, &readfds))
        {
            sockData.client_size += sizeof(int);
            void *temp = realloc(sockData.client_array, sockData.client_size);
            if (temp == NULL) 
            {
                perror("realloc");
                return 1;
            }
            sockData.client_array[sockData.max_clients] = accept(sockData.sockfd, NULL, NULL);

            if (sockData.client_array[sockData.max_clients] > sockData.max_fd)
            {
                sockData.max_fd = sockData.client_array[sockData.max_clients];
            }

            sockData.max_clients++;
        }
    }
}