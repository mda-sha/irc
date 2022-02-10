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
#include <csignal>
#include "channel.hpp"
#include "client.hpp"

// class Client;
// void closeAndErase(std::vector<Client*> clients, int i);
// int checkExistingNicknames(std::vector<Client*> clients, std::string const &nickname);
// std::string makeStringAfterPrefix(std::vector<std::string> cmd);

    int sock;

void sig(int sig);


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

    int checkExistingNicknames(std::string const &nickname, std::vector<Client*>clients)
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


// std::string g_password = "puk";
// std::string oper_password = "puk";

// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <cstdlib>


class irc
{
private:
    std::vector<Client*> clients;
    std::vector<Channel*> channels;
    std::string g_password;
    std::string oper_password;
    int port;

    
public:


    void closeAndErase(int i)
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

    void getRegistered(std::vector<std::string> cmd, int i)
    {
        if (cmd[0] == "PASS")
        {
            if (!clients[i]->setPassword(g_password, cmd[1], clients, i))
                return;
        }
        else
        {
            if (!clients[i]->getPass())
                closeAndErase(i);
        }
        if (cmd[0] == "USER")
        {
            if (!clients[i]->getPass())
                closeAndErase(i);
            else
                clients[i]->setName(cmd, i, clients);
        }
        if (cmd[0] == "NICK" && clients[i]->getPass())
        {
            if (!clients[i]->getPass())
                closeAndErase(i);
            else
                clients[i]->setNick(clients, i, cmd[1]);
        }
        clients[i]->checkIfAutorized();
    }

    void privmsg(std::vector<std::string> cmd, int i, bool notice)
    {
        std::vector<int> ind;
        std::vector<std::string>::iterator it_cmd = cmd.begin() + 1;
        std::vector<std::string>::iterator ite_cmd = cmd.end();
        while (it_cmd != ite_cmd && (*it_cmd)[0] != ':')
        {
            ind.push_back(checkExistingNicknames(*it_cmd, clients));
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
            if (clients[*it_ind]->getAway() && !notice)
            {
                std::string str = ":server 301 " + clients[i]->getNick() + " " + clients[*it_ind]->getNick() + " :" + clients[*it_ind]->getAwayMsg() + "\n";
                send(clients[i]->clientSocket, str.c_str(), str.size(), 0);
            }
            ++it_ind;
        }
    }

    void away(std::vector<std::string> cmd, int i)
    {
        if (cmd.size() == 1)
        {
            clients[i]->setAway(false);
            std::string stringToSend = ":server 305 ";
            stringToSend += clients[i]->getNick();
            stringToSend += " :You are no longer marked as being away\n";
            send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
            return;
        }
        clients[i]->setAway(true);
        clients[i]->setAwayMsg(makeStringAfterPrefix(cmd));
        std::string stringToSend = ":server 306 ";
        stringToSend += clients[i]->getNick();
        stringToSend += " :You have been marked as being away\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
    }

    void userhost(std::vector<std::string> cmd, int i)
    {
        std::string stringToSend;
        if (cmd.size() == 1)
        {
            stringToSend = ":server 461 " + clients[i]->getNick() + "USERHOST :Not enough parameters\n";
            send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
            return;
        }
        int params = 5;
        std::vector<std::string>::iterator it_cmd = cmd.begin() + 1;
        std::vector<std::string>::iterator ite_cmd = cmd.end();
        stringToSend = ":server 302 " + clients[i]->getNick() + " :";
        while (params-- && it_cmd != ite_cmd)
        {
            std::vector<Client*>::iterator it_cl = clients.begin();
            std::vector<Client*>::iterator ite_cl = clients.end();
            while (it_cl != ite_cl)
            {
                if (*it_cmd == (*it_cl)->getNick())
                {
                    stringToSend += *it_cmd;
                    stringToSend += "=+@127.0.0.1 ";
                }
                it_cl++;
            }
            it_cmd++;
        }
        stringToSend += "\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
    }

    void oper(std::vector<std::string> cmd, int i)
    {
        if (cmd.size() < 3)
        {
            send(clients[i]->clientSocket, "ERR_NEEDMOREPARAMS\n", 20, 0);
            return;
        }
        int a;
        if ((a = checkExistingNicknames(cmd[1], clients)) != -1 && cmd[2] == oper_password)
        {
            clients[i]->makeOper();
            send(clients[i]->clientSocket, ":server 381 :You are now an IRC operator\n", 42, 0);
        }
    }

