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
std::string oper_password = "puk";

class Client;
void closeAndErase(std::vector<Client*> clients, int i);
int checkExistingNicknames(std::vector<Client*> clients, std::string const &nickname);


std::string makeStringAfterPrefix(std::vector<std::string> cmd)
{
    std::vector<std::string>::iterator it = cmd.begin();
    std::vector<std::string>::iterator ite = cmd.end();
    while (it != ite)
    {
        if ((*it)[0] == ':')
            break;
        it++;
    }
    (*it).erase(0, 1);
    std::string ret;
    ret.erase();
    while (it != ite)
    {
        ret.append(*it);
        if (it + 1 != ite)
            ret.append(" ");
        it++;
    }
    return ret;
}

class Client
{
public:

    int clientSocket;

private:
    bool isAutorized;
    bool password;
    bool away;
    bool oper;
    std::string username;
    std::string realname;
    std::string nick;
    std::string away_msg;
public:
    Client(int sock) : clientSocket(sock)
    {
        isAutorized = 0;
        away = 0;
        oper = 0;
        nick.clear();
        realname.clear();
        username.clear();
    }

    void setAwayMsg(std::string const &str) { away_msg = str; }
    std::string getAwayMsg() { return away_msg; }

    void setAway(bool t_f)
    {
        away = t_f;
    }

    bool getAway() { return away; }

    std::string &getUsername() { return username; }
    std::string &getRealname() { return realname; }

    void makeOper() { oper = true; }

