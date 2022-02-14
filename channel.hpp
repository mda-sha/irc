#ifndef CHANNEL
#define CHANNEL

class Client;
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
#include "client.hpp"

class Channel;
void removeChannel(std::vector<Channel*> &channels, std::string channelName);

class Channel
{
private:
    std::string name;
    bool inviteOnly;
    bool flagT;
    std::string pass;
    std::string topic;
    std::string operatorNick;
    int maxClients;
    int clients;

public:
    std::string getName();
    void setName(std::string name);
    Channel(std::string name);
    bool getT();
    void setT(bool a);
    void setOperatorNick(std::string nick);
    std::string getPass();
    void setPass(std::string pass);
    void removePass();
    std::string getOperatorNick();
    bool getInviteOnly();
    void setInviteOnly(bool a);
    int getMaxClients();
    void setMaxClients(int q);
    int getClients();
    void addClient();
    void removeClient(std::vector<Channel*> &channels);
    std::string getTopic();
    void setTopic(std::string newTopic);
    void removeTopic();
    void sendToEverybody(std::string stringToSend, std::vector<Client*> clients);

};

#endif