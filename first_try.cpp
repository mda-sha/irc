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
#include <cstring>

// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <cstdlib>


std::string g_password = "puk";

class Client;
void closeAndErase(std::vector<Client*> clients, int i);

class Client
{
public:

    int clientSocket;

private:
    bool isAutorized;
    bool password;
    std::string username;
    std::string realname;
    std::string nick;
public:
    Client(int sock) : clientSocket(sock)
    {
        isAutorized = 0;
        nick.clear();
        realname.clear();
        username.clear();
    }

    void setPassword(std::string const &pass)
    {
        if (pass.length() == 0)
        {
            send(clientSocket, "ERR_NEEDMOREPARAMS\n", 20, 0);
            return;
        }
        if (isAutorized)
        {
            send(clientSocket, "ERR_ALREADYREGISTRED\n", 20, 0);
            return;
        }
        if (g_password == pass)
            password = true;
    }
    bool getPass() { return password; }
    void setNick(std::string const &nick) { this->nick = nick; }
    std::string &getNick()  { return nick; }
    void setName(std::vector<std::string> cmd, int i, std::vector<Client*> clients)
    {
        if (nick.empty())
        {
            closeAndErase(clients, i);
            return;
        }
        else
        {
            if (cmd.size() < 5)
            {
                send(clientSocket, "ERR_NEEDMOREPARAMS\n", 20, 0);
                return;
            }
            username = cmd[1];
            if (cmd[4][0] == ':')
            {
                cmd[4].erase(0, 1);
                int i = 4;
                int len = cmd.size();
                while (i < len)
                {
                    realname.append(cmd[i]);
                    if (i + 1 < len)
                        realname.append(" ");
                    i++;
                }
            }
            else
                realname = cmd[4];
        }
    }
    void checkIfAutorized()
    {
        if (password && !realname.empty() &&  !nick.empty() && !username.empty())
        {
            isAutorized = true;
            send(clientSocket, "You are registered\n", 20, 0);
        }
    }
    bool getIsAutorized()
    {
        return isAutorized;
    }
};

void closeAndErase(std::vector<Client*> clients, int i)
{
    close(clients[i]->clientSocket);
    std::vector<Client*>::iterator it = clients.begin() + i;
    clients.erase(it);
}


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

bool checkExistingNicknames(std::vector<Client*> clients, std::string const &nickname)
{
    std::vector<Client*>::iterator it = clients.begin();
    std::vector<Client*>::iterator ite = clients.end();
    while (it != ite)
    {
        if ((*it)->getNick() == nickname)
            return true;
        it++;
    }
    return false;
}


void getRegistered(std::vector<std::string> cmd, int i, std::vector<Client*> clients)
{
    if (cmd[0] == "PASS")
        clients[i]->setPassword(cmd[1]);
    else
    {
        if (!clients[i]->getPass())
            closeAndErase(clients, i);
    }
    if (cmd[0] == "USER")
    {
        if (!clients[i]->getPass())
            closeAndErase(clients, i);
        else
            clients[i]->setName(cmd, i, clients);
    }
    if (cmd[0] == "NICK" && clients[i]->getPass())
    {
        if (!clients[i]->getPass())
            closeAndErase(clients, i);
        else
        {
            if (!checkExistingNicknames(clients, cmd[1]))
                clients[i]->setNick(cmd[1]);
            else
                send(clients[i]->clientSocket, "ERR_NICKCOLLISION\n", 19, 0);
        }
    }
    clients[i]->checkIfAutorized();
}


void whichCmd(char *buf, int i, std::vector<Client*> clients)
{
    std::string tmp = buf;
    if (tmp.size() == 1)
        return;
    tmp.erase(tmp.size() - 1);
    tmp.append("\0");
    std::vector<std::string> cmd = splitString(tmp);
    if (!clients[i]->getIsAutorized())
        getRegistered(cmd, i, clients);

}

int main()
{
    int port = 1665;
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
	fcntl(sock, F_SETFL, O_NONBLOCK); ///////////////////// разобраться, что это. без этого не работает


    std::vector<struct pollfd> fds;
    std::vector<Client*> clients;
    while (1)
    {
        socklen_t clientSize = sizeof(addr);
        int clientSocket = accept(sock, (struct sockaddr*)&addr, &clientSize);
        if (clientSocket >= 0)
        {
    		// char	host[INET_ADDRSTRLEN];/////
	    	// inet_ntop(AF_INET, &(addr.sin_addr), host, INET_ADDRSTRLEN); ////

            clients.push_back(new Client(clientSocket));
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
                    char buf[512];
                    memset(buf, 0, 100);
                    int bytesRecv;
                    while ((bytesRecv = recv(fds[i].fd, buf, 100, 0) > 0))
                    {
                        whichCmd(buf, i, clients);
                    }
                    fds[i].revents = 0;
                }
            }
        }
    }
    close(sock);
}