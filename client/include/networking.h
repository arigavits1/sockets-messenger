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

int networkSetup();
void* sendToServer(int sockfd, std::string& message, size_t messageSize);
void* recvFromServer(int sockfd, std::string* imguiBuffer, std::atomic<bool>& should_run);