    bool checkChannelName(std::string name)
    {
        if (name[0] != '&' && name[0] != '#')
            return false;
        if (name.find(' ') != std::string::npos || name.find(',') != std::string::npos)
            return false;
        return true;
    }

    std::string makeUsersInChannelString(std::string channelName, int i)
    {
        std::string str = ":server 353 " + clients[i]->getNick() + " = " + channelName + " :";
        std::vector<Client*>::iterator it = clients.begin();
        std::vector<Client*>::iterator ite = clients.end();
        while (it != ite)
        {
            std::vector<std::string> clientChannels = (*it)->getChannels();
            std::vector<std::string>::iterator cl_ch_it = clientChannels.begin();
            std::vector<std::string>::iterator cl_ch_ite = clientChannels.end();
            while (cl_ch_it != cl_ch_ite)
            {
                if (*cl_ch_it == channelName)
                {
                    std::vector<Channel*>::iterator ch_it = channels.begin();
                    std::vector<Channel*>::iterator ch_ite = channels.end();
                    while (ch_it != ch_ite)
                    {
                        if ((*ch_it)->getOperatorNick() == (*it)->getNick() && channelName == (*ch_it)->getName())
                        {
                            str+= "@";
                            break;
                        }
                        ++ch_it;
                    }
                    str += (*it)->getNick();
                    str+= " ";
                    break;
                }
                ++cl_ch_it;
            }
            ++it;
        }
        str.erase(str.length() - 1, 1);
        str += "\n";
        return str;
    }

    void sendMsgAfterJoin(std::vector<std::string> cmd, int i, Channel newChannel)
    {
        std::string stringToSend;


        stringToSend = ":" + clients[i]->getNick() + "!" + clients[i]->getUsername() + "@127.0.0.1 JOIN :" +  newChannel.getName() + "\n";/////отправить всем на канале
        // send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        newChannel.sendToEverybody(stringToSend, clients);
        stringToSend.clear();
        std::string topic = newChannel.getTopic();
        if (topic.empty())
            stringToSend = ":server 331 " + clients[i]->getNick() + " " + cmd[1] + " :No topic is set\n";
        else
            stringToSend = ":server 332 " + clients[i]->getNick() + " " + cmd[1] + " :" + topic + "\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        stringToSend.clear();
        stringToSend = makeUsersInChannelString(cmd[1], i);
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        stringToSend.clear();
        stringToSend = ":server 366 " + clients[i]->getNick() + " " + cmd[1] + " :End of /NAMES list\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
    }

    void join(std::vector<std::string> cmd, int i)
    {
        std::string stringToSend;
        if (cmd.size() == 1)
        {
            stringToSend = ":server 461 " + clients[i]->getNick() + "JOIN :Not enough parameters\n";
            send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
            return;
        }
        std::vector<Channel*>::iterator it = channels.begin();
        std::vector<Channel*>::iterator ite = channels.end();
        while (it != ite)
        {
            if ((*it)->getName() == cmd[1])
            {
                if ((*it)->getInviteOnly())
                {
                    stringToSend = ":server 473 " + cmd[1] + " :Cannot join channel (+i)\n";
                    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                    return;
                }
                if ((*it)->getMaxClients() != -1 && (*it)->getClients() == (*it)->getMaxClients())
                {
                    stringToSend = ":server 471 " + cmd[1] + " :Cannot join channel (+l)\n";
                    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                    return;
                }
                std::string pass = (*it)->getPass();
                if (!pass.empty() && cmd[2] != pass)
                {
                    stringToSend = ":server 475 " + cmd[1] + " :Cannot join channel (+k)\n";
                    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                    return;
                }
                if (!clients[i]->checkIfOnChannel((*it)->getName()))
                {
                    clients[i]->addToChannel(*it);
                    (*it)->addClient();
                    sendMsgAfterJoin(cmd, i, **it);
                }
                return;
            }
            ++it;
        }
        if (it == ite)
        {
            if (!checkChannelName(cmd[1]))
            {
                stringToSend = ":server 403 " + clients[i]->getNick() + " " + cmd[1] + " :No such channel\n";
                send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                return;
            }
            Channel *newChannel = new Channel(cmd[1]);
            newChannel->setOperatorNick(clients[i]->getNick());
            clients[i]->addToChannel(newChannel);
            channels.push_back(newChannel);
            sendMsgAfterJoin(cmd, i, *newChannel);
            std::cout << "new channel " << cmd[1] << " created" << std::endl;
        }
    }