    void setPassword(std::string pass, std::vector<Client*> clients, int i)
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
        if (pass[0] == ':')
            pass = pass.substr(1, pass.size() - 1);
        if (g_password == pass)
            password = true;
        else
            closeAndErase(clients, i);
    }
    bool getPass() { return password; }
    void setNick(std::vector<Client*> clients, int i, std::string nickname)
    {
        if ((checkExistingNicknames(clients, nickname)) == -1)
            nick = nickname;
        else
            send(clients[i]->clientSocket, "ERR_NICKCOLLISION\n", 19, 0);
    }

    std::string &getNick()  { return nick; }
    void setName(std::vector<std::string> cmd, int i, std::vector<Client*> clients)
    {
        if (cmd.size() < 5)
        {
            send(clientSocket, "ERR_NEEDMOREPARAMS\n", 20, 0);
            return;
        }
        username = cmd[1];
        if (cmd[4][0] == ':')
            realname = makeStringAfterPrefix(cmd);
        else
            realname = cmd[4];
    }
    void checkIfAutorized()
    {
        if (password && !realname.empty() &&  !nick.empty() && !username.empty())
        {
            isAutorized = true;
            std::string msg;
            msg = ":server 375 " + nick + " :- server Message of the day -\n";
            send(clientSocket, msg.c_str(), msg.size(), 0);
            msg = ":server 372 " + nick + " : ДАШКА КАКАШКА\n";
            send(clientSocket, msg.c_str(), msg.size(), 0);
            msg = ":server 376 " + nick + " :End of /MOTD command\n";
            send(clientSocket, msg.c_str(), msg.size(), 0);
            std::cout << "new registered user " << nick << std::endl;
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

int checkExistingNicknames(std::vector<Client*> clients, std::string const &nickname)
{
    int i = 0;
    int len = clients.size();
    while (i < len)
    {
        if (clients[i]->getNick() == nickname)
            return i;
        i++;
    }
    return -1;
}


void getRegistered(std::vector<std::string> cmd, int i, std::vector<Client*> clients)
{
    if (cmd[0] == "PASS")
        clients[i]->setPassword(cmd[1], clients, i);
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
            clients[i]->setNick(clients, i, cmd[1]);
    }
    clients[i]->checkIfAutorized();
}


void privmsg(std::vector<std::string> cmd, std::vector<Client*> clients, int i, bool notice)
{
    std::vector<int> ind;
    std::vector<std::string>::iterator it_cmd = cmd.begin() + 1;
    std::vector<std::string>::iterator ite_cmd = cmd.end();
    while (it_cmd != ite_cmd && (*it_cmd)[0] != ':')
    {
        ind.push_back(checkExistingNicknames(clients, *it_cmd));
        ++it_cmd;
    }
    std::string stringToSend = makeStringAfterPrefix(cmd);
    if (stringToSend.empty())
    {
        send(clients[i]->clientSocket, "ERR_NOTEXTTOSEND\n", 18, 0);
        return;
    }
    std::string nick = clients[i]->getNick();
    std::vector<int>::iterator it_ind = ind.begin();
    std::vector<int>::iterator ite_ind = ind.end();
    while (it_ind != ite_ind)
    {
        if (*it_ind == -1)
        {
            send(clients[i]->clientSocket, "ERR_NOSUCHNICK\n", 16, 0);
            ++it_ind;
            continue;
        }
        std::string finalString= ":" + nick + "!";
        std::string tmp = clients[i]->getUsername();
        finalString += tmp;
        finalString += "@127.0.0.1 PRIVMSG ";
        tmp = clients[*it_ind]->getNick();
        finalString += tmp;
        finalString += " :";
        finalString += stringToSend;
        finalString += "\n";
        
        send(clients[*it_ind]->clientSocket, finalString.c_str(), finalString.size(), 0);
        std::cout << finalString << std::endl;

        if (clients[*it_ind]->getAway() && !notice)
        {
            std::string resp = clients[*it_ind]->getNick();
            std::string msg = clients[*it_ind]->getAwayMsg();
            send(clients[i]->clientSocket, "Automatic respond from ", 24, 0);
            send(clients[i]->clientSocket, resp.c_str(), resp.size(), 0);
            send(clients[i]->clientSocket, ": ", 2, 0);
            send(clients[i]->clientSocket, msg.c_str(), msg.size(), 0);
            send(clients[i]->clientSocket, "\n", 1, 0);
        }
        ++it_ind;
    }
}

void away(std::vector<std::string> cmd, std::vector<Client*> clients, int i)
{
    if (cmd.size() == 1)
    {
        clients[i]->setAway(false);
        return;
    }
    clients[i]->setAway(true);
    clients[i]->setAwayMsg(makeStringAfterPrefix(cmd));
}

void userhost(std::vector<std::string> cmd, std::vector<Client*> clients, int i)
{
    if (cmd.size() == 1)
    {
        send(clients[i]->clientSocket, "ERR_NEEDMOREPARAMS\n", 20, 0);
        return;
    }
    int params = 5;
    std::string tmp;
    std::vector<std::string>::iterator it_cmd = cmd.begin() + 1;
    std::vector<std::string>::iterator ite_cmd = cmd.end();
    while (params-- && it_cmd != ite_cmd)
    {
        std::vector<Client*>::iterator it_cl = clients.begin();
        std::vector<Client*>::iterator ite_cl = clients.end();
        while (it_cl != ite_cl)
        {
            if (*it_cmd == (*it_cl)->getNick())
            {
                tmp = (*it_cl)->getNick();
                send(clients[i]->clientSocket, "Nickname: ", 10, 0);
                send(clients[i]->clientSocket, tmp.c_str(), tmp.size(), 0);
                send(clients[i]->clientSocket, "\n", 1, 0);
                send(clients[i]->clientSocket, "Username: ", 10, 0);
                tmp = (*it_cl)->getUsername();
                send(clients[i]->clientSocket, tmp.c_str(), tmp.size(), 0);
                send(clients[i]->clientSocket, "\n", 1, 0);
                send(clients[i]->clientSocket, "Real name: ", 11, 0);
                tmp = (*it_cl)->getRealname();
                send(clients[i]->clientSocket, tmp.c_str(), tmp.size(), 0);
                send(clients[i]->clientSocket, "\n\n", 2, 0);
            }
            it_cl++;
        }
        it_cmd++;
    }
}

void oper(std::vector<std::string> cmd, std::vector<Client*> clients, int i)
{
    if (cmd.size() < 3)
    {
        send(clients[i]->clientSocket, "ERR_NEEDMOREPARAMS\n", 20, 0);
        return;
    }
    int a;
    if ((a = checkExistingNicknames(clients, cmd[1])) != -1 && cmd[2] == oper_password)
        clients[i]->makeOper();
}

void whichCmd(std::vector<std::string> cmd, int i, std::vector<Client*> clients)
{
    if (cmd.size() == 0)
        return;
    if (!clients[i]->getIsAutorized())
        getRegistered(cmd, i, clients);
    else
    {
        if (cmd[0] == "PASS")
            clients[i]->setPassword(cmd[1], clients, i);
        if (cmd[0] == "USER")
            clients[i]->setName(cmd, i, clients);
        if (cmd[0] == "NICK")
            clients[i]->setNick(clients, i, cmd[1]);
        if (cmd[0] == "PRIVMSG")
            privmsg(cmd, clients, i, false);
        if (cmd[0] == "AWAY")
            away(cmd, clients, i);
        if (cmd[0] == "NOTICE")
            privmsg(cmd, clients, i, true);
        if (cmd[0] == "USERHOST")
            userhost(cmd, clients, i);
        if (cmd[0] == "OPER")
            oper(cmd, clients, i);
    }

}

void prepareBuf(char *buf, int i, std::vector<Client*> clients)
{
    std::string tmp = buf;
    if (tmp.size() == 1)
        return;
    int n;
    std::string newBuf;
    std::vector<std::string> cmd;
    while ((n = tmp.find("\n")) != std::string::npos)
    {
        newBuf = tmp.substr(0, n);
        tmp = tmp.substr(n + 1, tmp.size() - n);
        if (newBuf[newBuf.size() - 1] == '\r')
            newBuf.erase(newBuf.size() - 1);
        cmd = splitString(newBuf);
        whichCmd(cmd, i, clients);           
    }
    if (tmp[tmp.size() - 1] == '\r')
        tmp.erase(tmp.size() - 1);
    cmd = splitString(tmp);
    whichCmd(cmd, i, clients); 
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
                        prepareBuf(buf, i, clients);
                        // whichCmd(buf, i, clients);
                    }
                    fds[i].revents = 0;
                }
            }
        }
    }
    close(sock);
}
