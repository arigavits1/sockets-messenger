#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <atomic>

#define PORT 62

int networkSetup()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (!sockfd)
    {
        perror("Socket error");
        return -1;
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, "192.168.1.6", &(server_address.sin_addr));

    std::cout << "Attempting to connect to server...\n";
    int connection_status = connect(sockfd, (const sockaddr*)&server_address, sizeof(server_address));
    if (connection_status == -1)
    {
        perror("Connect error");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

void sendToServer(int sockfd, std::string& message, size_t messageSize)
{
    ssize_t bytesSent = send(sockfd, message.c_str(), messageSize, 0);
    if (bytesSent < 0)
    {
        perror("Send error");
        return;
    }
}

void* recvFromServer(int sockfd, std::string* imguiBuffer, std::atomic<bool>& should_run)
{
    char buffer[256];
    fd_set readfds;
    struct timeval tv;

    for (;;)
    {
        if (should_run == 0)
        {
            break;
        }
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        tv.tv_sec = 0;
        tv.tv_usec = 500;

        int select_result = select(sockfd + 1, &readfds, NULL, NULL, &tv);

        if (select_result == -1)
        {
            perror("Select error");
            break;
        }
        else if (select_result == 0)
        {
            continue;
        }

        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        buffer[bytes_received] = '\0';

        if (bytes_received < 0)
        {
            perror("Error receiving data");
            break;
        }
        else if (bytes_received == 0)
        {
            break;
        }

        std::string tempBuffer = buffer;
        *imguiBuffer += "\n" + tempBuffer;
        std::cout << buffer << "\n" << std::flush;

        if (strcmp(buffer, "bye") == 0)
        {
            std::cout << "Disconnecting..." << std::endl;
            should_run = false;
            break;
        }
    }    

    return NULL;
}