    void part(std::vector<std::string> cmd, int i)
    {
        std::string stringToSend;
        if (cmd.size() < 2)
        {
            stringToSend = ":server 461 " + clients[i]->getNick() + "USERHOST :Not enough parameters\n";
            send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
            return;
        }
        std::vector<Channel*>::iterator it = channels.begin();
        std::vector<Channel*>::iterator ite = channels.end();
        while (it != ite)
        {
            if ((*it)->getName() == cmd[1])
            {
                if (clients[i]->removeChannel(cmd[1]))
                {
                    stringToSend = ":" + clients[i]->getNick() + "!" + clients[i]->getUsername() + "@127.0.0.1 PART " + cmd[1] + "\n";
                    (*it)->sendToEverybody(stringToSend, clients);
                    (*it)->removeClient();
                }
                else
                {
                    stringToSend = ":server 442 " + cmd[1] + " :You're not on that channel\n";
                    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                }
                return;
            }
            ++it;
        }
        stringToSend = ":server 403 " + clients[i]->getNick() + " " + cmd[1] + " :No such channel\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
    }

    void invite(std::vector<std::string> cmd, int i)
    {
        std::string stringToSend;
        if (cmd.size() < 3)
        {
            stringToSend = ":server 461 " + clients[i]->getNick() + "INVITE :Not enough parameters\n";
            send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
            return;
        }
        if (!clients[i]->checkIfOnChannel(cmd[2]))
        {
            stringToSend = ":server 442 " + cmd[2] + " :You're not on that channel\n";
            send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
            return;
        }
        std::vector<Client*>::iterator it_cl = clients.begin();
        std::vector<Client*>::iterator ite_cl = clients.end();
        while (it_cl != ite_cl)
        {
            if ((*it_cl)->getNick() == cmd[1])
            {
                if ((*it_cl)->checkIfOnChannel(cmd[1]))
                {
                    stringToSend = ":server 443 " + (*it_cl)->getNick() + " " + cmd[2] + " :is already on channel\n";
                    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                    return;
                }
                std::vector<Channel*>::iterator it_ch = channels.begin();
                std::vector<Channel*>::iterator ite_ch = channels.end();
                while (it_ch != ite_ch)
                {
                    if ((*it_ch)->getName() == cmd[2])
                    {
                        if ((*it_ch)->getInviteOnly() && (*it_ch)->getOperatorNick() != clients[i]->getNick())
                        {
                            stringToSend = ":server 482 " + cmd[2] + " :You're not channel operator\n";
                            send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                            return;
                        }
                        stringToSend = ":server 341 " + cmd[2] + " " + (*it_cl)->getNick() + "\n";
                        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                        stringToSend.clear();
                        stringToSend = ":" + clients[i]->getNick() + "!" + clients[i]->getUsername() + "@127.0.0.1 INVITE " + cmd[1] + " :" +  cmd[2] + "\n"; //////отправить всем
                        (*it_ch)->sendToEverybody(stringToSend, clients);
                        send((*it_cl)->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                        return;                        
                    }
                    ++it_ch;
                }
            }
            ++it_cl;
        }
        stringToSend = ":server 401 " + cmd[1] + " :No such nick/channel\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
    }

    void kick(std::vector<std::string> cmd, int i)
    {
        std::string stringToSend;
        if (cmd.size() < 3)
        {
            stringToSend = ":server 461 " + clients[i]->getNick() + "KICK :Not enough parameters\n";
            send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
            return;
        }
        if (!clients[i]->checkIfOnChannel(cmd[1]))
        {
            stringToSend = ":server 442 " + cmd[1] + " :You're not on that channel\n";
            send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
            return;
        }
        std::vector<Client*>::iterator it_cl = clients.begin();
        std::vector<Client*>::iterator ite_cl = clients.end();        
        while (it_cl != ite_cl)
        {
            if ((*it_cl)->getNick() == cmd[2])
            {
                if ((*it_cl)->checkIfOnChannel(cmd[1]))
                {
                    std::vector<Channel*>::iterator it_ch = channels.begin();
                    std::vector<Channel*>::iterator ite_ch = channels.end(); 
                    while (it_ch != ite_ch)
                    {
                        if ((*it_ch)->getName() == cmd[1])
                        {
                            if (clients[i]->getNick() == (*it_ch)->getOperatorNick())
                            {
                                if (cmd.size() == 3)
                                    stringToSend = ":" + clients[i]->getNick() + "!" + clients[i]->getUsername() + "@127.0.0.1 KICK " + cmd[1] + " " +  cmd[2] + " :" + clients[i]->getNick() + "\n"; //////отправить всем
                                else
                                    stringToSend = ":" + clients[i]->getNick() + "!" + clients[i]->getUsername() + "@127.0.0.1 KICK " + cmd[1] + " " +  cmd[2] + " :" + makeStringAfterPrefix(cmd) + "\n"; //////отправить всем
                                (*it_ch)->sendToEverybody(stringToSend, clients);
                                (*it_ch)->removeClient();
                                (*it_cl)->removeChannel(cmd[1]);
                                return;
                            }
                            stringToSend = ":server 482 " + cmd[1] + " :You're not channel operator\n";
                            send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                            return;
                        }
                        ++it_ch;
                    }
                    stringToSend = ":server 403 " + cmd[1] + " :No such channel\n";
                    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                    return;
                }
            }
            ++it_cl;
        }
        stringToSend = ":server 401 " + cmd[1] + " :No such nick/channel\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
    }


    void whichCmd(std::vector<std::string> cmd, int i)
    {
        if (cmd.size() == 0)
            return;
        if (!clients[i]->getIsAutorized())
            getRegistered(cmd, i);
        else
        {
            if (cmd[0] == "PASS")
            {
                if (!clients[i]->setPassword(g_password, cmd[1], clients, i))
                    return;
            }
            if (cmd[0] == "USER")
                clients[i]->setName(cmd, i, clients);
            if (cmd[0] == "NICK")
                clients[i]->setNick(clients, i, cmd[1]);
            if (cmd[0] == "PRIVMSG")
                privmsg(cmd, i, false);
            if (cmd[0] == "AWAY")
                away(cmd, i);
            if (cmd[0] == "NOTICE")
                privmsg(cmd, i, true);
            if (cmd[0] == "USERHOST")
                userhost(cmd, i);
            if (cmd[0] == "OPER")
                oper(cmd, i);
            if (cmd[0] == "JOIN")
                join(cmd, i);
            if (cmd[0] == "PART")
                part(cmd, i);
            if (cmd[0] == "INVITE")
                invite(cmd, i);
            if (cmd[0] == "KICK")
                kick(cmd, i);
        }

    }

    void prepareBuf(char *buf, int i)
    {
        std::string tmp = buf;
        if (tmp.size() == 1)
            return;
        std::cout << buf << std::endl;
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
            whichCmd(cmd, i);           
        }
        if (tmp[tmp.size() - 1] == '\r')
            tmp.erase(tmp.size() - 1);
        cmd = splitString(tmp);
        whichCmd(cmd, i); 
    }

    void launchIrcServer(int port, std::string pass)
    {
        sockaddr_in addr;
        if (port < 1024)
        {
            std::cout << "port is reserved" << std::endl;
            return;
        }
        this->port = port;
        this->g_password = pass;
        this->oper_password = "puk";


        addr.sin_addr.s_addr = INADDR_ANY;
    	addr.sin_port = htons(port);
    	addr.sin_family = AF_INET;
        inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1)
            std::cout << "*" << std::endl;
        if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
            std::cout << "*" << std::endl;
        if (listen(sock, SOMAXCONN) == -1)
            std::cout << "*" << std::endl;
    	fcntl(sock, F_SETFL, O_NONBLOCK); ///////////////////// разобраться, что это. без этого не работает        
            std::vector<struct pollfd> fds;

        signal(SIGINT, &sig);
        signal(SIGQUIT, &sig);

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
                std::cout << "new clientSocket = " << clientSocket << std::endl;
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
                            prepareBuf(buf, i);
                        fds[i].revents = 0;
                    }
                }
            }
        }
    }

};