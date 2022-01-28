// #pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <vector>
#include <sys/fcntl.h>

// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <cstdlib>


class Client
{
public:
    std::string name;
    std::string nick;
    int clientSocket;
    Client(int sock) : clientSocket(sock) {}
};

std::vector<std::string> splitString(std::string buf)
{
    std::vector<std::string> strings;
    int i = 0;
    int len = buf.length();
    std::size_t tmp;
    while (i < len)
    {
        tmp = buf.find(' ', i);
        if (tmp == std::string::npos)
        {
            std::string newString = buf.substr(i, len - i);
            strings.push_back(newString);
            break;
        }
        std::string newString = buf.substr(i, tmp - i);
        if (newString.length() > 0)
            strings.push_back(newString);
        i = tmp + 1;
    }
    return strings;
}

void whichCmd(char *buf)
{
    std::vector<std::string> cmd = splitString(buf);
    std::cout << cmd[0] << std::endl;
    std::cout << cmd[1] << std::endl;
}

int main()
{
    int port = 1661;
    sockaddr_in addr;

    if (port < 1024)
    {
        std::cout << "port is reserved" << std::endl;
        return 1;
    }
    addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        std::cout << "*" << std::endl;
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        std::cout << "*" << std::endl;
    if (listen(sock, SOMAXCONN) == -1)
        std::cout << "*" << std::endl;
	fcntl(sock, F_SETFL, O_NONBLOCK); /////////////////////


    std::vector<struct pollfd> fds;
    // std::vector<Client*> clients;
    while (1)
    {
        // sockaddr_in client;
        socklen_t clientSize = sizeof(addr);
        int clientSocket = accept(sock, (struct sockaddr*)&addr, &clientSize);
        if (clientSocket >= 0)
        {
    		char	host[INET_ADDRSTRLEN];/////
	    	inet_ntop(AF_INET, &(addr.sin_addr), host, INET_ADDRSTRLEN); ////

           // clients.push_back(new Client(clientSocket));
            struct pollfd b;
            b.fd = clientSocket;
            b.events = POLLIN;
            b.revents = 0;
            fds.push_back(b);
            std::cout << "clientSocket = " << clientSocket << std::endl;
        }

        int a = poll(fds.data(), fds.size(), 1000);
        if (a > 0)
        {
            for (int i = 0; i < fds.size(); i++)
            {
                if (fds[i].revents & POLLIN)
                {
                    char buf[100];
                    memset(buf, 0, 100);
                    int bytesRecv;
                    while ((bytesRecv = recv(fds[i].fd, buf, 100, 0) > 0))
                    {
                        whichCmd(buf);
                    }
                    fds[i].revents = 0;
                }
            }
        }
    }
    close(sock);
}